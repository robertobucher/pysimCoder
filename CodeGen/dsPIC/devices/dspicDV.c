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
#include <dspicFun.h>
#include <uart1.h>

#define FRAME_START_1 0x5A
#define FRAME_START_2 0xA5

void static DV_Stream_Float(int ch, float *value) {
  int i,j;
  union {
    double f;
    uint8_t bytes[4];
  } data;

  /* 2 headers bytes */
  while(U1STATbits.TXBF);
  U1TXB = FRAME_START_1;
    
  while(U1STATbits.TXBF);
  U1TXB = FRAME_START_2;

  for(i=0;i<ch;i++){
    data.f = value[i];

    /* send double */
    for(j = 0; j < 4; j++) {
      while(U1STATbits.TXBF);
      U1TXB = data.bytes[j];
    }
  }
}

static void init(python_block *block)
{
  /*
    115200 bauds : 0x1b2
    460800 bauds : 0x57
    921600 bauds : 0x36
  */
  
  U1BRG = 0x36;       /* 921600 bauds */
}

static void inout(python_block *block)
{
  int N = block->nin;
  double *u;
  int i;
  float values[N];

  for(i=0;i<N;i++){
    u = block->u[i];
    values[i] = (float) u[0];
  }
  DV_Stream_Float(N, values);
}

static void end(python_block *block)
{
}

void dspicDV(int flag, python_block *block)
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


