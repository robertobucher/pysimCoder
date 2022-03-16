#include "firmata.h"
#include <math.h>
#include <stdio.h>

#define ADCREF 1023.0					// 10-bit resolution for AD converter
#define AREF 5.0                              // ADC reference voltage for 5 V logic
#define ARES AREF/ADCREF                      // ADC resolution for 5 V logic


//arduino uno D1-14 -> A0-A5
//       pins  0-13 ^^ 14-19
#define HEAT_YPIN 14 // A0=14 // NTC Thermistor (Sensor)
#define HEAT_UPIN 3 // D3=3 // Cartridge (Actuator)
#define REF_TEMP (25.0 + ABSZERO) // Thermistor reference temperature
#define NTC_RES 100000.0 // Resistance of the thermistor
#define VD_RES 100000.0 // Resistance of voltage divider arm
#define VD_REF 5.0 		// Input for the voltage divider
#define NTC_BETA 3950.0 // value of Beta factor from datasheet MF58
#define ABSZERO 273.15 // Absolute zero in Celsius


// READ
float getThermistorVoltage(t_firmata *firmata) {
    firmata_pull(firmata);
    uint32_t v = firmata->pins[HEAT_YPIN].value;
    printf("raw=%d\n", v);
    return (float)v * ARES;
}

float getThermistorResistance(t_firmata *firmata) {
	float Vterm = getThermistorVoltage(firmata);
	return ((Vterm*VD_RES)/(VD_REF-Vterm));
}

float sensorRead(t_firmata *firmata) {
    return (1 / ((1 / REF_TEMP) + (log(getThermistorResistance(firmata) / NTC_RES) / NTC_BETA))) - ABSZERO;
}


// WRITE
float percToPwm(float perc){
	float percFloat = perc*2.55;
	return percFloat;
}

void actuatorWrite(t_firmata *firmata, float percent) {
    firmata_analogWrite(firmata, HEAT_UPIN, percToPwm(percent));
}

int main() {
  t_firmata * firmata;
  int i = 10000;

  firmata = firmata_new("/dev/ttyACM0"); //init Firmata
  while (!firmata -> isReady) {
    firmata_pull(firmata);
  }

  firmata_pinMode(firmata, HEAT_UPIN, MODE_PWM); // Cartridge
  firmata_pinMode(firmata, HEAT_YPIN, MODE_ANALOG);  // Thermistor pin

  while (i > 0) {
    firmata_pull(firmata);

    // this seems like a bug in FirmataC - it needs to be called only once tho
    if (firmata->pins[HEAT_UPIN].mode != MODE_PWM) {
      firmata_pinMode(firmata, HEAT_UPIN, MODE_PWM);
      firmata_analogWrite(firmata, HEAT_UPIN, LOW);
      printf("PWM mode set\n");
    }

    // this seems like a bug in FirmataC - it needs to be called only once tho
    if (firmata->pins[HEAT_YPIN].mode != MODE_ANALOG) {
      firmata_pinMode(firmata, HEAT_YPIN, MODE_ANALOG);
      printf("ANALOG mode set\n");
    }

    actuatorWrite(firmata, 100);

    float val = sensorRead(firmata);
    float volatage = getThermistorVoltage(firmata);
    printf("READ = %f C; VOLTAGE = %f; yMode: %d; uMode: %d\n",
        val,
        volatage,
        firmata->pins[HEAT_YPIN].mode,
        firmata->pins[HEAT_UPIN].mode
    );

    i = i - 1;
    sleep(1);
  }
}

