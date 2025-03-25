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
extern unsigned char BuffIn[6];

#define ANALOGIN 1
#define MAXLEN 4
#define MAXLENIN 6
#define INPUT 0

static void init(python_block *block)
{
  int * intPar    = block->intPar;

  BuffOut[0] = 0;
  BuffOut[1] = intPar[0];
  int16_t *value = (int16_t *) &BuffOut[2];
  *value =  INPUT;
  write(fdSerial, BuffOut, MAXLEN);
  usleep(50000);
}

static void inout(python_block *block)
{
  double * realPar = block->realPar;
  int * intPar    = block->intPar;
  double *y = block->y[0];
  /* int recv_len; */
  /* int32_t *val; */
  uint8_t ch;

  BuffOut[0] = ANALOGIN;
  ch = intPar[0];
  BuffOut[1] = ch;
  int16_t *value = (int16_t *) &BuffOut[2];  
  * value = 0;
  write(fdSerial, BuffOut, MAXLEN);
  
  y[0] = maprD2D(pins[ch]/1023.0, realPar[0], realPar[1]);
  
}

static void end(python_block *block)
{
}

void ar2inoAI(int flag, python_block *block)
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


