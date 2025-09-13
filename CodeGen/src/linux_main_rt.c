#include <platform.h>
#include <pyblock.h>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sched.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>
#include <getopt.h>

#ifdef CG_WITH_IOPL
#include <sys/io.h>
#endif

#ifdef CANOPEN
void canopen_synch(void);
#endif

#define NSEC_PER_SEC  1000000000
#define USEC_PER_SEC	1000000
#define XSTRIFY(x)        #x
#define STRIFY(x)         XSTRIFY(x)

/* This is a discrimination against nonASCII encodings!!!!!! */

#define SHV_DEVID_FLAG  1000
#define SHV_IPADDR_FLAG 1001
#define SHV_MOUNT_FLAG  1002
#define SHV_PASSWD_FLAG 1003
#define SHV_PORT_FLAG   1004
#define SHV_USER_FLAG   100

static volatile int end = 0;
static double T = 0.0;
static double Tsamp;

/* Options presettings */
static char rtversion[] = "0.9";
static int prio = 99;
static int verbose = 0;
static int wait = 0;
static int extclock = 0;
double FinalTime = 0.0;

static const struct option optargs[] =
{
  {"benchmark", no_argument, 0, 'b'},
  {"ext-clock", no_argument, 0, 'e'},
  {"final-time", required_argument, 0, 'f'},
  {"help", no_argument, 0, 'h'},
  {"prio", required_argument, 0, 'p'},
  {"verbose", no_argument, 0, 'v'},
  {"version", no_argument, 0, 'V'},

  /* Let SHV parameters be only of the long type.
   * Let's keep basic letters reserved for stuff related more to the model.
   * The number identifiers shouldn't be accessed by normal Ascii coding :)
   */

  {"shv-devid", required_argument, 0, SHV_DEVID_FLAG},
  {"shv-ipaddr", required_argument, 0, SHV_IPADDR_FLAG},
  {"shv-mount", required_argument, 0, SHV_MOUNT_FLAG},
  {"shv-passwd", required_argument, 0, SHV_PASSWD_FLAG},
  {"shv-port", required_argument, 0, SHV_PORT_FLAG},
  {"shv-user", required_argument, 0, SHV_USER_FLAG},
  {0, 0, 0, 0}
};

/* Platform dependant model context, use this to store parameters. */

struct pysim_platform_model_ctx
{
  pthread_cond_t pause_barrier;  /* To step the model's execution */
  pthread_mutex_t mutex;         /* Cond needs this */
  volatile int ctrlloopend;      /* Signal to stop the loop */
  int running_state;             /* Running state, use to get */
  bool com_inited;               /* SHV running, no need to initialize again */
};

/* REVISIT: a global instance is created, for showcase purposes.
 * The idea is to rewrite this in future so there are no global variables,
 * to make running of multiple models possible.
 *
 * REVISIT: Create a function for each platform that creates the model's
 *          instance.
 */
struct pysim_platform_model_ctx NAME(MODEL, _pt_ctx);

#ifdef CONF_SHV_USED
static void shv_my_at_signlr(struct shv_con_ctx *ctx, enum shv_attention_reason r)
{
}
#endif


double get_run_time(void)
{
  return(T);
}

double get_Tsamp(void)
{
  return(Tsamp);
}

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

static inline void tsnorm(struct timespec *ts)
{
  while (ts->tv_nsec >= NSEC_PER_SEC) {
    ts->tv_nsec -= NSEC_PER_SEC;
    ts->tv_sec++;
  }
}

static inline double calcdiff(struct timespec t1, struct timespec t2)
{
  long diff;
  diff = USEC_PER_SEC * ((long) t1.tv_sec - (long) t2.tv_sec);
  diff += ((int) t1.tv_nsec - (int) t2.tv_nsec) / 1000;
  return (1e-6*diff);
}

static void *rt_task(void *p)
{
  struct timespec t_next, t_current, t_isr, T0;
  struct sched_param param;
  struct pysim_platform_model_ctx *mctx = (struct pysim_platform_model_ctx*) p;
  mctx->com_inited = false;

  if (prio >= 0) {
    param.sched_priority = prio;
    if(sched_setscheduler(0, SCHED_FIFO, &param)==-1) {
      perror("sched_setscheduler failed");
      exit(-1);
    }
  }

  mlockall(MCL_CURRENT | MCL_FUTURE);

  while (!end) {
    Tsamp = NAME(MODEL,_get_tsamp)();

    t_isr.tv_sec =  0L;
    t_isr.tv_nsec = (long)(1e9*Tsamp);
    tsnorm(&t_isr);

    T=0;

    NAME(MODEL,_init)();

#ifdef CONF_SHV_USED
    if (!mctx->com_inited) {
      if (NAME(MODEL, _com_init)(shv_my_at_signlr) == 0) {
        mctx->com_inited = true;
      }
    }
#endif
    /* Wait for connections a bit (so the first samples are not delayed) */
    usleep(1000 * 1000);
#ifdef CANOPEN
    canopen_synch();
#endif

    /* get current time */
    clock_gettime(CLOCK_MONOTONIC,&t_current);
    T0 = t_current;

    mctx->running_state = PYSIM_MODEL_CTRLLOOP_RUNNING;
    puts("CTRLLOOP START");
    while (!mctx->ctrlloopend){

      /* periodic task */
      T = calcdiff(t_current,T0);
      NAME(MODEL,_isr)(T);

#ifdef CANOPEN
      canopen_synch();
#endif

      if((FinalTime >0) && (T >= FinalTime)) break;

      t_next.tv_sec = t_current.tv_sec + t_isr.tv_sec;
      t_next.tv_nsec = t_current.tv_nsec + t_isr.tv_nsec;
      tsnorm(&t_next);

      /* Check if Overrun */
      clock_gettime(CLOCK_MONOTONIC,&t_current);
      if (t_current.tv_sec > t_next.tv_sec ||
    (t_current.tv_sec == t_next.tv_sec && t_current.tv_nsec > t_next.tv_nsec)) {
        int usec = (t_current.tv_sec - t_next.tv_sec) * 1000000 + (t_current.tv_nsec -
                   t_next.tv_nsec)/1000;
        fprintf(stderr, "Base rate overrun by %d us\n", usec);
        t_next= t_current;
      }
      clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t_next, NULL);
      t_current = t_next;
    }
    NAME(MODEL,_end)();
    mctx->running_state = PYSIM_MODEL_CTRLLOOP_NOTRUNNING;
    if (end) {
      break;
    }
    pthread_mutex_lock(&mctx->mutex);
    while (mctx->ctrlloopend) {
      pthread_cond_wait(&mctx->pause_barrier, &mctx->mutex);
    }
    pthread_mutex_unlock(&mctx->mutex);
  }
