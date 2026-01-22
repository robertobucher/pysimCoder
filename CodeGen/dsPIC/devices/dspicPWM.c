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
#include <xc.h>
#include <sccp1.h>
#include <dsPICfun.h>
#include <commonFun.h>

#define PINS_PPSLock()           (RPCONbits.IOLOCK = 1)
#define PINS_PPSUnlock()         (RPCONbits.IOLOCK = 0)

static void init(python_block *block)
{
  int * intPar    = block->intPar;
  uint8_t port_id = intPar[1];
  uint8_t pin     = intPar[2]; 

  gpio_config((gpio_port_t)port_id,
	      pin,
	      GPIO_OUTPUT,
	      GPIO_DIGITAL);
  
  PINS_PPSUnlock(); // unlock PPS
  RPOR12bits.RP49R = 0x0027UL;  //RD0->SCCP1:OCM1;
  PINS_PPSLock(); // lock PPS
}

static void inout(python_block *block)
{
  double * realPar = block->realPar;
  double *u = block->u[0];
  uint16_t iVal;

  double value = mapD2wD(u[0], realPar[0], realPar[1]);
  iVal = (uint16_t) (value*5000);

  SCCP1_PWM_DutyCycleSet(iVal);
}

static void end(python_block *block)
{
}

void dspicPWM(int flag, python_block *block)
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


