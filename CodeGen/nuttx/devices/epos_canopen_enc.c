/*
COPYRIGHT (C) 2016  Roberto Bucher (roberto.bucher@supsi.ch)

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
#include <stdio.h>
#include <stdlib.h>

#include <canopen.h>

static uint8_t read_req[8] = {0x40, 0x64, 0x60, 0x00, 0x01, 0x00, 0x00, 0x00};

static void init(python_block *block)
{
  if(canOpenTH(block->str)) exit(1);  
  registerMsg(0x580+block->intPar[0], 0x6064, 0x00);
}

static void inout(python_block *block)
{
  uint8_t DATA[8];
  double pi2 = 6.28318530718;
  int *counter, len;
  double *y = block->y[0];
  unsigned short *index;

  sendMsg(0x600+block->intPar[0],read_req,8);
  
  y[0] = pi2/block->realPar[0]*getValue(0x580+block->intPar[0], 0x6064, 0x00);
}

static void end(python_block *block)
{
  canClose();
}

void epos_canopen_enc(int Flag, python_block *block)
{
  if (Flag==CG_OUT){          /* set output */
    inout(block);
  }
  else if (Flag==CG_END){     /* termination */ 
    end(block);
  }
  else if (Flag==CG_INIT){    /* initialisation */
    init(block);
  }
}


