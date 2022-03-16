#include <stdio.h>
#include <pyblock.h>
#include "firmata.h"

extern t_firmata * af_firmataInstance;

static void inout(python_block * block) {
    if (af_firmataInstance == NULL) {
        printf("af - customRead - inout - firmataInstance is empty - waiting\n");
        return;
    }

    while (!af_firmataInstance->isReady) {
        //Wait until device is up
        firmata_pull(af_firmataInstance);
    }

    firmata_pull(af_firmataInstance);
    int value = block->intPar[0];


    double * y = block->y[0];
    y[0] = af_firmataInstance->custom_value[value];
//    printf("af - customRead - inout - value[%d] = %f\n", value, y[0]);
}

void customRead_af(int flag, python_block * block) {
    if (flag == CG_OUT) {
        /* get input */
        inout(block);
    } else if (flag == CG_STUPD) {
    } else if (flag == CG_END) {
    } else if (flag == CG_INIT) {
    }
}
