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

extern void Error_Handler(void);

static void MX_DAC1_Init(void);
static void MX_OPAMP2_Init(void);

#define DAC_RES 4095

DAC_HandleTypeDef hdac1;
OPAMP_HandleTypeDef hopamp2;

static void init(python_block *block)
{
  int * intPar    = block->intPar;
  
  MX_DAC1_Init();
  MX_OPAMP2_Init();

  switch(intPar[0]){
  case 0:
    __HAL_DAC_ENABLE(&hdac1, DAC_CHANNEL_1);
    HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);
    HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0);
    intPar[0] = DAC_CHANNEL_1;
    break;
  case 1:
    __HAL_DAC_ENABLE(&hdac1, DAC_CHANNEL_2);
    HAL_DAC_Start(&hdac1, DAC_CHANNEL_2);
    HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_2, DAC_ALIGN_12B_R, 0);
    intPar[0] = DAC_CHANNEL_2;
    MX_OPAMP2_Init();
    HAL_OPAMP_Start(&hopamp2);
    break;
  }
}

static void inout(python_block *block)
{
  double * realPar = block->realPar;
  int * intPar    = block->intPar;
  double *u = block->u[0];
  
  double value;
  int nVal;

  value  = u[0];
  if(value>realPar[1]) value = realPar[1];
  if(value<realPar[0]) value = realPar[0];
  value = mapD2wD(value, realPar[0], realPar[1]);
  nVal = value*DAC_RES;
  HAL_DAC_SetValue(&hdac1, intPar[0], DAC_ALIGN_12B_R, nVal);
}

static void end(python_block *block)
{
}

void stm32AO(int flag, python_block *block)
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

static void MX_DAC1_Init(void)
{
  DAC_ChannelConfTypeDef sConfig = {0};
  
  /* DAC Initialization  */
  hdac1.Instance = DAC1;
  if (HAL_DAC_Init(&hdac1) != HAL_OK)
    {
      Error_Handler();
    }
  sConfig.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_ENABLE;
  sConfig.DAC_UserTrimming = DAC_TRIMMING_FACTORY;
  if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_1) != HAL_OK)
    {
      Error_Handler();
    }
  /* DAC channel OUT2 config  */
  sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_ENABLE;
  if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_2) != HAL_OK)
    {
      Error_Handler();
    }
  while(HAL_DACEx_SelfCalibrate(&hdac1, &sConfig, DAC_CHANNEL_1) != HAL_OK);
  while(HAL_DACEx_SelfCalibrate(&hdac1, &sConfig, DAC_CHANNEL_2) != HAL_OK);
}

static void MX_OPAMP2_Init(void)
{
  hopamp2.Instance = OPAMP2;
  hopamp2.Init.Mode = OPAMP_FOLLOWER_MODE;
  hopamp2.Init.NonInvertingInput = OPAMP_NONINVERTINGINPUT_DAC_CH;
  hopamp2.Init.PowerMode = OPAMP_POWERMODE_HIGHSPEED;
  hopamp2.Init.UserTrimming = OPAMP_TRIMMING_FACTORY;
  
  if (HAL_OPAMP_Init(&hopamp2) != HAL_OK)
  {
    Error_Handler();
  }
}
