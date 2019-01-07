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

static BYTE NMT_pre[2] = {0x80, 0x00};
static BYTE NMT_act[2] = {0x01, 0x00};

#define DATA_SIZE 3

static BYTE data[DATA_SIZE][8]= {
  {0x2b, 0x40, 0x60, 0x00, 0x06, 0x00, 0x00, 0x00},
  {0x2b, 0x40, 0x60, 0x00, 0x07, 0x00, 0x00, 0x00},  //Switch on
  {0x2b, 0x40, 0x60, 0x00, 0x0F, 0x00, 0x00, 0x00},
  {0x2b, 0x40, 0x60, 0x00, 0x1f, 0x00, 0x00, 0x00},
};

static BYTE pos_mode[8] = {0x2f, 0x60, 0x60, 0x00, 0x01, 0x00, 0x00, 0x00};
static BYTE read_req[8] = {0x40, 0x64, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00};

static void init(python_block *block)
{
  int i;
  BYTE DATA[8];

  if(canOpen()) exit(1);

  NMT_pre[1] = (BYTE) block->intPar[0];
  NMT_act[1] = (BYTE) block->intPar[0];

  sendMsg(0x000,NMT_pre,2);
  sendMsg(0x000,NMT_act,2);   /* Operational status */

  for(i=0;i<DATA_SIZE;i++){
    sendMsg(0x600+block->intPar[0],data[i],8);  /* configure device */
    rcvMsg(DATA, TIMEOUT);
    usleep(50000);
  }
  
  sendMsg(0x600+block->intPar[0],pos_mode,8);
  do{
    rcvMsg(DATA, TIMEOUT);
    usleep(50000);
  }while(DATA[0]!=0x60);
}

static void inout(python_block *block)
{
  BYTE DATA[8];
  int *counter, len;
  double pi2 =  6.28318530718;
  double *y = block->y[0];
  int *index;

  sendMsg(0x600+block->intPar[0],read_req,8);
  do{
    len = rcvMsg(DATA, TIMEOUT);
  }while(DATA[1]!=0x64);
  
  if(len>0){
    counter = (int*) &DATA[4];
    y[0] = (double) (*counter)*pi2/(block->realPar[0]);
  }
}

static void end(python_block *block)
{
  canClose();
}

void FH_canopen_enc(int Flag, python_block *block)
{
  if (Flag==OUT){          /* set output */
    inout(block);
  }
  else if (Flag==END){     /* termination */ 
    end(block);
  }
  else if (Flag==INIT){    /* initialisation */
    init(block);
  }
}


