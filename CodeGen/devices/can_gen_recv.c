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

#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int

#include <canopen.h>
#include <stdlib.h>
#include <unistd.h>

static void init(python_block *block)
{
  if(canOpenTH()) exit(1);  
  registerMsg(block->intPar[3], (WORD) block->intPar[1], (BYTE) block->intPar[2]);
}

static void inout(python_block *block)
{
  BYTE DATA[8];
  double *y;
  
  WORD *index;
  BYTE *subindex;
  DWORD *data_index;

  DATA[0] = 0x40;
  index = (WORD *) &DATA[1];
  subindex = (BYTE *) &DATA[3];
  *index = block->intPar[1];
  *subindex = block->intPar[2];

  sendMsg(block->intPar[0],DATA,8);

  y =  block->y[0];
  y[0] = block->realPar[0]*getValue(block->intPar[3], block->intPar[1], block->intPar[2]);
}

static void end(python_block *block)
{
}

void can_gen_recv(int flag, python_block *block)
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


