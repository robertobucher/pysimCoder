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

#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int

#include <canopen.h>

static BYTE read_req[8]={0x40,0x04,0x60,0x00,0x00,0x00,0x00,0x00};

static void init(python_block *block)
{
  if(canOpenTH()) exit(1);  
  registerMsg(0x400+block->intPar[0], 0x6004, 0x00);
}  

static void inout(python_block *block)
{
  BYTE DATA[8];
  double pi = 3.14159265359;

  short int counter;
  double *y = block->y[0];
  int len;

  sendMsg(block->intPar[0],read_req,8);
  counter = (short int) (getValue(0x400+block->intPar[0], 0x6004, 0x00) & 0xFFFF);
  y[0] = 2*pi*(counter-0xFFF)/block->realPar[0];
}

static void end(python_block *block)
{
  canClose();
}

void baumer_enc(int Flag, python_block *block)
{
  if (Flag==CG_OUT){          /* set output */
    inout(block);
  }
  else if (Flag==CG_END){     /* termination */ 
    end(block);
  }
  else if (Flag ==CG_INIT){    /* initialisation */
    init(block);
  }
}


