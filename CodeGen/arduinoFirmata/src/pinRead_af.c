#include <stdio.h>
#include <pyblock.h>
#include "firmata.h"


extern t_firmata * af_firmataInstance;

static void setPinMode(python_block *block) {
  int pin = block->intPar[0];
  int mode = block->intPar[1];

  firmata_pinMode(af_firmataInstance, pin, mode);
  // firmata_digitalWrite(af_firmataInstance, pin, LOW); // reset pin to LOW
}

static void inout(python_block * block) {
    if (af_firmataInstance == NULL) {
        printf("af - pinRead - inout - firmataInstance is empty - waiting\n");
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
    } else {
        return;
    }

    firmata_pull(af_firmataInstance);
    if (af_firmataInstance->pins[pin].mode != mode) {
        printf("af - pinRead - inout - pin %d is not in correct mode; expected=%d real=%d\n", pin, mode, af_firmataInstance->pins[pin].mode);
        setPinMode(block);
        return;
    }


  double * y = block->y[0];
    y[0] = af_firmataInstance->pins[pin].value;
//    printf("af - pinRead - inout - pin[%d] = %f; mode=%d\n",pin, y[0], af_firmataInstance->pins[pin].mode);
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
