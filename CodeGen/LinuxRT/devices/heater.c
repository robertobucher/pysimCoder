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
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h> 
#include <termios.h>
#include <unistd.h> 
#include <stdint.h> 
#include <string.h>
#include <pthread.h>

#define _POSIX_SOURCE 1

 #define BAUDRATE B576000
#define MAXLEN  10
#define BUFLEN  5

static void * getData(void * p)
{
  python_block *block = (python_block *) p;
  int * intPar    = block->intPar;
  int i;
  int recv_len;
  int index;
  int fd = intPar[0];
  
  double *y;
  uint16_t data[BUFLEN];

  while(1){
    recv_len = read(fd, (uint8_t *) data, MAXLEN);
    for( i=0;i<block->nout;i++){
      index = intPar[i+1];
      y = block->y[i];
      y[0] = 0.1*data[index];
    }   
  }
}

static void init(python_block *block)
{
  int * intPar    = block->intPar;
  int fd;
  struct termios newtio;
  pthread_t thrd;

  fd =  open(block->str, O_RDWR);
  if(fd == -1){
    fprintf(stderr," Error opening port\n");
    exit(1);
  }

  bzero(&newtio, sizeof(newtio));

  newtio.c_cflag = BAUDRATE | CS8 |  CLOCAL | CREAD;
  newtio.c_cflag &=  ~(PARENB | PARODD);
  newtio.c_iflag = 0;
  newtio.c_oflag = 0;
        
  newtio.c_lflag = 0;
         
  newtio.c_cc[VTIME]    = 0; 
  newtio.c_cc[VMIN]     = 100; 
   
  tcflush( fd, TCIFLUSH );  
  tcsetattr(fd,TCSANOW,&newtio);
  
  intPar[0] = fd;
  pthread_create(&thrd, NULL, getData, (void *) block);
}

static void inout(python_block *block)
{
  int * intPar    = block->intPar;
  int fd = block->intPar[0];
  double *u;
  uint16_t TxBuf[5];
  int i;
  int inputs = block->nin;
  int send_len, recv_len;
  uint16_t data[BUFLEN];
  
  for(i=0;i<BUFLEN;i++) TxBuf[i] = 0;
  for(i=0;i<inputs;i++) {
    u = block->u[i];
    if(u[0]<0.0) u[0] = 0.0;
    if(u[0]>1.0) u[0] = 1.0;
    TxBuf[i] = (uint16_t) (u[0]*999);
  }
  send_len = write(fd, (uint8_t *) TxBuf, MAXLEN);
}

static void end(python_block *block)
{
  int * intPar    = block->intPar;
  
  close(intPar[0]);
}

void heater(int flag, python_block *block)
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


