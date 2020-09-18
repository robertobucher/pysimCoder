/* Electronic Speed Controller (ESC) - Library */

/*
	
*/

#include "ESC.h"

// < Constructor >
/* Sets the proper pin to output.
*/
ESC::ESC(byte ESC_pin, int outputMin, int outputMax, int armVal)
{
	oPin = ESC_pin;
	oMin = outputMin;
	oMax = outputMax;
	oArm = armVal;
}

// < Destructor >
ESC::~ESC()
{
	// Nothing to destruct
}

// calib
/* Calibrate the maximum and minimum PWM signal the ESC is expecting
*/
void ESC::calib(void)
{
	myESC.attach(oPin);  			// attaches the ESC on pin oPin to the ESC object
	myESC.writeMicroseconds(oMax);
		delay(ESC_CAL_DELAY);
	myESC.writeMicroseconds(oMin);
		delay(ESC_CAL_DELAY);
	arm();
}

void ESC::arm(void)
{
	myESC.attach(oPin);  			// attaches the ESC on pin oPin to the ESC object
	myESC.writeMicroseconds (oArm);
}
void ESC::stop(void)
{
	myESC.writeMicroseconds (ESC_STOP_PULSE);
}
void ESC::speed(int outputESC)
{
	oESC = constrain(outputESC, oMin, oMax);
	myESC.writeMicroseconds(oESC);
}