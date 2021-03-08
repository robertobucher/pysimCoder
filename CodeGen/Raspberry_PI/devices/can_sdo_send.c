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

#include <canopen.h>
#include <stdlib.h>

static void init(python_block *block)
{
  if(canOpenTH(block->str)) exit(1);
}

static void inout(python_block *block)
{
  short int U_can;
  uint8_t *Uaddr;

  uint8_t DATA[8];
  double *u = block->u[0];
  double *y;
  
  uint16_t *index;
  uint8_t *subindex;
  uint32_t *data_index;

  DATA[0] = 0x22;
  index = (uint16_t *) &DATA[1];
  subindex = (uint8_t *) &DATA[3];
  data_index = (uint32_t *) &DATA[4];
  *index = block->intPar[1];
  *subindex = block->intPar[2];

  if(block->intPar[4]) *data_index = u[0];
  else                 *data_index = block->intPar[3];

  sendMsg(0x600+block->intPar[0],DATA,8);
  if(block->nout == 1){
    y = block->y[0];
    y[0] = u[0];
  }
}

static void end(python_block *block)
{
  uint8_t DATA[8];
  double *y;
  
  uint16_t *index;
  uint8_t *subindex;
  uint32_t *data_index;

  DATA[0] = 0x22;
  index = (uint16_t *) &DATA[1];
  subindex = (uint8_t *) &DATA[3];
  data_index = (uint32_t *) &DATA[4];
  *index = block->intPar[1];
  *subindex = block->intPar[2];

  *data_index = 0;
  sendMsg(0x600+block->intPar[0],DATA,8);
  if(block->nout == 1){
    y = block->y[0];
    y[0] = 0.0;
  }

  canClose();
}

void can_sdo_send(int flag, python_block *block)
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


