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
#include <myFun.h>

#define INPUT 1
#define OUTPUT 2
#define LOW             (0x0)
#define HIGH            (0x1)

static void init(python_block *block)
{
  int * intPar    = block->intPar;
  int i;
  for(i=0;i<3;i++) setPinMode(intPar[i], OUTPUT);
  writeDO(intPar[1], LOW);
  writeDO(intPar[2], LOW);
}

static void inout(python_block *block)
{
  int * intPar    = block->intPar;
  double *u = block->u[0];
  double val = u[0];

  if (val>1.0) val = 1.0;
  if (val<-1.0) val = -1.0;
  if (val> 0.0){
    writeDO(intPar[1], HIGH);
    writeDO(intPar[2], LOW);
    writePWM(intPar[0], val);
  }
  else{
    writeDO(intPar[1], LOW);
    writeDO(intPar[2], HIGH);
    writePWM(intPar[0], -val);
  }
}
	       
static void end(python_block *block)
{
  int * intPar    = block->intPar;
  writeDO(intPar[1], LOW);
  writeDO(intPar[2], LOW);
}

void brikiDCMot(int flag, python_block *block)
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


