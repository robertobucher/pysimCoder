#include <stdio.h>
#include <pyblock.h>
#include "firmata.h"


extern t_firmata * af_firmataInstance;

static void setPinMode(python_block *block) {
  int pin = block->intPar[0];
  int mode = block->intPar[1];

  firmata_pinMode(af_firmataInstance, pin, mode);
}

static void inout(python_block * block) {
  if (af_firmataInstance->isReady) {
    firmata_pull(af_firmataInstance);
  } else {
    return;
  }

  setPinMode(block);
  firmata_pull(af_firmataInstance);
  int pin = block->intPar[0];
  double * y = block->y[0];
  y[0] = af_firmataInstance->pins[pin].value;
}

void pinRead_af(int flag, python_block * block) {
    if (flag == CG_OUT) {
        /* get input */
        inout(block);
    } else if (flag == CG_STUPD) {
    } else if (flag == CG_END) {
    } else if (flag == CG_INIT) {     
    }
}
