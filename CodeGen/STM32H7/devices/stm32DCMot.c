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

#define PWM_FULL 1000

extern TIM_HandleTypeDef htim4;

static void init(python_block *block)
{
  int * intPar    = block->intPar;

  int port1, port2;
  unsigned short ch1, ch2;
  
  switch(intPar[0]){
  case 1:
    intPar[0] = TIM_CHANNEL_1;
    break;
  case 2:
    intPar[0] = TIM_CHANNEL_2;
    break;
  case 3:
    intPar[0] = TIM_CHANNEL_3;
    break;
  case 4:
    intPar[0] = TIM_CHANNEL_4;
    break;
  default:
    intPar[0] = TIM_CHANNEL_1;
    break;
  }

  ch1 = (0x01 << (intPar[2]));
  ch2 = (0x01 << (intPar[4]));
  port1 = set_GPIO_outPort(intPar[1], ch1);
  port2 = set_GPIO_outPort(intPar[3], ch2);

  intPar[1] = port1;
  intPar[2] = ch1;
  intPar[3] = port2;
  intPar[4] = ch2;  
}

static void inout(python_block *block)
{
  double * realPar = block->realPar;
  int * intPar    = block->intPar;
  double *u = block->u[0];
 TIM_OC_InitTypeDef sConfigOC;
   double value = u[0];
  int nVal;
  
  if (value>realPar[1]) value = realPar[1];
  if (value<realPar[0]) value = realPar[0];
  nVal = value*PWM_FULL;

  if (value>0.0){
    HAL_GPIO_WritePin((GPIO_TypeDef *) intPar[1], intPar[2], GPIO_PIN_SET);
    HAL_GPIO_WritePin((GPIO_TypeDef *) intPar[3], intPar[4], GPIO_PIN_RESET);    
  }
  else{
    HAL_GPIO_WritePin((GPIO_TypeDef *) intPar[1], intPar[2], GPIO_PIN_RESET);
    HAL_GPIO_WritePin((GPIO_TypeDef *) intPar[3], intPar[4], GPIO_PIN_SET);
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = nVal;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, intPar[0]);
  HAL_TIM_PWM_Start(&htim4, intPar[0]);
}

static void end(python_block *block)
{
  int * intPar    = block->intPar;
  
  HAL_GPIO_WritePin((GPIO_TypeDef *) intPar[1], intPar[2], GPIO_PIN_RESET);
  HAL_GPIO_WritePin((GPIO_TypeDef *) intPar[3], intPar[4], GPIO_PIN_RESET);
}

void stm32DCMot(int flag, python_block *block)
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


