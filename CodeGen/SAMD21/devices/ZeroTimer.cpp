/*
  ZeroTimer.cpp - Arduino Zero M0 Timer Interrput library
  Copyright (c) 2017 Tamasa (@EHbtj).  All right reserved.
*/

#include "Arduino.h"
#include "ZeroTimer.h"

#define CPU_HZ 48000000
#define TIMER_PRESCALER_DIV 1024

void (*func1)();
void (*func2)();
 
void TC_Timer::startTimer(unsigned long period, void (*f)())
{
  REG_GCLK_CLKCTRL = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID (GCM_TCC2_TC3)) ;
  while ( GCLK->STATUS.bit.SYNCBUSY == 1 );

  TcCount16* TC = (TcCount16*) TC3;

  TC->CTRLA.reg &= ~TC_CTRLA_ENABLE;

  // Use the 16-bit timer
  TC->CTRLA.reg |= TC_CTRLA_MODE_COUNT16;
  while (TC->STATUS.bit.SYNCBUSY == 1);

  // Use match mode so that the timer counter resets when the count matches the compare register
  TC->CTRLA.reg |= TC_CTRLA_WAVEGEN_MFRQ;
  while (TC->STATUS.bit.SYNCBUSY == 1);

  setPeriod(period);

  // Enable the compare interrupt
  TC->INTENSET.reg = 0;
  TC->INTENSET.bit.MC0 = 1;

  NVIC_EnableIRQ(TC3_IRQn);

  TC->CTRLA.reg |= TC_CTRLA_ENABLE;
  while (TC->STATUS.bit.SYNCBUSY == 1);

	func1 = f;
} 

void TC_Timer::setPeriod(unsigned long period)
{
	int prescaler;

	TcCount16* TC = (TcCount16*) TC3;

	if (period > 300000) {
		// Set prescaler to 1024
		TC->CTRLA.reg |= TC_CTRLA_PRESCALER_DIV1024;
		prescaler = 1024;
	} else if (80000 < period && period <= 300000) {
		// Set prescaler to 256
		TC->CTRLA.reg |= TC_CTRLA_PRESCALER_DIV256;
		prescaler = 256;
	} else if (20000 < period && period <= 80000) {
		// Set prescaler to 64
		TC->CTRLA.reg |= TC_CTRLA_PRESCALER_DIV64;
		prescaler = 64;
	} else if (10000 < period && period <= 20000) {
		// Set prescaler to 16
		TC->CTRLA.reg |= TC_CTRLA_PRESCALER_DIV16;
		prescaler = 16;
	} else if (5000 < period && period <= 10000) {
		// Set prescaler to 8
		TC->CTRLA.reg |= TC_CTRLA_PRESCALER_DIV8;
		prescaler = 8;
	} else if (2500 < period && period <= 5000) {
		// Set prescaler to 4
		TC->CTRLA.reg |= TC_CTRLA_PRESCALER_DIV4;
		prescaler = 4;
	} else if (1000 < period && period <= 2500) {
		// Set prescaler to 2
		TC->CTRLA.reg |= TC_CTRLA_PRESCALER_DIV2;
		prescaler = 2;
	} else if (period <= 1000) {
		// Set prescaler to 1
		TC->CTRLA.reg |= TC_CTRLA_PRESCALER_DIV1;
		prescaler = 1;
	}
	while (TC->STATUS.bit.SYNCBUSY == 1);

	int compareValue = (int)(CPU_HZ / (prescaler / ((float)period / 1000000))) - 1;

  
  // Make sure the count is in a proportional position to where it was
  // to prevent any jitter or disconnect when changing the compare value.
  TC->COUNT.reg = map(TC->COUNT.reg, 0, TC->CC[0].reg, 0, compareValue);
  TC->CC[0].reg = compareValue;
  while (TC->STATUS.bit.SYNCBUSY == 1);
	
} 

void TC3_Handler()
{
	TcCount16* TC = (TcCount16*) TC3;
  // If this interrupt is due to the compare register matching the timer count
  if (TC->INTFLAG.bit.MC0 == 1) {
    TC->INTFLAG.bit.MC0 = 1;
		(*func1)();
  }
}



