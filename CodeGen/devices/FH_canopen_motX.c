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
License along with this library; if not, write to the Free Software1
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
*/

#include <pyblock.h>

#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int

#define TIMEOUT 1000

#include <canopen.h>
#include <stdlib.h>
#include <unistd.h>

static BYTE NMT_pre[2]  = {0x80, 0x00}; // network managment pre operational
static BYTE NMT_act[2]  = {0x01, 0x00}; // activate

#define DATA_SIZE 4

static BYTE data[DATA_SIZE][8]= {
  {0x2b, 0x40, 0x60, 0x00, 0x06, 0x00, 0x00, 0x00}, //ready to switch on
  {0x2b, 0x40, 0x60, 0x00, 0x07, 0x00, 0x00, 0x00}, //Switch on
  {0x2b, 0x40, 0x60, 0x00, 0x0F, 0x00, 0x00, 0x00}, //operation enabled
  {0x2b, 0x40, 0x60, 0x00, 0x1f, 0x00, 0x00, 0x00},
};

static BYTE pos_mode[8] = {0x2f, 0x60, 0x60, 0x00, 0x01, 0x00, 0x00, 0x00}; //position mode
static BYTE setPos[6] = {0x1F,0x00}; //position

static BYTE set_point_ack[2]    = {0x0F, 0x00};//reset set point

static void init(python_block *block)
{
  int i;
  BYTE DATA[8];
  unsigned char cval;
  int *U_can;
  
  if(canOpen()) exit(1);

  NMT_act[1] = (BYTE) block->intPar[0];

  sendMsg(0x000,NMT_act,2);   // Operational status

   for(i=0;i<DATA_SIZE;i++){
    sendMsg(0x600+block->intPar[0],data[i],8);  /* configure device */
    rcvMsg(DATA, TIMEOUT);
    usleep(50000);
  }
   sendMsg(0x600+block->intPar[0], pos_mode,8); 
   usleep(50000);  
} 

static void inout(python_block *block)
{
  int *U_can;

  BYTE DATA[8];
  double pi2 =  6.28318530718;
  double *u = block->u[0];
  unsigned short *index;
  
  U_can = (int *) &setPos[2];
  *U_can = (int) (u[0]/pi2*block->realPar[0]);

  sendMsg(0x200+block->intPar[0],set_point_ack,2);
  sendMsg(0x300+block->intPar[0],setPos,6);
}

static void end(python_block *block)
{
  BYTE DATA[8];
  unsigned short *index;
  int *U_can;

  canClose();
}

void FH_canopen_motX(int Flag, python_block *block)
{
  if (Flag==OUT){          /* set output */
    inout(block);
  }
  else if (Flag==END){     /* termination */ 
    end(block);
  }
  else if (Flag ==INIT){    /* initialisation */
    init(block);
  }
}


