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

#define OUTPUT 2

static void init(python_block *block)
{
  int * intPar    = block->intPar;
  double * realPar = block->realPar;

  int channel = intPar[1];
  int range =  intPar[2];
  int conf;

  setPinMode(intPar[0], OUTPUT);
  writeDO(intPar[0],1);
  
  switch(range){
  case 0:
    realPar[0] = 6.144;
    break;
  case 1:
    realPar[0] = 4.096;
    break;
  case 2:
    realPar[0] = 2.048;
    break;
  case 3:
    realPar[0] = 1.024;
    break;
  case 4:
    realPar[0] = 0.512;
    break;
  default:
    realPar[0] = 0.256;
    break;
  } 

  conf = ((0x40 | (channel<<4) | range << 1) << 8) | 0x8B;
  intPar[1] = conf & 0xFFFF;
  initSPI();
}

static void inout(python_block *block)
{
  double * realPar = block->realPar;
  int * intPar    = block->intPar;
  double *y = block->y[0];
  int i;
  int value;
  
  value = SPI_ADS1018(intPar[1], intPar[0]);
  y[0] = (double) (value) * realPar[0]/2048.0;
}

static void end(python_block *block)
{
}

void brikiAD1018(int flag, python_block *block)
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


