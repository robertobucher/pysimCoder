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

static BYTE read_req[8] = {0x40, 0x14, 0x23, 0x01, 0x00, 0x00, 0x00, 0x00};

static void init(python_block *block)
{
   if(canOpenTH()) exit(1);
  
  registerMsg(0x580+block->intPar[0], 0x2314, block->intPar[1]+7);
}

static void inout(python_block *block)
{
  double *y = block->y[0];

  read_req[3] = block->intPar[1]+7;
  sendMsg(0x600+block->intPar[0],read_req,8);
  y[0] = 0.001*get2ByteValue(0x580+block->intPar[0], 0x2314, block->intPar[1]+7);
}

static void end(python_block *block)
{
  canClose();
}

void FH_5XXX_AD(int flag, python_block *block)
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


