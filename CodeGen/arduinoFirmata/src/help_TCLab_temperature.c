#include <stdio.h>
#include <pyblock.h>
#include <math.h>
//#include "firmata.h"


double convertToTemp2(python_block *block) {

  double *u = block->u[0];
  double U = (double)u[0] / 1024; // ADCREF = 1024 for arduino uno

  return (U * 5000.0 - 500.0) / 10.0;
}

static void inout(python_block *block)
{
  double *y = block->y[0];
  y[0] = convertToTemp2(block);
}

void help_TCLab_temperature(int flag, python_block *block)
{
  if (flag==CG_OUT){
    inout(block);
  }
}


