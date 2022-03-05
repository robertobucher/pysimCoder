#include <stdio.h>
#include <pyblock.h>
#include "firmata.h"


extern t_firmata * af_firmataInstance;

int aw_firmataReady = 0;
int aw_pin = -1;
int aw_pinValue = 0;
int aw_mode = MODE_ANALOG;

static void write(int newValue) {
    if (aw_pin == -1) {
        printf("af - analogWrite - pin not set\n");
        return;
    }

    if (newValue == aw_pinValue) {
//        return;
    }

    aw_pinValue = newValue;

    firmata_pull(af_firmataInstance);
    firmata_analogWrite(af_firmataInstance, aw_pin, newValue);
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

    if (aw_firmataReady == 0 && af_firmataInstance->isReady) {
        aw_firmataReady = 1;

        // init analogWrite_af.c
        aw_pin = block->intPar[0];
        aw_mode = block->intPar[1];

        firmata_pinMode(af_firmataInstance, aw_pin, aw_mode);
        firmata_analogWrite(af_firmataInstance, aw_pin, LOW);
    }

    if (aw_firmataReady == 0) {
        return;
    }

    firmata_pull(af_firmataInstance);
    if (af_firmataInstance->pins[aw_pin].mode != aw_mode) {
        aw_firmataReady = 0;
        printf("af - analogWrite - inout - pin %d is not in correct mode; expected=%d real=%d\n", aw_pin, aw_mode, af_firmataInstance->pins[aw_pin].mode);
        return;
    }


    double * U = block->u[0];
    printf("af - analogWrite - inout - pin[%d] = %f; mode=%d\n",aw_pin, U[0], af_firmataInstance->pins[aw_pin].mode);

    int pinVal = (int) U[0];
    write(pinVal);

}

static void end(python_block * block) {
    write(0);
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
