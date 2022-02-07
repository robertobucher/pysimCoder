#include <stdio.h>
#include <pyblock.h>
#include "firmata.h"


extern t_firmata * af_firmataInstance;

int dw_firmataReady = 0;
int dw_pin = -1;
int dw_pinValue = 0;

static void write(int newValue) {
    if (dw_pin == -1) {
        printf("af - digitalWrite - pin not set\n");
        return;
    }

    if (newValue == dw_pinValue) {
        return;
    }

    dw_pinValue = newValue;

    firmata_pull(af_firmataInstance);
    firmata_digitalWrite(af_firmataInstance, dw_pin, newValue);
}

static void inout(python_block * block) {
    if (af_firmataInstance == NULL) {
        printf("af - digitalWrite - inout - firmataInstance is empty - waiting\n");
        return;
    }

    while (!af_firmataInstance->isReady) {
        //Wait until device is up
        firmata_pull(af_firmataInstance);
    }

    if (dw_firmataReady == 0 && af_firmataInstance->isReady) {
        dw_firmataReady = 1;

        // init digitalWrite
        dw_pin = block->intPar[0];
        firmata_pinMode(af_firmataInstance, dw_pin, MODE_OUTPUT);

        firmata_digitalWrite(af_firmataInstance, dw_pin, LOW);
    }

    if (dw_firmataReady == 0) {
        return;
    }

    double * U = block->u[0];
    printf("af - digitalWrite - inout - U[0] = %f\n", U[0]);

    int pinVal = (int) U[0];
    write(pinVal);

}

static void end(python_block * block) {
    write(0);
}

void digitalWrite_af(int flag, python_block * block) {
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
