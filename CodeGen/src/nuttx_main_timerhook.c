/****************************************************************************
 * CodeGen/src/nuttx_main_timerhook.c
 *
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText:  Stepan Pressl <pressl.stepan@gmail.com>
 * SPDX-FileContributor:    Stepan Pressl <pressl.stepan@gmail.com>
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 * Author: Stepan Pressl <pressl.stepan@gmail.com>
 *
 ****************************************************************************/

/* Yes. We have some race conditions here. But it's intentional.
 * The thread ending a bit later does not really bother us.
 * If we used locks, we would have some nonnegligable overhead.
 */

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <platform.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <sched.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <poll.h>

#include <nuttx/config.h>
#include <nuttx/timers/timer.h>

#include <pyblock.h>

#ifdef HAVE_MLOCK
#include <sys/mman.h>
#endif

#ifdef CONF_SHV_USED
#ifdef CONF_SHV_UPDATES_USED
#include <nxboot.h>
#include <nuttx/mtd/mtd.h>
#include <sys/boardctl.h>
#include <unistd.h>
#endif
#endif

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#ifndef PYSIM_NUTTX_SAMPLETIMER
#error "You need to define PYSIM_NUTTX_SAMPLETIMER (e.g. /dev/timer0)"
#endif

#define TIMER_DEV       ((const char *)PYSIM_NUTTX_SAMPLETIMER)
#define PERIOD_WATCHDOG ((uint32_t)1000000)
#define LOW_PRIORITY    ((int)50)

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static int timespec_diff_us(struct timespec t1, struct timespec t2);
static void *benchmark_task(void *p);
static void *rt_task(void *p);
static void proc_opt(int argc, char *argv[]);
static void endme(int n);
static void print_usage(void);
#ifdef CONF_SHV_USED
static void shv_my_at_signlr(shv_con_ctx_t *ctx, enum shv_attention_reason r);
#endif

/* Platform dependant model context, use this to store parameters. */

struct pysim_platform_model_ctx
{
  pthread_cond_t pause_barrier;  /* To step the model's execution */
  pthread_mutex_t mutex;         /* Cond needs this */
  volatile int ctrlloopend;      /* Signal to stop the loop */
  int running_state;             /* Running state, use to get */
  bool com_inited;               /* SHV running, no need to initialize again */

  /* Benchmark stats */

  volatile int nloops;           /* The number of loops ran by ctrlloop */
  volatile int maxlat_wakeup;    /* Maximum latency after the timer poll */
  volatile int maxlat_afterisr;  /* Maximum latency after the model computation */
};

/* REVISIT: a global instance is created, for showcase purposes.
 * The idea is to rewrite this in future so there are no global variables,
 * to make running of multiple models possible.
 *
 * REVISIT: Create a function for each platform that creates the model's
 *          instance.
 */
struct pysim_platform_model_ctx NAME(MODEL, _pt_ctx);

/****************************************************************************
 * Private Data
 ****************************************************************************/

static volatile int end = 0;
static double T = 0.0;
static double sampling_period;
static int time_counter = 0;

/* Options presettings */

static char rtversion[] = "0.9";
static int prio = 99;
static int verbose = 0;
static int wait = 0;
static int extclock = 0;
static int benchmark = 0;
static double final_time = 0.0;

#ifdef CANOPEN
void canopen_synch(void);
#endif

static int timespec_diff_us(struct timespec t1, struct timespec t2)
{
  int diff;
  diff = 1000000 * ((int) t1.tv_sec - (int) t2.tv_sec);
  diff += ((int) t1.tv_nsec - (int) t2.tv_nsec) / 1000;
  return diff;
}

static double timespec_diff_s(struct timespec t1, struct timespec t2)
{
  double diff;
  diff = 1.0 * ((long)t1.tv_sec - (long)t2.tv_sec);
  diff += 1e-9 * t1.tv_nsec - 1e-9 * t2.tv_nsec;
  return diff;
}

static void *benchmark_task(void *p)
{
  struct pysim_platform_model_ctx *mctx = (struct pysim_platform_model_ctx*) p;

  /* Set the priority of the task to a low level */

  struct sched_param param;
  param.sched_priority = LOW_PRIORITY;
  pthread_setschedparam(pthread_self(), SCHED_RR, &param);

  while (!end)
    {
      usleep(1000 * 1000);
      printf("Nloops: %d, maxlat_wakeup: %d, maxlat_afterisr: %d\n",
             mctx->nloops, mctx->maxlat_wakeup, mctx->maxlat_afterisr);
    }

  return NULL;
}

