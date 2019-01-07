#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#define XNAME(x,y)  x##y
#define NAME(x,y)   XNAME(x,y)

int NAME(MODEL,_init)(void);
int NAME(MODEL,_isr)(double);
int NAME(MODEL,_end)(void);
double NAME(MODEL,_get_tsamp)(void);

static volatile int end = 0;
static double T = 0.0;

/* Options presettings */
static char rtversion[] = "0.9";
static int prio = 99;
static int verbose = 0;
static int extclock = 0;
static int wait = 0;
double FinalTime = 0.0;

double get_run_time()
{
  return(T);
}

void endme(int n)
{
  end = 1;
}

void print_usage(void)
{
  puts(  "\nUsage:  'RT-model-name' [OPTIONS]\n"
         "\n"
         "OPTIONS:\n"
         "  -h  print usage\n"
	 "  -v  verbose output\n"
	 "  -p <priority>  set rt task priority (default 99)\n"
	 "  -e  external clock\n"
	 "  -w  wait to start\n"
	 "  -V  print version\n"
	 "\n");
}

static void proc_opt(int argc, char *argv[])
{
  int i;
  while((i=getopt(argc,argv,"ef:hp:vVw"))!=-1){
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
    }
  }
}

int main(int argc,char** argv)
{
  double Tsamp = NAME(MODEL,_get_tsamp)();

  proc_opt(argc, argv);

  signal(SIGINT,endme);
  signal(SIGKILL,endme);

  T=0;

  NAME(MODEL,_init)();

  while(!end){
    /* periodic task */
    NAME(MODEL,_isr)(T);

    /* calculate next shot */
    T+=Tsamp;
    /* Check task end */
    if((FinalTime >0) && (T >= FinalTime)) break;
  }
  NAME(MODEL,_end)();  
}

