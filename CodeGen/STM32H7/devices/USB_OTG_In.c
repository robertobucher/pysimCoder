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
#include "usb_device.h"
#include "usbd_cdc_if.h"

extern USBD_HandleTypeDef hUsbDeviceFS;
extern uint8_t OTG_inFlag;
extern uint8_t  *OTG_inMsg;

static void init(python_block *block)
{
  double *y;
  int i;
  int N = block->nout;
  double *realPar = block->realPar;
  double *dataBuffer;

  dataBuffer = (double *) malloc(N*sizeof(double));
  block->ptrPar = dataBuffer;
  
  for(i=0; i<N;i++){
    y = block->y[i];
    dataBuffer[i] = realPar[i];
    y[0] = realPar[i];
  }
  start_OTG();
  
  OTG_inFlag = 0;
}

static void inout(python_block *block)
{
  int i;
  int N = block->nout;
  double *y;
  double * dataBuffer = (double *) block->ptrPar;

  if(OTG_inFlag){
    USBD_CDC_HandleTypeDef   *hcdc = (USBD_CDC_HandleTypeDef *) hUsbDeviceFS.pClassData;
    if(hcdc->RxLength==N*sizeof(double)) memcpy(dataBuffer, hcdc->RxBuffer, hcdc->RxLength);
    OTG_inFlag = 0;
  }
  for(i=0; i<N;i++){
    y = block->y[i];
    y[0] = dataBuffer[i];
  }
}

static void end(python_block *block)
{
}

void USB_OTG_In(int flag, python_block *block)
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


