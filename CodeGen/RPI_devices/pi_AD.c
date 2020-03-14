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
#include <linux/spi/spidev.h>
#include <stdlib.h>
#include <stdint.h>
#include <spiconfADC.h>
#include <commonFun.h>
#include <stdio.h>

static void init(python_block *block)
{
  int * intPar    = block->intPar;
  unsigned char ch = (unsigned char) intPar[1];
  
  intPar[2] = spiOpen_ADC(intPar[0]);
  if(intPar[2]<0) exit(1);
  intPar[1] = (0xC0 | (ch<<4) | 0x05) & 0x00FF;

}

static void inout(python_block *block)
{
  int * intPar = block->intPar;
  double *realPar = block->realPar;
  
  double *y = block->y[0];
  unsigned char reg1 = (unsigned char) intPar[1];

  y[0]= (double) adc_reg_ADC(reg1)/1000.2442 - realPar[0];  
}

static void end(python_block *block)
{
  spiClose_ADC();
}

void pi_AD(int flag, python_block *block)
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


