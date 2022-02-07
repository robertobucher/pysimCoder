#include <stdio.h>
#include <pyblock.h>
#include "firmata.h"

t_firmata * af_firmataInstance;

// TODO - this block should be initialized befdore any other arduinoFirmata blocks
//  currently it seems it is not possible to oreder it in python api
static void init(python_block *block)
{
    char * port = (char *) block->str;

    af_firmataInstance = firmata_new(port); //init Firmata
    while (!af_firmataInstance -> isReady) {
      //Wait until device is up
      firmata_pull(af_firmataInstance);
    }


//    firmata_pinMode(firmata, 8, MODE_OUTPUT); //set pin 13 (led on most arduino) to out
//
//    sleep(2);
//    firmata_pull(firmata);
//    firmata_digitalWrite(firmata, 8, LOW); //unlight led
//
//    sleep(2);
//    firmata_pull(firmata);
//    firmata_digitalWrite(firmata, 8, HIGH); //light led
//
//    sleep(2);
//    firmata_pull(firmata);
//    firmata_digitalWrite(firmata, 8, LOW); //unlight led
//
//    sleep(2);
//    firmata_pull(firmata);
//    firmata_digitalWrite(firmata, 8, HIGH); //light led
//
//    sleep(2);
//    firmata_pull(firmata);
}

static void inout(python_block *block)
{
//  printf("af - setup - inout\n");
  /* double * realPar = block->realPar; */
  /* int * intPar    = block->intPar; */
  /* double *y = block->y[0]; */
  /* double *u = block->u[0]; */

}

static void update(python_block *block)
{
  printf("af - setup - update\n");
  /* double * realPar = block->realPar; */
  /* int * intPar    = block->intPar; */
  /* double *y = block->y[0]; */
  /* double *u = block->u[0]; */

}

static void end(python_block *block)
{
  printf("af - setup - end\n");

  /* double * realPar = block->realPar; */
  /* int * intPar    = block->intPar; */
  /* double *y = block->y[0]; */
  /* double *u = block->u[0]; */

}

void setup_af(int flag, python_block *block)
{
  if (flag==CG_OUT){          /* get input */
    inout(block);
  }
  else if (flag == CG_STUPD){
    update(block);
  }
  else if (flag==CG_END){     /* termination */
    end(block);
  }
  else if (flag ==CG_INIT){    /* initialisation */
    init(block);
  }
}


