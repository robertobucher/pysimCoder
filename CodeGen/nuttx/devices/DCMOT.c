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
#include <nuttx/config.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <nuttx/spi/spi.h>
#include <nuttx/spi/spi_transfer.h>

#define MAXLEN  10
#define BUFLEN  5

uint16_t TxBuf[BUFLEN];
int16_t RxBuf[BUFLEN];

struct spi_trans_s trans;
struct spi_sequence_s seq;

static void init(python_block *block)
{
  int * intPar    = block->intPar;
  int fd;
  
  fd = open(block->str, O_RDONLY);
  if (fd < 0)
    {
      fprintf(stderr, "Error opening device: %s\n", block->str);
      exit(1);
    }
  seq.dev = 0x170000;
  seq.mode = 0;
  seq.nbits = 8;
  seq.frequency = 4000000;
  seq.ntrans = 1;
  seq.trans = &trans;
  trans.deselect = true;
  trans.delay = 0;
  trans.nwords = 10;
  trans.txbuffer = (uint8_t *) TxBuf;
  trans.rxbuffer = (uint8_t *) RxBuf;
  intPar[0] = fd;
}

static void inout(python_block *block)
{
  int * intPar    = block->intPar;
  int fd = intPar[0];
  int inputs = block->nin;
  int outputs = block->nout;
  int ret;
  int i, index;
  double *y;
  double *u = block->u[0];

  for(i=0;i<BUFLEN;i++) TxBuf[i] = 0;
  if(u[0] < -1.0) u[0] = -1.0;
  if(u[0] >  1.0) u[0] = 1.0;
  TxBuf[0] = (uint16_t) ((u[0]+1)*499.5);
  
  ret = ioctl(fd, SPIIOC_TRANSFER, (unsigned long)((uintptr_t) &seq));

  if (ret < 0){
    fprintf(stderr,"ERROR: Failed to write value to %s\n", block->str);
    close(fd);
    exit(1);
  }
  else {
    for( i=0;i<outputs;i++){
      index = intPar[i+1];
      y = block->y[i];
      y[0] = 1.0*RxBuf[index];
    }
  }
}

static void end(python_block *block)
{
  int * intPar    = block->intPar;
  
  close(intPar[0]);
}

void DCMOT(int flag, python_block *block)
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


