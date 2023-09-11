#include <stdio.h>
#include <pyblock.h>
#include "firmata.h"


extern t_firmata * af_firmataInstance;

static void write(int newValue, int pin) {
    if (pin == -1) {
        return;
    }

    /* firmata_pull(af_firmataInstance); */
    firmata_analogWrite(af_firmataInstance, pin, newValue);
}

static void setPinMode(python_block *block) {
  int pin = block->intPar[0];
  int mode = block->intPar[1];

  firmata_pinMode(af_firmataInstance, pin, mode);
  firmata_analogWrite(af_firmataInstance, pin, LOW);
}

static void inout(python_block * block) {
  while (!af_firmataInstance->isReady) {
    firmata_pull(af_firmataInstance);
  }

  setPinMode(block);
  firmata_pull(af_firmataInstance);
  int pin = block->intPar[0];
  int mode = block->intPar[1];

    double * U = block->u[0];
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