void TCC_Timer::startTimer(unsigned long period, void (*f)())
{
	REG_GCLK_CLKCTRL = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID(GCM_TCC0_TCC1));
	while ( GCLK->STATUS.bit.SYNCBUSY == 1 );

  Tcc* TC = (Tcc*) TCC0; // get timer struct
  
  TC->CTRLA.reg &= ~TCC_CTRLA_ENABLE;   // Disable TC
  while (TC->SYNCBUSY.bit.ENABLE == 1); // wait for sync 
	
	setPeriod(period);

  // Use match mode so that the timer counter resets when the count matches the compare register
  TC->WAVE.reg |= TCC_WAVE_WAVEGEN_NFRQ;   // Set wave form configuration 
  while (TC->SYNCBUSY.bit.WAVE == 1); // wait for sync 

  // Enable the compare interrupt
  TC->INTENSET.reg = 0;
	TC->INTENSET.bit.OVF = 1;
  TC->INTENSET.bit.MC0 = 1;

  NVIC_EnableIRQ(TCC0_IRQn);

  TC->CTRLA.reg |= TCC_CTRLA_ENABLE ;
  while (TC->SYNCBUSY.bit.ENABLE == 1); // wait for sync 

	func2 = f;
} 


void TCC_Timer::setPeriod(unsigned long period)
{
	int prescaler;

	Tcc* TC = (Tcc*) TCC0;

	if (period > 300000) {
		// Set prescaler to 1024
		TC->CTRLA.reg |= TCC_CTRLA_PRESCALER_DIV1024;
		prescaler = 1024;
	} else if (80000 < period && period <= 300000) {
		// Set prescaler to 256
		TC->CTRLA.reg |= TCC_CTRLA_PRESCALER_DIV256;
		prescaler = 256;
	} else if (20000 < period && period <= 80000) {
		// Set prescaler to 64
		TC->CTRLA.reg |= TCC_CTRLA_PRESCALER_DIV64;
		prescaler = 64;
	} else if (10000 < period && period <= 20000) {
		// Set prescaler to 16
		TC->CTRLA.reg |= TCC_CTRLA_PRESCALER_DIV16;
		prescaler = 16;
	} else if (5000 < period && period <= 10000) {
		// Set prescaler to 8
		TC->CTRLA.reg |= TCC_CTRLA_PRESCALER_DIV8;
		prescaler = 8;
	} else if (2500 < period && period <= 5000) {
		// Set prescaler to 4
		TC->CTRLA.reg |= TCC_CTRLA_PRESCALER_DIV4;
		prescaler = 4;
	} else if (1000 < period && period <= 2500) {
		// Set prescaler to 2
		TC->CTRLA.reg |= TCC_CTRLA_PRESCALER_DIV2;
		prescaler = 2;
	} else if (period <= 1000) {
		// Set prescaler to 1
		TC->CTRLA.reg |= TCC_CTRLA_PRESCALER_DIV1;
		prescaler = 1;
	}

	int compareValue = (int)(CPU_HZ / (prescaler / ((float)period / 1000000))) - 1;

	TC->PER.reg = compareValue; 
  while (TC->SYNCBUSY.bit.PER == 1);

  // Make sure the count is in a proportional position to where it was
  // to prevent any jitter or disconnect when changing the compare value.
  //TC->COUNT.reg = map(TC->COUNT.reg, 0, TC->CC[0].reg, 0, compareValue);

  TC->CC[0].reg = 0xFFF;
	//TC->CC[0].reg = compareValue;
  while (TC->SYNCBUSY.bit.CC0 == 1);

} 

void TCC0_Handler()
{
	Tcc* TC = (Tcc*) TCC0;       // get timer struct
  
  if (TC->INTFLAG.bit.MC0 == 1) {  // A compare to cc0 caused the interrupt
		TC->INTFLAG.bit.MC0 = 1;    // writing a one clears the flag ovf flag
  }

	if (TC->INTFLAG.bit.OVF == 1) {
		(*func2)();
		TC->INTFLAG.bit.OVF = 1;
	}
}

TC_Timer TC;
TCC_Timer TCC;