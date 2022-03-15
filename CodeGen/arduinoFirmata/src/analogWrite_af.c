#include <stdio.h>
#include <pyblock.h>
#include "firmata.h"


extern t_firmata * af_firmataInstance;

static void write(int newValue, int pin) {
    if (pin == -1) {
        printf("af - analogWrite - pin not set\n");
        return;
    }

    firmata_pull(af_firmataInstance);
    firmata_analogWrite(af_firmataInstance, pin, newValue);
}

static void setPinMode(python_block *block) {
  int pin = block->intPar[0];
  int mode = block->intPar[1];

  firmata_pinMode(af_firmataInstance, pin, mode);
  firmata_analogWrite(af_firmataInstance, pin, LOW);
}

static void inout(python_block * block) {
    if (af_firmataInstance == NULL) {
        printf("af - analogWrite - inout - firmataInstance is empty - waiting\n");
        return;
    }

    while (!af_firmataInstance->isReady) {
        //Wait until device is up
        firmata_pull(af_firmataInstance);
    }

    int pin = block->intPar[0];
    int mode = block->intPar[1];

    if (af_firmataInstance->isReady) {
        setPinMode(block);
    } else  {
      return;
    }

    firmata_pull(af_firmataInstance);
    if (af_firmataInstance->pins[pin].mode != mode) {
        printf("af - analogWrite - inout - pin %d is not in correct mode; expected=%d real=%d\n", pin, mode, af_firmataInstance->pins[pin].mode);
        setPinMode(block);
        return;
    }

    double * U = block->u[0];
//    printf("af - analogWrite - inout - pin[%d] = %f; mode=%d\n", pin, U[0], af_firmataInstance->pins[pin].mode);

    int pinVal = (int) U[0];
    write(pinVal, pin);

}

static void end(python_block * block) {
  int pin = block->intPar[0];
  write(0, pin);
}

void analogWrite_af(int flag, python_block * block) {
    if (flag == CG_OUT) {
        /* get input */
        inout(block);
    } else if (flag == CG_STUPD) {
    } else if (flag == CG_END) {
        /* termination */
        end(block);
    } else if (flag == CG_INIT) {
    }
}
