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

#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int

#define TIMEOUT -1

#include <canopen.h>
#include <stdlib.h>
#include <unistd.h>

static BYTE read_req[8] = {0x40, 0x77, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00};

static void init(python_block *block)
{
  if(canOpenTH()) exit(1);
  
  registerMsg(0x580+block->intPar[0], 0x6077, 0x00);
}

static void inout(python_block *block)
{
  double *y = block->y[0];

  sendMsg(0x600+block->intPar[0],read_req,8);
  y[0] = 1.0*getValue(0x580+block->intPar[0], 0x6077, 0x00);
}

static void end(python_block *block)
{
  canClose();
}

void FH_5XXX_getTQ(int Flag, python_block *block)
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


