/*
  COPYRIGHT (C) 2009  Roberto Bucher (roberto.bucher@supsi.ch)

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
*/

#include <pyblock.h>
#include <wiringPi.h>
#include <stdlib.h>
#include <commonFun.h>

#define RANGE  1024
#define ZERO      512 

static void init(python_block *block)
{
  int * intPar    = block->intPar;
  if (wiringPiSetup () == -1)
    exit (1) ;

  pinMode (intPar[0], PWM_OUTPUT) ;    /* set PWM port as output */
  pwmSetMode(PWM_MODE_MS); 
  pwmSetClock(2);
  pwmSetRange (RANGE) ;

  pwmWrite (intPar[0], ZERO) ;
}

static void inout(python_block *block)
{
  int * intPar    = block->intPar;
  double * realPar = block->realPar;
  
  double *u = block->u[0];
  double val = u[0];
  if (val>realPar[1]) val = realPar[1];
  if (val<realPar[0]) val = realPar[0];
  
  double value = (int) mapD2wD(val, realPar[0], realPar[1]);
  
  pwmWrite (intPar[0], (int) (value*RANGE)) ;
}

static void end(python_block *block)
{
  int * intPar    = block->intPar;
  pwmWrite (intPar[0], 0) ;
}

void pwm(int flag, python_block *block)
{
  if (flag==CG_OUT){          /* get input */
    inout(block);
  }
  else if (flag==CG_END){     /* termination */ 
    end(block);
  }
  else if (flag ==CG_INIT){    /* initialisation */
    init(block);
  }
}


