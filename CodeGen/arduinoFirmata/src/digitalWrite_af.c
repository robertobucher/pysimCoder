#include <stdio.h>
#include <pyblock.h>
#include "firmata.h"


extern t_firmata * af_firmataInstance;

int firmataReady = 0;
int pin = -1;
int pinValue = 0;

static void write(int newValue) {
    if (pin == -1) {
        printf("af - digitalWrite - pin not set\n");
        return;
    }

    if (newValue == pinValue) {
        return;
    }

    pinValue = newValue;

    firmata_pull(af_firmataInstance);
    firmata_digitalWrite(af_firmataInstance, pin, newValue);
}

static void inout(python_block * block) {
    if (af_firmataInstance == NULL) {
        printf("af - digitalWrite - inout - firmataInstance is empty - waiting\n");
        return;
    }

    while (!af_firmataInstance -> isReady) {
        //Wait until device is up
        firmata_pull(af_firmataInstance);
    }

    if (firmataReady == 0 && af_firmataInstance -> isReady) {
        firmataReady = 1;

        // init digitalWrite
        pin = block -> intPar[0];
        firmata_pinMode(af_firmataInstance, pin, MODE_OUTPUT);

        firmata_digitalWrite(af_firmataInstance, pin, LOW);
    }

    if (firmataReady == 0) {
        return;
    }

    double * U = block -> u[0];
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
