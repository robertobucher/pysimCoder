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

#define RANGE  1024
#define ZERO      512 
#define VZERO    5.0

static void init(python_block *block)
{
  int * intPar    = block->intPar;
  if (wiringPiSetup () == -1)
    exit (1) ;

  pinMode (intPar[0], PWM_OUTPUT) ;    /* set PWM port as output */
  pwmSetMode(PWM_MODE_BAL); 
  pwmSetClock(250);
  pwmSetRange (RANGE) ;

  pwmWrite (intPar[0], 0) ;
}

static void inout(python_block *block)
{
  int * intPar    = block->intPar;
  double *u = block->u[0];
  
  int val = (int) ((u[0]+VZERO)/VZERO*ZERO);
  pwmWrite (intPar[0], val) ;
}

static void end(python_block *block)
{
  int * intPar    = block->intPar;
  pwmWrite (intPar[0], 0) ;
}

void pwm(int flag, python_block *block)
{
  if (flag==OUT){          /* get input */
    inout(block);
  }
  else if (flag==END){     /* termination */ 
    end(block);
  }
  else if (flag ==INIT){    /* initialisation */
    init(block);
  }
}


