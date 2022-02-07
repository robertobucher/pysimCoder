#include <stdio.h>
#include <pyblock.h>
#include "firmata.h"


extern t_firmata * af_firmataInstance;

int dr_firmataReady = 0;
int dr_pin = -1;

static void inout(python_block * block) {
    if (af_firmataInstance == NULL) {
        printf("af - digitalWrite - inout - firmataInstance is empty - waiting\n");
        return;
    }

    while (!af_firmataInstance->isReady) {
        //Wait until device is up
        firmata_pull(af_firmataInstance);
    }

    if (dr_firmataReady == 0 && af_firmataInstance->isReady) {
        dr_firmataReady = 1;

        // init digitalWrite
        dr_pin = block->intPar[0];
        firmata_pinMode(af_firmataInstance, dr_pin, MODE_INPUT);
        firmata_digitalWrite(af_firmataInstance, dr_pin, LOW); // reset pin to LOW
    }

    if (dr_firmataReady == 0) {
        return;
    }

    firmata_pull(af_firmataInstance);
    if (af_firmataInstance->pins[dr_pin].mode != MODE_INPUT) {
        dr_firmataReady = 0;
        printf("af - digitalWrite - inout - pin %d is not in input mode\n", dr_pin);
        return;
    }


  double *y = block->y[0];
    y[0] = af_firmataInstance->pins[dr_pin].value;
    printf("af - digitalRead - inout - Y[0] = %f\n", y[0]);
}

void digitalRead_af(int flag, python_block * block) {
    if (flag == CG_OUT) {
        /* get input */
        inout(block);
    } else if (flag == CG_STUPD) {
    } else if (flag == CG_END) {
    } else if (flag == CG_INIT) {
    }
}
