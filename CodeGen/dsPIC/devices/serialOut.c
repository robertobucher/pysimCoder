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
#include <uart1.h>

#define PINS_PPSLock()           (RPCONbits.IOLOCK = 1)
#define PINS_PPSUnlock()         (RPCONbits.IOLOCK = 0)

static void init(python_block *block)
{
  PINS_PPSUnlock(); // unlock PPS

  RPINR13bits.U1RXR = 0x0032UL; //RD1->UART1:U1RX;
  RPOR28bits.RP114R = 0x0013UL;  //RH1->UART1:U1TX;

  PINS_PPSLock(); // lock PPS
}

double get_run_time(void);

static void uart1_send_buffer(const uint8_t *buf, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        while (!UART1_IsTxReady());
        UART1_Write(buf[i]);
    }
}

static void inout(python_block *block)
{
  int i;
  double *u;
  int N = block->nin;
  /* float values[N]; */
  float values[N+1];

  values[0] = (float) get_run_time();
  for(i=0;i<N;i++){
    u = block->u[i];
    values[i+1] = (float) u[0];
  }
  int LEN = (N+1)*sizeof(float);
  uart1_send_buffer((const uint8_t *) values, LEN); 
}

static void end(python_block *block)
{
}

void serialOut(int flag, python_block *block)
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


