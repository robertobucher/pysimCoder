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
#include <dsPICfun.h>
#include <qei1.h>

#define PINS_PPSLock()           (RPCONbits.IOLOCK = 1)
#define PINS_PPSUnlock()        (RPCONbits.IOLOCK = 0)

static void init(python_block *block)
{
   gpio_config(0, 0, GPIO_INPUT, GPIO_DIGITAL);
  gpio_config(0, 1, GPIO_INPUT, GPIO_DIGITAL);

  PINS_PPSUnlock(); // unlock PPS
  RPINR9bits.QEIA1R = 0x0001UL; //RA0->QEI1:QEIA1;
  RPINR9bits.QEIB1R = 0x0002UL; //RA1->QEI1:QEIB1;  
  PINS_PPSLock(); // lock PPS

  QEI1_PositionCountWrite(0);  // Init value for tests
  QEI1_Enable();
}

static void inout(python_block *block)
{
  double * realPar = block->realPar;
  double *y = block->y[0];
  double scale = 1;

  if (realPar[0] != 0)    {
    scale = 1/realPar[0];
  }

  uint32_t cnt = QEI1_PositionCountRead();
  int32_t iVal = (int32_t) cnt;

  y[0] = scale *iVal;
}

static void end(python_block *block)
{
}

void dspicENC(int flag, python_block *block)
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