#ifdef CONF_SHV_USED
static void shv_my_at_signlr(shv_con_ctx_t *ctx, enum shv_attention_reason r)
{
}
#endif

static void *rt_task(void *p)
{
  int ret;
  int timerfd;
  int loops;
  double tdiff;
  uint32_t maxtimeout;
  uint32_t req_timeout;
  uint32_t lat;
  uint32_t maxlat1;
  uint32_t maxlat2;
  struct timer_notify_s notify;
  struct timer_status_s tstatus;
  struct pollfd fds[1];
  struct sigevent notify_event;
  struct sched_param param;
  struct timespec curtime;
  struct timespec starttime;
  struct timespec T0;
  pthread_t bench_thrd;

  struct pysim_platform_model_ctx *mctx = (struct pysim_platform_model_ctx*) p;
  time_counter = 0;
  mctx->com_inited = false;

  /* initialize sigevent, SIGEV_NONE is sufficient */

  memset(&notify_event, 0, sizeof(struct sigevent));
  notify_event.sigev_notify = SIGEV_NONE;

  /* fill in the timer_notify_s struct */

  notify.periodic = true;
  notify.pid      = getpid();
  notify.event    = notify_event;

  /* Timerhook related data */

  param.sched_priority = prio;
  if (sched_setscheduler(0, SCHED_FIFO, &param) == -1)
    {
      perror("sched_setscheduler failed");
      exit(-1);
    }

  /* Start the benchmark logger here, if needed */

  if (benchmark)
    {
      pthread_create(&bench_thrd, NULL, benchmark_task, p);
    }

  /* Thread creation routines end */

  while (!end)
    {
      timerfd = open(TIMER_DEV, O_RDWR);
      if (timerfd < 0)
        {
          perror("timer open");
          exit(-1);
        }

      /* prepare for poll */

      fds[0].fd = timerfd;
      fds[0].events = POLLIN;

#ifdef HAVE_MLOCK
      mlockall(MCL_CURRENT | MCL_FUTURE);
#endif

      sampling_period = NAME(MODEL, _get_tsamp)();
      req_timeout = (uint32_t) (sampling_period * USEC_PER_SEC);

      ret = ioctl(timerfd, TCIOC_MAXTIMEOUT,
                  (unsigned long)((uintptr_t) &maxtimeout));
      if (ret < 0)
        {
          perror("timer maxtimeout");
          exit(-1);
        }

      if (req_timeout > maxtimeout)
        {
          printf("Can't set such sampling period (maximum = %lu)\n", maxtimeout);
          exit(-1);
        }

      ret = ioctl(timerfd, TCIOC_SETTIMEOUT, (unsigned long)req_timeout);
      if (ret < 0)
        {
          perror("timer settimeout");
          exit(-1);
        }

      ret = ioctl(timerfd, TCIOC_NOTIFICATION,
                  (unsigned long)((uintptr_t) &notify));
      if (ret < 0)
        {
          perror("notify timer");
          exit(-1);
        }


      NAME(MODEL, _init)();
#ifdef CONF_SHV_USED
      if (!mctx->com_inited)
        {
          if (NAME(MODEL, _com_init)(shv_my_at_signlr) == 0)
            {
              mctx->com_inited = true;
            }
        }
#endif

      /* Wait for connections a bit (so the first samples are not delayed) */

      usleep(1000 * 1000);

#ifdef CANOPEN
      canopen_synch();
#endif

      /* start the loop */

      ret = ioctl(timerfd, TCIOC_START);
      if (ret < 0)
        {
          perror("timer start");
          exit(-1);
        }

      /* Save the start time */

      clock_gettime(CLOCK_MONOTONIC, &starttime);
      T = 0;
      T0 = starttime;
      maxlat1 = maxlat2 = 0;
      loops = 0;

      /* The loop now starts */

      mctx->running_state = PYSIM_MODEL_CTRLLOOP_RUNNING;
      puts("CTRLLOOP START");
      while (!mctx->ctrlloopend)
        {
          clock_gettime(CLOCK_MONOTONIC, &curtime);
          T = timespec_diff_s(curtime, T0);
          if (final_time > 0 && T >= final_time)
            {
              break;
            }

          ret = poll(fds, 1, -1);
          if (ret < 0)
            {
              perror("poll timer");
              break;
            }
          if (benchmark)
            {
              loops += 1;
              ret = ioctl(timerfd, TCIOC_GETSTATUS,
                          (unsigned long)((uintptr_t) &tstatus));
              if (ret < 0)
                {
                  perror("timer getstatus");
                  exit(-1);
                }
              lat = tstatus.timeout - tstatus.timeleft;
              if (lat > maxlat1)
                {
                  maxlat1 = lat;
                }
            }

          /* periodic task */

          NAME(MODEL, _isr)(T);

          if (benchmark)
            {
              ret = ioctl(timerfd, TCIOC_GETSTATUS,
                          (unsigned long)((uintptr_t) &tstatus));
              if (ret < 0)
                {
                  perror("timer getstatus");
                  exit(-1);
                }
              lat = tstatus.timeout - tstatus.timeleft;
              if (lat > maxlat2)
                {
                  maxlat2 = lat;
                }

              clock_gettime(CLOCK_MONOTONIC, &curtime);
              if (timespec_diff_us(curtime, starttime) >= USEC_PER_SEC)
                {
                  mctx->nloops = loops;
                  mctx->maxlat_wakeup = maxlat1;
                  mctx->maxlat_afterisr = maxlat2;
                  loops = 0;
                  maxlat1 = 0;
                  maxlat2 = 0;
                  starttime = curtime;
                }
            }

          time_counter += 1;

#ifdef CANOPEN
              canopen_synch();
#endif
        }

      ret = ioctl(timerfd, TCIOC_STOP);
      if (ret < 0)
        {
          perror("timer stop");
          exit(-1);
        }

      NAME(MODEL, _end)();
      close(timerfd);

      /* Restore mctx state */

      mctx->running_state = PYSIM_MODEL_CTRLLOOP_NOTRUNNING;
      mctx->nloops = 0;
      mctx->maxlat_wakeup = 0;
      mctx->maxlat_afterisr = 0;

      if (end)
        {
          break;
        }
      pthread_mutex_lock(&mctx->mutex);
      while (mctx->ctrlloopend)
        {
          pthread_cond_wait(&mctx->pause_barrier, &mctx->mutex);
        }
      pthread_mutex_unlock(&mctx->mutex);
      puts("CTRLLOOP resumed");
    }
#ifdef CONF_SHV_USED
  if (mctx->com_inited)
    {
      NAME(MODEL, _com_end)();
    }
#endif
  if (benchmark)
    {
      pthread_join(bench_thrd, NULL);
    }
  pthread_exit(0);
}

