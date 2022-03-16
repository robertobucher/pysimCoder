#include <stdio.h>
#include <pyblock.h>
#include <math.h>
//#include "firmata.h"

double ADCREF = -1; // =  1023.0;					// 10-bit resolution for AD converter
double AREF = -1; // =  5.0;                              // ADC reference voltage for 5 V logic
double ARES = -1; // =  AREF/ADCREF;                      // ADC resolution for 5 V logic
double ABSZERO = -1; // =  273.15; // Absolute zero in Celsius
double REF_TEMP = -1; // =  (25.0 + ABSZERO); // Thermistor reference temperature
double NTC_RES = -1; // =  100000.0; // Resistance of the thermistor
double VD_RES = -1; // =  100000.0; // Resistance of voltage divider arm
double VD_REF = -1; // =  5.0; 		// Input for the voltage divider
double NTC_BETA = -1; // =  3950.0; // value of Beta factor from datasheet MF58

// READ
double getThermistorVoltage(python_block *block) {
  double *u = block->u[0];
  return (double)u[0] * ARES;
}

double getThermistorResistance(python_block *block) {
	double Vterm = getThermistorVoltage(block);
	return ((Vterm*VD_RES)/(VD_REF-Vterm));
}

double convertToTemp(python_block *block) {
    return (1 / ((1 / REF_TEMP) + (log(getThermistorResistance(block) / NTC_RES) / NTC_BETA))) - ABSZERO;
}

static void init(python_block *block)
{
  ADCREF = block->realPar[0];
  AREF = block->realPar[1];
  REF_TEMP = block->realPar[2];
  NTC_RES = block->realPar[3];
  VD_RES = block->realPar[4];
  VD_REF = block->realPar[5];
  NTC_BETA = block->realPar[6];
  ABSZERO = block->realPar[7];

  ARES = AREF/ADCREF;

  printf("ADCREF: %f\n", ADCREF);
  printf("AREF: %f\n", AREF);
  printf("ABSZERO: %f\n", ABSZERO);
  printf("REF_TEMP: %f\n", REF_TEMP);
  printf("NTC_RES: %f\n", NTC_RES);
  printf("VD_RES: %f\n", VD_RES);
  printf("VD_REF: %f\n", VD_REF);
  printf("NTC_BETA: %f\n", NTC_BETA);
}

static void inout(python_block *block)
{
  double *y = block->y[0];
  y[0] = convertToTemp(block);
}


void help_HeatShield_temperature(int flag, python_block *block)
{
  if (flag==CG_OUT){
    inout(block);
  } else if (flag == CG_INIT) {
    init(block);
  }
}


