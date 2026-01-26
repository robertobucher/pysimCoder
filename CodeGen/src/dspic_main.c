#include <xc.h>
#include <stdbool.h>
#include <system.h>

#define XNAME(x,y)  x##y
#define NAME(x,y)   XNAME(x,y)

int NAME(MODEL,_init)(void);
int NAME(MODEL,_isr)(double);
int NAME(MODEL,_end)(void);
double NAME(MODEL,_get_tsamp)(void);

#define FBASE 50000000UL

static double T = 0.0;
static double Tsamp;

volatile bool isrTick = false;

double get_run_time(void)
{
  return(T);
}

double get_Tsamp(void)
{
  return(Tsamp);
}

static void APP_Task(void) {
  if (PORTCbits.RC8)
    LATCbits.LATC8 = 0;
  else
    LATCbits.LATC8 = 1;
}

/* =========================
   Interrupt Timer1
   ========================= */
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void)
{
  isrTick = true;
  IFS1bits.T1IF = 0;
}

/* =========================
   Main
   ========================= */
int main(void)
{
  SYSTEM_Initialize();

  TRISCbits.TRISC8 = 0;
  
  /* ===== Timer1 setup ===== */
  T1CONbits.ON = 0;        // stop timer
  T1CONbits.TCS = 0;       // clock interno (FCY)
  T1CONbits.TCKPS = 0;     // prescaler 1:1
  TMR1 = 0;
    
  Tsamp = NAME(MODEL,_get_tsamp)();
    
  NAME(MODEL,_init)();
    
  PR1  = (int) (FBASE * Tsamp-1);
 
  IFS1bits.T1IF = 0;
  IPC6bits.T1IP = 1;       // priority level 1 (? IPC1!)
  IEC1bits.T1IE = 1;       // enable

  T1CONbits.ON = 1;        // start timer

  /* ===== Loop principale ===== */
  while (1) {
    if (isrTick) {  
      isrTick = false;
      NAME(MODEL,_isr)(T);
      T += Tsamp;
      APP_Task();
    }
  }
}
