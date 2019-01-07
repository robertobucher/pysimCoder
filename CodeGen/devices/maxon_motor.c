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
#include <string.h>

#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int

#include <canopen.h>
#include <unistd.h>

#define DATA_SIZE 5

static BYTE data[DATA_SIZE][8]= {
  {0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x15, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x15, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x15, 0x88, 0x00, 0x01, 0x00, 0x00, 0x00},
  {0x00, 0x05, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00}
};

static BYTE write_req[8]={0x00,0x22,0x00,0x00,0x00,0x00,0x00,0x00};

static void init(python_block *block)
{
  int i;

  if(canOpen()) exit(1);

  data[1][6]=(BYTE) (0x000000FF & (int) block->realPar[0]);
  data[1][7]=(BYTE) ((0x0000FF00 & (int) block->realPar[0]) >> 8);
  data[2][6]=(BYTE) (0x000000FF & (int) block->realPar[1]);
  data[2][7]=(BYTE) ((0x0000FF00 & (int) block->realPar[1]) >> 8);

  for(i=0;i<DATA_SIZE;i++){
    sendMsg(0x600+block->intPar[0],data[i],8);   /* SPDO */
    usleep(100000);
  }
}

static void inout(python_block *block)
{
  BYTE DATA[8];
  int U_can;
  double *u = block->u[0];

  U_can = (int) u[0];

  write_req[2]=(BYTE)U_can;
  write_req[3]=(BYTE)(U_can>>8);

  sendMsg(0x600+block->intPar[0],write_req,8);
}

static void end(python_block *block)
{
  BYTE DATA[8];
  int ret;

  write_req[2]=0x00;
  write_req[3]=0x00;
  sendMsg(0x600+block->intPar[0],write_req,8);

  canClose();
}

void maxon_mot(int Flag, python_block *block)
{
  if (Flag==OUT){          /* get the input */
    inout(block);
  }
  else if (Flag==END){     /* termination */ 
    end(block);
  }
  else if (Flag ==INIT){    /* initialisation */
    init(block);
  }
}


