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
#include <stm32SPI4.h>

extern SPI_HandleTypeDef hspi4;

static void init(python_block *block)
{
  int * intPar    = block->intPar;
  int port;
  unsigned short ch;
  if(intPar[0] != -1){
    ch = (0x01 << (intPar[1]));  
    port = set_GPIO_outPort(intPar[0], ch);
    intPar[0] = port;
    intPar[1] = ch;
    MX_SPI4_Init_NOSS();
    HAL_GPIO_WritePin((GPIO_TypeDef *) intPar[0], intPar[1], GPIO_PIN_SET);    
  }
  else{
    MX_SPI4_Init();
  }
}

static void inout(python_block *block)
{
  int * intPar    = block->intPar;
  double *y;
  double *u;
  int inputs = block->nin;
  int outputs = block->nout;
  int i;

  uint16_t TxBuf[5];
  uint16_t RxBuf[5];

  for(i=0;i<5;i++) TxBuf[i] = 0;

  for(i=0;i<inputs;i++) {
    u = block->u[i];
    if(u[0]<0.0) u[0] = 0.0;
    if(u[0]>1.0) u[0] = 1.0;
    TxBuf[i] = (uint16_t) (u[0]*999);
  }
  
  /* CS if not SS */
  if(intPar[0] != -1)
    HAL_GPIO_WritePin((GPIO_TypeDef *) intPar[0], intPar[1], GPIO_PIN_RESET);

  HAL_SPI_TransmitReceive(&hspi4, (uint8_t *) TxBuf, (uint8_t *) RxBuf , 20, 1000);
  
  if(intPar[0] != -1)
    HAL_GPIO_WritePin((GPIO_TypeDef *) intPar[0], intPar[1], GPIO_PIN_SET);
  
  for(i=0;i<outputs;i++){
    y = block->y[i];
    y[0] = 0.1*RxBuf[intPar[2+i]];
  }
}

static void end(python_block *block)
{
}

void stm32SPI(int flag, python_block *block)
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


