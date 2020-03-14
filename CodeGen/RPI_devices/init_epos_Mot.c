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

#define TIMEOUT -1

#include <canopen.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>

static BYTE NMT_pre[2]  = {0x80, 0x00};
static BYTE NMT_act[2]  = {0x01, 0x00};

static BYTE unsetPDO[8][8] = {
  {0x22, 0x00, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x22, 0x01, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x22, 0x02, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x22, 0x03, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x22, 0x00, 0x1A, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x22, 0x01, 0x1A, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x22, 0x02, 0x1A, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x22, 0x03, 0x1A, 0x00, 0x00, 0x00, 0x00, 0x00},
};

#define DATA_SIZE 7

static BYTE data[DATA_SIZE][8]= {
  {0x22, 0x40, 0x60, 0x00, 0x80, 0x00, 0x00, 0x00},
  {0x22, 0x40, 0x60, 0x00, 0x06, 0x00, 0x00, 0x00},
  {0x22, 0x40, 0x60, 0x00, 0x07, 0x00, 0x00, 0x00},
  {0x22, 0x40, 0x60, 0x00, 0x0F, 0x00, 0x00, 0x00},
  {0x22, 0xF6, 0x60, 0x01, 0x00, 0x00, 0x00, 0x00},
  {0x22, 0xF6, 0x60, 0x02, 0x00, 0x00, 0x00, 0x00},
  {0x22, 0x60, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00},
};

static BYTE reset[8]={0x22, 0x03, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00};

static void init(python_block *block)
{
  int i;
  BYTE DATA[8];

  if(canOpenTH()) exit(1);

  NMT_pre[1] = (BYTE) block->intPar[0];
  NMT_act[1] = (BYTE) block->intPar[0];

  data[4][4] = (0x000000FF & (int) block->realPar[0]);
  data[4][5] = ((0x0000FF00 & (int) block->realPar[0]) >> 8);
  data[5][4] = (0x000000FF & (int) block->realPar[1]);
  data[5][5] = ((0x0000FF00 & (int) block->realPar[1]) >> 8);

  data[6][4] = block->intPar[1];

  sendMsg(0x000,NMT_pre,2);

  for(i=0;i<8;i++){
    sendMsg(0x600+block->intPar[0],unsetPDO[i],8);  /* configure device */
    /* rcvMsg(DATA, TIMEOUT); */
    usleep(50000);
  }
  sendMsg(0x000,NMT_act,2);   /* Operationa status */

  sendMsg(0x600+block->intPar[0],reset,8);
  /* do{ */
  /*   rcvMsg(DATA, TIMEOUT); */
  /*   usleep(50000); */
  /* }while(DATA[0]!=0x60); */

  for(i=0;i<DATA_SIZE;i++){
    sendMsg(0x600+block->intPar[0],data[i],8);  /* configure device */
    /* rcvMsg(DATA, TIMEOUT); */
    usleep(50000);
  }
}

static void end(python_block *block)
{
  canClose();
}

void init_epos_Mot(int flag, python_block *block)
{
  if (flag==CG_END){     /* termination */ 
    end(block);
  }
  else if (flag==CG_INIT){    /* initialisation */
    init(block);
  }
}


