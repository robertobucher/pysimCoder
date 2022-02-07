#define FIRMATAC_DEBUG
#include "firmata.h"
#include <stdio.h>

int isHigh = 0;

int main() {
  t_firmata * firmata;
  int i = 0;

  firmata = firmata_new("/dev/ttyACM0");
  while (!firmata -> isReady) {
    firmata_pull(firmata);
  }

  firmata_pinMode(firmata, 8, MODE_OUTPUT);
  firmata_digitalWrite(firmata, 8, LOW);

  firmata_pinMode(firmata, 7, MODE_INPUT);
  firmata_digitalWrite(firmata, 7, LOW);

  while (1) {
    firmata_pull(firmata);

    // this seems like a bug in FirmataC - it needs to be called only once tho
    if (firmata->pins[7].mode != MODE_INPUT) {
      firmata_pinMode(firmata, 7, MODE_INPUT);
      firmata_digitalWrite(firmata, 7, LOW);
    }

    int newHigh = (int) firmata -> pins[7].value;

    if (newHigh != isHigh) {
      isHigh = newHigh;
      firmata_digitalWrite(firmata, 8, isHigh);
    }
  }
}