#ifdef CONF_SHV_USED
  if (mctx->com_inited) {
    NAME(MODEL, _com_end)();
  }
#endif
  pthread_exit(0);
}

void endme(int n)
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
    "  -e --ext-clock: external clock (what is this actually for?)\n"
    "  -f --final-time <val> set model's final time to val\n"
    "  -h --help: print usage\n"
    "  -p --prio <val>: set rt task priority to val (default 99)\n"
    "  -v --verbose: verbose output\n"
    "  -V --version: print version\n"
    " --shv-devid <dev-id>: set the device's name in SHV\n"
    " --shv-ipaddr <ip>: set the broker's IPv4\n"
    " --shv-mount <mount>: device's mount point in SHV\n"
    " --shv-passwd <passwd>: password to access the broker\n"
    " --shv-port <port>: set the broker's port\n"
    " --shv-user <user> user to access the broker");
}

char *parse_string(char ** str, int parse_char)
{
  char *p = strdup(*str);
  char *r = p;
  char *s = strchr(r, parse_char);
  char *t = r;

  if (s == NULL)
    {
      return p;
    }

  *s = '\0';
  r = s + 1;

  *str = r;

  return t;
}

static void proc_opt(int argc, char *argv[])
{
  int i;
  char *t;

  while((i=getopt_long(argc,argv,"ef:hp:vVw",optargs,NULL))!=-1){
    switch(i){
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
    case 'e':
      extclock = 1;
      break;
    case 'w':
      wait = 1;
      break;
    case 'V':
      printf("Version %s\n",rtversion);
      exit(0);
      break;
    case 'f':
      if (strstr(optarg, "inf")) {
        FinalTime = 0.0;
      } else if ((FinalTime = atof(optarg)) <= 0.0) {
        printf("-> Invalid final time.\n");
        exit(1);
      }
      break;
    case SHV_DEVID_FLAG:
      printf("Setting here!");
      setenv("CONF_SHV_BROKER_DEV_ID", optarg, 1);
      break;
    case SHV_IPADDR_FLAG:
      setenv("CONF_SHV_BROKER_IP", optarg, 1);
      break;
    case SHV_MOUNT_FLAG:
      setenv("CONF_SHV_BROKER_MOUNT", optarg, 1);
      break;
    case SHV_PASSWD_FLAG:
      setenv("CONF_SHV_BROKER_PASSWORD", optarg, 1);
      break;
    case SHV_PORT_FLAG:
      setenv("CONF_SHV_BROKER_PORT", optarg, 1);
      break;
    case SHV_USER_FLAG:
      setenv("CONF_SHV_BROKER_USER", optarg, 1);
      break;
    case '?':
      print_usage();
      exit(1);
      break;
    default:
      break;
    }
  }
}

/* Staging change only, for now. The passed pointer is to be used
 * for future purposes, to get rid of global variables and to be generic.
 */

double NAME(MODEL, _runtime)(struct pysim_platform_model_ctx *ctx)
{
  return T;
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
/* This function provides the default initialization
 * of the file node on LinuxRT, the updated file name is
 * updatedexe_MODEL
 */
int shv_init_fwupdate(struct pysim_platform_model_ctx *ctx,
                      struct shv_file_node *file_node)
{
  if (ctx == NULL || file_node == NULL) {
      return -1;
  }

  file_node->name           = STRIFY(NAME(updatedexe_, MODEL));
  file_node->file_type      = SHV_FILE_REGULAR;
  file_node->file_maxsize   = (1 << 27); /* 12& MB */
  file_node->file_pagesize  = 1024;

  return 0;
}

/* This does nothing on Linux */
int shv_init_fwstable(struct pysim_platform_model_ctx *ctx,
                      struct shv_fwstable_node *fwstable_node)
{
  if (ctx == NULL || fwstable_node == NULL) {
      return -1;
  }
  return 0;
}
#endif /* CONF_SHV_UPDATES_USED */
#endif /* CONF_SHV_USED */

int main(int argc,char** argv)
{
  pthread_t thrd;
  int fd;
  int uid;

  proc_opt(argc, argv);

  signal(SIGINT,endme);
  signal(SIGKILL,endme);

#ifdef CG_WITH_NRT
  uid = geteuid();
  if (uid!=0){
    fd=open("/dev/nrt",O_RDWR);
    if (fd<0){
      printf("This SW needs superuser privilegies to run!\n");
      exit(1);
    }
    close(fd);
  }
#endif /*CG_WITH_NRT*/

#ifdef CG_WITH_IOPL
  iopl(3);
#endif /*CG_WITH_IOPL*/

  pthread_create(&thrd,NULL,rt_task,&NAME(MODEL, _pt_ctx));

  pthread_join(thrd,NULL);
  return(0);
}