static void endme(int n)
{
  end = 1;
  NAME(MODEL, _pt_ctx).ctrlloopend = 1;
}

static void print_usage(void)
{
  puts(
    "\nUsage:  'RT-model-name' [OPTIONS]\n"
    "\n"
    "OPTIONS:\n"
    "  -h  print usage\n"
    "  -f <final time> set the final time of the execution\n"
    "  -v  verbose output\n"
    "  -p <priority>  set rt task priority (default 99)\n"
    "  -b  model timing benchmark, each second the number of model\n"
    "      execution is printed, alongside worst case latencies\n"
    "  -e  external clock\n"
    "  -w  wait to start\n"
    "  -V  print version\n"
    "\n");
}

static void proc_opt(int argc, char *argv[])
{
  int i;
  while ((i = getopt(argc, argv, "ef:hp:vVwb")) != -1)
    {
      switch (i)
        {
          case 'h':
            print_usage();
            exit(0);
            break;
          case 'p':
            prio = atoi(optarg);
            break;
          case 'v':
            verbose = 1;
            break;
          case 'w':
            wait = 1;
            break;
          case 'b':
            benchmark = 1;
            break;
          case 'V':
            printf("Version %s\n", rtversion);
            exit(0);
            break;
          case 'f':
            break;
        }
    }
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: get_Tsamp
 *
 * Description:
 *   Get model sampling period.
 *
 ****************************************************************************/

double get_Tsamp()
{
  return sampling_period;
}

double get_run_time()
{
  return time_counter * sampling_period;
}

/* Temporary fix. REVISIT: replace with NAME(MODEL, _comprio) */
int get_priority_for_com(void)
{
  if (prio < 0)
    {
      return -1;
    }
  else
    {
      return prio - 1;
    }
}

/* Staging change only, for now. The passed pointer is to be used
 * for future purposes, to get rid of global variables and to be generic.
 */

double NAME(MODEL, _runtime)(struct pysim_platform_model_ctx *ctx)
{
  return time_counter * sampling_period;
}

int NAME(MODEL, _comprio)(struct pysim_platform_model_ctx *ctx)
{
  if (prio < 0)
    {
      return -1;
    }
  else
    {
      return prio - 1;
    }
}

void NAME(MODEL, _pausectrl)(struct pysim_platform_model_ctx *ctx)
{
  ctx->ctrlloopend = 1;
  puts("PAUSING CTRLLOOP");
}

void NAME(MODEL, _resumectrl)(struct pysim_platform_model_ctx *ctx)
{
  puts("RESUMING CTRLLOOP");
  pthread_mutex_lock(&ctx->mutex);
  ctx->ctrlloopend = 0;
  pthread_cond_signal(&ctx->pause_barrier);
  pthread_mutex_unlock(&ctx->mutex);
}

int NAME(MODEL, _getctrlstate)(struct pysim_platform_model_ctx *ctx)
{
  return ctx->running_state;
}

#ifdef CONF_SHV_USED
#ifdef CONF_SHV_UPDATES_USED
static int _shv_nxboot_opener(shv_file_node_t *item)
{
  struct shv_file_node_fctx *fctx = (struct shv_file_node_fctx*) item->fctx;
  if (!(fctx->flags & SHV_FILE_POSIX_BITFLAG_OPENED))
    {
      fctx->fd = nxboot_open_update_partition();
      if (fctx->fd < 0)
        {
          return -1;
        }
      fctx->flags |= SHV_FILE_POSIX_BITFLAG_OPENED;
    }
  return 0;
}

/* This function provides the default initialization of the update file node
 * for NuttX and allows for NXBoot integration.
 */
int shv_init_fwupdate(struct pysim_platform_model_ctx *ctx,
                      shv_file_node_t *file_node)
{
  if (ctx == NULL || file_node == NULL)
    {
      return -1;
    }

  /* Default to NXBoot if we have it defined */

#ifdef CONFIG_BOOT_NXBOOT
  struct mtd_geometry_s geometry;
  int flash_fd = nxboot_open_update_partition();
  if (flash_fd < 0)
    {
      return -1;
    }
  if (ioctl(flash_fd, MTDIOC_GEOMETRY,
            (unsigned long)((uintptr_t)&geometry)) < 0)
    {
      close(flash_fd);
      return -1;
    }

  file_node->file_type = SHV_FILE_MTD;
  file_node->file_maxsize = geometry.erasesize * geometry.neraseblocks;
  file_node->file_pagesize = geometry.blocksize;
  file_node->file_erasesize = geometry.erasesize;

  file_node->fops.opener = _shv_nxboot_opener;
#endif

  return 0;
}

int shv_init_fwstable(struct pysim_platform_model_ctx *ctx,
                      struct shv_fwstable_node *fwstable_node)
{
  if (ctx == NULL || fwstable_node == NULL)
    {
      return -1;
    }

#ifdef CONFIG_BOOT_NXBOOT
  fwstable_node->ops.confirm = nxboot_confirm;
#endif
  return 0;
}
#endif /* CONF_SHV_UPDATES_USED */
#endif /* CONF_SHV_USED */

int main(int argc, char** argv)
{
  pthread_t thrd;

  /* Initialize the context variables */

  NAME(MODEL, _pt_ctx).ctrlloopend = 0;
  NAME(MODEL, _pt_ctx).running_state = PYSIM_MODEL_CTRLLOOP_NOTRUNNING;

  pthread_mutex_init(&NAME(MODEL, _pt_ctx).mutex, NULL);
  pthread_cond_init(&NAME(MODEL, _pt_ctx).pause_barrier, NULL);

  proc_opt(argc, argv);
  signal(SIGINT, endme);
  signal(SIGKILL, endme);

  pthread_create(&thrd, NULL, rt_task, &NAME(MODEL, _pt_ctx));
  pthread_join(thrd, NULL);
  return 0;
}
