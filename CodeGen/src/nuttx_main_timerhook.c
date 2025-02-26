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

/****************************************************************************
 * Included Files
 ****************************************************************************/

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

#ifdef HAVE_MLOCK
#include <sys/mman.h>
#endif

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define XNAME(x,y)  x##y
#define NAME(x,y)   XNAME(x,y)

#ifndef PYSIM_NUTTX_SAMPLETIMER
#error "You need to define PYSIM_NUTTX_SAMPLETIMER (e.g. /dev/timer0)"
#endif

#define TIMER_DEV       ((const char *)PYSIM_NUTTX_SAMPLETIMER)
#define PERIOD_WATCHDOG ((uint32_t)1000000)
#define LOW_PRIORITY    ((int)50)

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

int NAME(MODEL, _init)(void);
int NAME(MODEL, _isr)(double);
int NAME(MODEL, _end)(void);
double NAME(MODEL, _get_tsamp)(void);

static int timespec_diff_us(struct timespec t1, struct timespec t2);
static void *benchmark_task(void *p);
static void *rt_task(void *p);
static void proc_opt(int argc, char *argv[]);
static void endme(int n);
static void print_usage(void);

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

/* For RT task benchmark */

volatile static int nloops = 0;
volatile static int maxlat_wakeup = 0;
volatile static int maxlat_afterisr = 0;

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
  /* set the priority of the task to a low level */

  struct sched_param param;
  param.sched_priority = LOW_PRIORITY;
  pthread_setschedparam(pthread_self(), SCHED_RR, &param);

  while (!end)
    {
      usleep(1000 * 1000);
      printf("Nloops: %d, maxlat_wakeup: %d, maxlat_afterisr: %d\n",
             nloops, maxlat_wakeup, maxlat_afterisr);
    }

  return NULL;
}

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

  time_counter = 0;

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

  /* start the benchmark logger, if needed */

  if (benchmark)
    {
      pthread_create(&bench_thrd, NULL, benchmark_task, NULL);
    }

  NAME(MODEL, _init)();

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

  while (!end)
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
              nloops = loops;
              maxlat_wakeup = maxlat1;
              maxlat_afterisr = maxlat2;
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
  if (benchmark)
    {
      pthread_join(bench_thrd, NULL);
    }
  pthread_exit(0);
}

static void endme(int n)
{
  end = 1;
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
    "      execution is printed, alongside worst case latencies"
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
 * Name: get_run_time
 *
 * Description:
 *   Get current run time.
 *
 ****************************************************************************/

double get_run_time()
{
  return time_counter * sampling_period;
}

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

/****************************************************************************
 * Name: get_Tsamp
 *
 * Description:
 *   Get priority for a communication task
 *
 ****************************************************************************/

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

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, char** argv)
{
  pthread_t thrd;
  int fd;

  proc_opt(argc, argv);

  signal(SIGINT, endme);
  signal(SIGKILL, endme);

  pthread_create(&thrd, NULL, rt_task, NULL);

  pthread_join(thrd, NULL);
  return 0;
}
