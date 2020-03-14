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

static BYTE NMT_act[2]  = {0x01, 0x00};

#define DATA_SIZE1 3
static BYTE data1[DATA_SIZE1][8]= {
  {0x2b, 0x40, 0x60, 0x00, 0x06, 0x00, 0x00, 0x00},  //ready to switch on
  {0x2b, 0x40, 0x60, 0x00, 0x07, 0x00, 0x00, 0x00},  //Switch on
  {0x2b, 0x40, 0x60, 0x00, 0x0F, 0x00, 0x00, 0x00},  //operation enabled
};

#define DATA_SIZE2 2
static BYTE data2[DATA_SIZE2][8]= {
  {0x2b, 0x40, 0x60, 0x00, 0x0f, 0x00, 0x00, 0x00},  
  {0x2b, 0x40, 0x60, 0x00, 0x1f, 0x00, 0x00, 0x00},
};

#define DATA_SIZE_HOMING 2
static BYTE data_H_3XXX[DATA_SIZE_HOMING][8]= {
  {0x2f, 0x60, 0x60, 0x00, 0x06, 0x00, 0x00, 0x00},
  {0x2f, 0x98, 0x60, 0x00, 0x23, 0x00, 0x00, 0x00},
};
static BYTE data_H_5XXX[DATA_SIZE_HOMING][8]= {
  {0x2f, 0x60, 0x60, 0x00, 0x06, 0x00, 0x00, 0x00},
  {0x2f, 0x98, 0x60, 0x00, 0x25, 0x00, 0x00, 0x00},
};

#define DATA_CTR_X 2
static BYTE data_ctr_X[DATA_CTR_X][8]= {
  {0x2b, 0x32, 0x23, 0x01, 0x00, 0x00, 0x00, 0x00},
  {0x2b, 0x32, 0x23, 0x02, 0x00, 0x00, 0x00, 0x00},
};

#define DATA_CTR_V 2
static BYTE data_ctr_V[DATA_CTR_X][8]= {
  {0x2b, 0x31, 0x23, 0x01, 0x00, 0x00, 0x00, 0x00},
  {0x2b, 0x31, 0x23, 0x02, 0x00, 0x00, 0x00, 0x00},
};

#define DATA_CTR_TQ 2
static BYTE data_ctr_TQ[DATA_CTR_X][8]= {
  {0x2b, 0x42, 0x23, 0x01, 0x00, 0x00, 0x00, 0x00},
  {0x2b, 0x42, 0x23, 0x02, 0x00, 0x00, 0x00, 0x00},
};

static void init5(python_block *block)
{
  double * realPar = block->realPar;
  int * intPar    = block->intPar;

  int i;
  unsigned char cval;
  
  if(canOpenTH()) exit(1);

  NMT_act[1] = (BYTE) block->intPar[0];

  sendMsg(0x000,NMT_act,2);   /* Operational status */
  usleep(50000);

  for(i=0;i<DATA_SIZE1;i++){
    sendMsg(0x600+block->intPar[0],data1[i],8);  /* Reset devices */
    usleep(50000);
  }
   
  if (block->realPar[0] > 0.0){        /* Position controller parameters  */
      cval = (unsigned char) ((int) block->realPar[0] & 0x0000FFFF);
      data_ctr_X[3][4] = cval;
      cval = (unsigned char) ((int) block->realPar[1] & 0x0000FFFF);
      data_ctr_X[4][4] = cval;
      for(i=0;i<DATA_CTR_X;i++){
	sendMsg(0x600+block->intPar[0],data_ctr_X[i],8); 
	usleep(50000);
      }
  }

  if (block->realPar[2] > 0.0){    /* Speed Controller parameters  */
      cval = (unsigned char) ((int) block->realPar[2] & 0x0000FFFF);
      data_ctr_V[3][4] = cval;
      cval = (unsigned char) ((int) block->realPar[3] & 0x0000FFFF);
      data_ctr_V[4][4] = cval;
      for(i=0;i<DATA_CTR_X;i++){
	sendMsg(0x600+block->intPar[0],data_ctr_V[i],8); 
	usleep(50000);
      }
  }

  if (block->realPar[4] > 0.0){   /* Torque controller parameters  */
      cval = (unsigned char) ((int) block->realPar[4] & 0x0000FFFF);
      data_ctr_TQ[3][4] = cval;
      cval = (unsigned char) ((int) block->realPar[5] & 0x0000FFFF);
      data_ctr_TQ[4][4] = cval;
      for(i=0;i<DATA_CTR_X;i++){
	sendMsg(0x600+block->intPar[0],data_ctr_TQ[i],8); 
	usleep(50000);
      }
  }
    
  if(block->intPar[1]){    /* Homing method: set position as 0  */
    for(i=0;i<DATA_SIZE_HOMING;i++){
      sendMsg(0x600+block->intPar[0],data_H_5XXX[i],8);  /* Set this position to 0 */
      usleep(50000);
    }
  }
  
  for(i=0;i<DATA_SIZE2;i++){
    sendMsg(0x600+block->intPar[0],data2[i],8);       /* Initialize device */
    usleep(50000);
  }
}

static void init3(python_block *block)
{
  double * realPar = block->realPar;
  int * intPar    = block->intPar;
  
  int i;
  unsigned char cval;
  
  if(canOpenTH()) exit(1);

  NMT_act[1] = (BYTE) block->intPar[0];

  sendMsg(0x000,NMT_act,2);   /* Operational status */
  usleep(50000);

  for(i=0;i<DATA_SIZE1;i++){
    sendMsg(0x600+block->intPar[0],data1[i],8);  /* Reset devices */
    usleep(50000);
  }

  if (block->realPar[0] > 0.0){        /* Position controller parameters  */
      cval = (unsigned char) ((int) block->realPar[0] & 0x0000FFFF);
      data_ctr_X[3][4] = cval;
      cval = (unsigned char) ((int) block->realPar[1] & 0x0000FFFF);
      data_ctr_X[4][4] = cval;
      for(i=0;i<DATA_CTR_X;i++){
	sendMsg(0x600+block->intPar[0],data_ctr_X[i],8); 
	usleep(50000);
      }
  }

  if (block->realPar[2] > 0.0){    /* Speed Controller parameters  */
      cval = (unsigned char) ((int) block->realPar[2] & 0x0000FFFF);
      data_ctr_V[3][4] = cval;
      cval = (unsigned char) ((int) block->realPar[3] & 0x0000FFFF);
      data_ctr_V[4][4] = cval;
      for(i=0;i<DATA_CTR_X;i++){
	sendMsg(0x600+block->intPar[0],data_ctr_V[i],8); 
	usleep(50000);
      }
  }

  if(block->intPar[1]){
    for(i=0;i<DATA_SIZE_HOMING;i++){
      sendMsg(0x600+block->intPar[0],data_H_3XXX[i],8);  /* Set this position to 0 */
      usleep(50000);
    }
  }
  
  for(i=0;i<DATA_SIZE2;i++){
    sendMsg(0x600+block->intPar[0],data2[i],8);       /* Initialize device */
    usleep(50000);
  }  
}

static void end(python_block *block)
{
  canClose();
}

void FH_5XXX_INIT(int flag, python_block *block)
{
  if (flag==CG_END){     /* termination */ 
    end(block);
  }
  else if (flag ==CG_INIT){    /* initialisation */
    init5(block);
  }
}

void FH_3XXX_INIT(int flag, python_block *block)
{
  if (flag==CG_END){     /* termination */ 
    end(block);
  }
  else if (flag ==CG_INIT){    /* initialisation */
    init3(block);
  }
}


