/*
COPYRIGHT (C) 2022  Roberto Bucher (roberto.bucher@supsi.ch)

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
#include <commonFun.h>
#include <stdint.h>
#include <unistd.h> 

extern int fdSerial;
extern int32_t pins[];
extern unsigned char BuffOut[4];

#define MAXLEN 4
#define DIGITALOUT 5
#define OUTPUT 1

static void init(python_block *block)
{
  int * intPar    = block->intPar;

  BuffOut[0] = 0;
  BuffOut[1] = intPar[0];
  int16_t *value = (int16_t *) &BuffOut[2];
  *value =  OUTPUT;
  write(fdSerial, BuffOut, MAXLEN);
  usleep(50000);
 }

static void inout(python_block *block)
{
  double * realPar = block->realPar;
  int * intPar    = block->intPar;
  double *u = block->u[0];

  BuffOut[0] = DIGITALOUT;
  BuffOut[1] = intPar[0];

  int16_t *value = (int16_t *) &BuffOut[2];

  if(u[0]>realPar[0]) *value = 1;
  else                        *value = 0;

  if (*value!=pins[intPar[0]]){
      pins[intPar[0]] = *value;
      write(fdSerial, BuffOut, MAXLEN);
    }
}

static void end(python_block *block)
{
  int * intPar    = block->intPar;
  BuffOut[0] = DIGITALOUT;
  BuffOut[1] = intPar[0];
  int16_t *value = (int16_t *) &BuffOut[2];
  
  * value = 0;
  write(fdSerial, BuffOut, MAXLEN);
}

void ar2inoDO(int flag, python_block *block)
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


