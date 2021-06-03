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
#include "usb_device.h"
#include "usbd_cdc_if.h"

extern USBD_HandleTypeDef hUsbDeviceFS;
extern uint8_t OTG_outFlag;
extern uint8_t OTG_outLen;
extern uint8_t * OTG_outMsg;

static void init(python_block *block)
{
  int * intPar    = block->intPar;
  int N = block->nin;
  double *dataBuffer;
  
  dataBuffer = (double *) calloc(N, sizeof(double));
  block->ptrPar = dataBuffer;
  OTG_outMsg = (uint8_t *) dataBuffer;
  OTG_outLen = N*sizeof(double);
  OTG_outFlag = 0;
  
  start_OTG();
  intPar[1] = 0;
}

static void inout(python_block *block)
{
  double *u;
  int i;
  int * intPar    = block->intPar;
  int N = block->nin;  
  double * dataBuffer = block->ptrPar;
  
  if((intPar[1] % intPar[0]) == 0){
    for(i=0;i<N;i++){
      u = block->u[i];
      dataBuffer[i] = u[0];
    }
    OTG_outFlag = 1;
  }
  intPar[1] = intPar[1]+1;
}

static void end(python_block *block)
{
}

void USB_OTG_Out(int flag, python_block *block)
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


