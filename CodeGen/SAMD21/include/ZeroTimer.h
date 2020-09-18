/*
  ZeroTimer.h - Arduino Zero M0 Timer Interrput library
  Copyright (c) 2017 Tamasa (@EHbtj).  All right reserved.
*/

#ifndef ZeroTimer_h
#define ZeroTimer_h

class TC_Timer
{
  public:
		void startTimer(unsigned long period, void (*f)());
		void setPeriod(unsigned long period);
};

class TCC_Timer
{
  public:
		void startTimer(unsigned long period, void (*f)());
		void setPeriod(unsigned long period);
};

extern TC_Timer TC;
extern TCC_Timer TCC;

#endif

