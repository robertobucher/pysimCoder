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
#include <pthread.h>

static void * getData(void * p)
{
  int i;
  python_block *block = (python_block *) p;
  int recv_len, maxlen;
  int fd = block->intPar[0];
  double *y;
  double data[block->nout];

  maxlen = block->nout*sizeof(double);

  while(1){
    recv_len = read(fd, (char *) data, maxlen);
    for( i=0;i<block->nout;i++){
      y = block->y[i];
      y[0] = data[i];
    }
  }
}

static void init(python_block *block)
{
  int * intPar    = block->intPar;
  int fd;
  struct termios ts;
  pthread_t thrd;

  fd =  open(block->str, O_RDWR);
  if(fd == -1){
    printf("Error opening USB\n");
    exit(1);
  }

  tcgetattr(fd, &ts);
  cfsetispeed(&ts, B115200);
 ts.c_cflag = (ts.c_cflag & ~CSIZE) | CS8;    
  ts.c_iflag &= ~IGNBRK;        
  ts.c_lflag = 0;                
  
  ts.c_oflag = 0;                
  ts.c_cc[VMIN]  = 1;            
  ts.c_cc[VTIME] = 100;            

  ts.c_iflag &= ~(IXON | IXOFF | IXANY); 

  ts.c_cflag |= (CLOCAL | CREAD);
  
  ts.c_cflag &= ~(PARENB | PARODD);     
  ts.c_cflag |= 0;
  ts.c_cflag &= ~CSTOPB;
  ts.c_cflag &= ~CRTSCTS;
  tcsetattr(fd, TCSANOW, &ts);
  
  intPar[0] = fd;
  pthread_create(&thrd, NULL, getData, (void *) block);  
}

static void inout(python_block *block)
{
}

static void end(python_block *block)
{
  int * intPar    = block->intPar;
  
  close(intPar[0]);
}

void serialIn(int flag, python_block *block)
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


