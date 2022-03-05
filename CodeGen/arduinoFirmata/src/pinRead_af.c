#include <stdio.h>
#include <pyblock.h>
#include "firmata.h"


extern t_firmata * af_firmataInstance;

int pr_firmataReady = 0;
int pr_pin = -1;
int pr_mode = MODE_INPUT;

static void inout(python_block * block) {
    if (af_firmataInstance == NULL) {
        printf("af - digitalWrite - inout - firmataInstance is empty - waiting\n");
        return;
    }

    while (!af_firmataInstance->isReady) {
        //Wait until device is up
        firmata_pull(af_firmataInstance);
    }

    if (pr_firmataReady == 0 && af_firmataInstance->isReady) {
        pr_firmataReady = 1;

        // init pinRead_af.c
        pr_pin = block->intPar[0];
        pr_mode = block->intPar[1];

        firmata_pinMode(af_firmataInstance, pr_pin, pr_mode);
//        firmata_digitalWrite(af_firmataInstance, pr_pin, LOW); // reset pin to LOW
    }

    if (pr_firmataReady == 0) {
        return;
    }

    firmata_pull(af_firmataInstance);
    if (af_firmataInstance->pins[pr_pin].mode != pr_mode) {
        pr_firmataReady = 0;
        printf("af - pinRead - inout - pin %d is not in correct mode; expected=%d real=%d\n", pr_pin, pr_mode, af_firmataInstance->pins[pr_pin].mode);
        return;
    }


  double * y = block->y[0];
    y[0] = af_firmataInstance->pins[pr_pin].value;
    printf("af - pinRead - inout - pin[%d] = %f; mode=%d\n",pr_pin, y[0], af_firmataInstance->pins[pr_pin].mode);
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
