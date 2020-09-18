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

static void init(python_block *block)
{
  int * intPar    = block->intPar;
  int fd;
  struct termios ts;

  fd =  open(block->str, O_RDWR);
  if(fd == -1){
    printf("Error opening USB\n");
    exit(1);
  }

  tcgetattr(fd, &ts);
  cfsetospeed(&ts, B115200);
  ts.c_cflag = (ts.c_cflag & ~CSIZE) | CS8;    
  ts.c_oflag &= ~IGNBRK;        
  ts.c_oflag = 0;                
  
  ts.c_oflag = 0;                
  ts.c_cc[VMIN]  = 0;            
  ts.c_cc[VTIME] = 100;            
  
  ts.c_oflag &= ~(IXON | IXOFF | IXANY);
 
  ts.c_cflag |= (CLOCAL);
  
  ts.c_cflag &= ~(PARENB | PARODD);     
  ts.c_cflag |= 0;
  ts.c_cflag &= ~CSTOPB;
  ts.c_cflag &= ~CRTSCTS;
  tcsetattr(fd, TCSANOW, &ts);
  tcflush( fd, TCIFLUSH );  
  intPar[0] = fd;
}

static void inout(python_block *block)
{
  int i, send_len;
  int fd = block->intPar[0];
  double *u;
  double data[block->nin];
  
  for(i=0;i< block->nin;i++){
    u = block->u[i];
    data[i] = u[0];
  }
  send_len = write(fd, (char *) data, sizeof(data));
}

static void end(python_block *block)
{
  int * intPar    = block->intPar;
  close(intPar[0]);
}

void serialOut(int flag, python_block *block)
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


