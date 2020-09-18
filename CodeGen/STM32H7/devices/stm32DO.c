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
#include <commonFun.h>
#include "stm32h7xx_hal.h"

static void init(python_block *block)
{
  int * intPar    = block->intPar;
  int port;
  unsigned short ch;

  ch = (0x01 << (intPar[1]));

  port = set_GPIO_outPort(intPar[0], ch);
  
  intPar[0] = port;
  intPar[1] = ch;
}

static void inout(python_block *block)
{
  double * realPar = block->realPar;
  int * intPar    = block->intPar;
  double *u = block->u[0];
  
  if(u[0] > realPar[0])  HAL_GPIO_WritePin((GPIO_TypeDef *) intPar[0], intPar[1], GPIO_PIN_SET);
  else                           HAL_GPIO_WritePin((GPIO_TypeDef *) intPar[0], intPar[1], GPIO_PIN_RESET);
}

static void end(python_block *block)
{
  int * intPar    = block->intPar;
  HAL_GPIO_WritePin((GPIO_TypeDef *) intPar[0], intPar[1], GPIO_PIN_RESET);
}

void stm32DO(int flag, python_block *block)
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


