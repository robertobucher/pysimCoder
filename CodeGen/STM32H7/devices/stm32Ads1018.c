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
#include <stm32SPI4.h>

#define DELAY 1300

extern SPI_HandleTypeDef hspi4;

static void init(python_block *block)
{
  int * intPar = block->intPar;
  double * realPar = block->realPar;
  uint8_t channel = (uint8_t) intPar[2];
  uint8_t range = (uint8_t) intPar[3];
  uint8_t single = (uint8_t) intPar[4];
  
  if(intPar[0] != -1){
    uint8_t ch = (0x01 << (intPar[1]));
    int port = set_GPIO_outPort(intPar[0], ch);
    intPar[0] = port;
    intPar[1] = ch;
    HAL_GPIO_WritePin((GPIO_TypeDef *) port, ch, GPIO_PIN_SET);
  }
  
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

  if(single == 1) {
    /* intPar[3] = 0xEB; */
    /* intPar[2] = (0xC0 | (channel<<4) | range << 1) & 0x00FF; */
    intPar[3] = 0xCB;
    intPar[2] = (0xC0 | (channel<<4) | range << 1 | 0x01) & 0x00FF;
  }
  else{
    intPar[3] = 0xCB;
    intPar[2] = (0xC0 | (channel<<4) | range << 1 | 0x01) & 0x00FF;
  }  
  MX_SPI4_Init();
}

static void inout(python_block *block)
{
  int * intPar = block->intPar;
  double * realPar = block->realPar;
  double *y = block->y[0];
  uint8_t TxBuf[2];
  uint8_t RxBuf[2];
  uint16_t value;
  int16_t cnt;

  TxBuf[1] = (uint8_t) intPar[2];
  TxBuf[0] = (uint8_t) intPar[3];
  
  /* CS if not SS */
  if(intPar[0] != -1) HAL_GPIO_WritePin((GPIO_TypeDef *) intPar[0], intPar[1], GPIO_PIN_RESET);
  
  HAL_SPI_TransmitReceive(&hspi4, TxBuf, RxBuf, 1, 100);
  
  /* CS if not SS */
  if(intPar[0] != -1) HAL_GPIO_WritePin((GPIO_TypeDef *) intPar[0], intPar[1], GPIO_PIN_SET);

  if(intPar[4]==0)
    for(cnt=0; cnt<DELAY; cnt++);
  
  value = ((RxBuf[0] | RxBuf[1] << 8) >> 4) & 0xFFF;
  
  y[0] = (double) (value) * realPar[0]/1024.0;
}

static void end(python_block *block)
{
}

void stm32Ads1018(int flag, python_block *block)
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


