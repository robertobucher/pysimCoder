#include <myFun.h>

#define uint32_t unsigned int
extern int txLen;

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

#ifdef DEBUG
  setPinMode(11, 2);
  writeDO(11, 0.0);
#endif
  
  Tsamp = NAME(MODEL,_get_tsamp)();
  NAME(MODEL,_init)();

  /* Communication to ESP32  */
  initIPC();
  
  /* Start ISR Timer  */
  period = (unsigned long) (Tsamp*1000000);
  setTimer(period, call);
}

void loop()
{
  if(txLen) sendIPC();
 }

void call()
{
#ifdef DEBUG
    writeDO(11, 1.0);
#endif
    
     NAME(MODEL,_isr)(T);
    T += Tsamp;
    
#ifdef DEBUG
   writeDO(11, 0.0);
#endif
}
