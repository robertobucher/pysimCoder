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

void Error_Handler(void);

void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim);

static void MX_TIM4_Init(void);

#define PWM_FULL 1000

TIM_HandleTypeDef htim4;

static void init(python_block *block)
{
  static int counter = 0;
  
  int * intPar    = block->intPar;

  if (counter == 0) {
    MX_TIM4_Init();
    counter++;
  }

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
}

static void inout(python_block *block)
{
  double * realPar = block->realPar;
  int * intPar    = block->intPar;
  double *u = block->u[0];
  
  TIM_OC_InitTypeDef sConfigOC;
  double value;
  int nVal;

  value  = u[0];
  if(value>realPar[1]) value = realPar[1];
  if(value<realPar[0]) value = realPar[0];
  value = mapD2wD(value, realPar[0], realPar[1]);
  nVal = value*PWM_FULL;

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = nVal;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, intPar[0]);
  HAL_TIM_PWM_Start(&htim4, intPar[0]);
}

static void end(python_block *block)
{
}

void stm32PWM(int flag, python_block *block)
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

static void MX_TIM4_Init(void)
{
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 6;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 999;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
    {
      Error_Handler();
    }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
    {
      Error_Handler();
    }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    {
      Error_Handler();
    }
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
    {
      Error_Handler();
    }
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
    {
      Error_Handler();
    }
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
    {
      Error_Handler();
    }
  HAL_TIM_MspPostInit(&htim4);
}


