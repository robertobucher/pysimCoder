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
#include <stm32Fun.h>
#include "stm32h7xx_hal.h"

/* static unsigned short PA_Pins = 0x0000; */
/* static unsigned short PB_Pins = 0x0000; */
/* static unsigned short PC_Pins = 0x0000; */
/* static unsigned short PD_Pins = 0x0000; */
/* static unsigned short PE_Pins = 0x0000; */
/* static unsigned short PF_Pins = 0x0000; */
/* static unsigned short PG_Pins = 0x0000; */

static void init(python_block *block)
{
  int * intPar    = block->intPar;
  int port;
  unsigned short ch;

  ch = (0x01 << (intPar[1]));
  port = set_GPIO_inPort(intPar[0], ch);
  
  /* GPIO_InitTypeDef GPIO_InitStruct = {0}; */
  
  /* switch(intPar[0]){ */
  /* case 0: */
  /*   __HAL_RCC_GPIOA_CLK_ENABLE(); */
  /*   port = (int) GPIOA; */
  /*   PA_Pins = PA_Pins | ch; */
  /*   GPIO_InitStruct.Pin = PA_Pins; */
  /*   GPIO_InitStruct.Mode = GPIO_MODE_INPUT; */
  /*   GPIO_InitStruct.Pull = GPIO_NOPULL; */
  /*   HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);    */
  /*   break; */
  /* case 1: */
  /*   __HAL_RCC_GPIOB_CLK_ENABLE(); */
  /*   port = (int) GPIOB; */
  /*   PB_Pins = PB_Pins | ch; */
  /*   GPIO_InitStruct.Pin = PB_Pins; */
  /*   GPIO_InitStruct.Mode = GPIO_MODE_INPUT; */
  /*   GPIO_InitStruct.Pull = GPIO_NOPULL; */
  /*   HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);    */
  /*   break; */
  /* case 2: */
  /*   __HAL_RCC_GPIOC_CLK_ENABLE(); */
  /*   port = (int) GPIOC; */
  /*   PC_Pins = PC_Pins | ch; */
  /*   GPIO_InitStruct.Pin = PC_Pins; */
  /*   GPIO_InitStruct.Mode = GPIO_MODE_INPUT; */
  /*   GPIO_InitStruct.Pull = GPIO_NOPULL; */
  /*   HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);    */
  /*   break; */
  /* case 3: */
  /*   __HAL_RCC_GPIOD_CLK_ENABLE(); */
  /*   port = (int) GPIOD; */
  /*   PD_Pins = PD_Pins | ch; */
  /*   GPIO_InitStruct.Pin = PD_Pins; */
  /*   GPIO_InitStruct.Mode = GPIO_MODE_INPUT; */
  /*   GPIO_InitStruct.Pull = GPIO_NOPULL; */
  /*   HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);    */
  /*   break; */
  /* case 4: */
  /*   __HAL_RCC_GPIOE_CLK_ENABLE(); */
  /*   port = (int) GPIOE; */
  /*   PE_Pins = PE_Pins | ch; */
  /*   GPIO_InitStruct.Pin = PE_Pins; */
  /*   GPIO_InitStruct.Mode = GPIO_MODE_INPUT; */
  /*   GPIO_InitStruct.Pull = GPIO_NOPULL; */
  /*   HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);    */
  /*   break; */
  /* case 5: */
  /*   __HAL_RCC_GPIOF_CLK_ENABLE(); */
  /*   port = (int) GPIOF; */
  /*   PF_Pins = PF_Pins | ch; */
  /*   GPIO_InitStruct.Pin = PF_Pins; */
  /*   GPIO_InitStruct.Mode = GPIO_MODE_INPUT; */
  /*   GPIO_InitStruct.Pull = GPIO_NOPULL; */
  /*   HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);    */
  /*   break; */
  /* case 6: */
  /*   __HAL_RCC_GPIOG_CLK_ENABLE(); */
  /*   port = (int) GPIOG; */
  /*   PG_Pins = PG_Pins | ch; */
  /*   GPIO_InitStruct.Pin = PG_Pins; */
  /*   GPIO_InitStruct.Mode = GPIO_MODE_INPUT; */
  /*   GPIO_InitStruct.Pull = GPIO_NOPULL; */
  /*   HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);    */
  /*   break; */
  /* default: */
  /*   break; */
  /* } */
  intPar[0] = port;
  intPar[1] = ch;
}

static void inout(python_block *block)
{
  int * intPar    = block->intPar;
  double *y = block->y[0];

  y[0] = 1.0*HAL_GPIO_ReadPin((GPIO_TypeDef *) intPar[0], intPar[1]);
}

static void end(python_block *block)
{
}

void stm32DI(int flag, python_block *block)
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


