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

#define ENCODERIN 3
#define MAXLEN 4
#define ENCRESET 2

static void init(python_block *block)
{
  int * intPar    = block->intPar;

  if(intPar[2]){
    BuffOut[0] = 0;
    BuffOut[1] = intPar[0];
    int16_t *value = (int16_t *) &BuffOut[2];
    *value =  ENCRESET;
    write(fdSerial, BuffOut, MAXLEN);
    usleep(50000);
  }
}

static void inout(python_block *block)
{
  int * intPar    = block->intPar;
  int ch = 0;
  double *y = block->y[0];

  BuffOut[0] = ENCODERIN;
  BuffOut[1] = intPar[0];
  int16_t *value = (int16_t *) &BuffOut[2];
  *value =  0;
  
  if(intPar[0]==0) ch = 2;
  else                    ch = 4;
  
  write(fdSerial, BuffOut, MAXLEN);
  y[0] = 1.0*pins[ch]/intPar[1];
}

static void end(python_block *block)
{
}

void ar2inoENC(int flag, python_block *block)
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


