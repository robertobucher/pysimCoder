#include <myFun.h>

#define XNAME(x,y)  x##y
#define NAME(x,y)   XNAME(x,y)

int NAME(MODEL,_init)(void);
int NAME(MODEL,_isr)(double);
int NAME(MODEL,_end)(void);
double NAME(MODEL,_get_tsamp)(void);

static double T = 0.0;
static double Tsamp;

double get_run_time()
{
  return(T);
}

double get_Tsamp()
{
  return(Tsamp);
}

void call();

void setup()
{
  unsigned long period;
  
  Tsamp = NAME(MODEL,_get_tsamp)();
  NAME(MODEL,_init)();
  period = (unsigned long) (Tsamp*1000000);
  setTimer(period, call);
}

void loop()
{
}

void call()
{
    NAME(MODEL,_isr)(T);
    T += Tsamp;
}
