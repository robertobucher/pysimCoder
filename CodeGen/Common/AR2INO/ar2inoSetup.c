/*
COPYRIGHT (C) 2022  Roberto Bucher (roberto.bucher@supsi.ch)

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
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h> 
#include <termios.h>
#include <unistd.h> 
#include <pthread.h>

#define MAXLEN 6

unsigned char BuffOut[4];
unsigned char BuffIn[6];

int32_t pins[127] = {0 };

int fdSerial;

static void * getData(void * p)
{
  int recv_len;
  uint8_t ch;
  int32_t *val;
  unsigned char *buffPt;

  while(1){
    recv_len = read(fdSerial, BuffIn, MAXLEN);
    ch = BuffIn[1];
    val = (int32_t *) &BuffIn[2];
    switch(ch){
    case 0: 
     pins[2] = *val;
     break;
    case 1:
      pins[4] = *val;
      break;
    default:
      pins[ch] = *val;
      break;
    }
  }
}

static void init(python_block *block)
{
  int * intPar    = block->intPar;
  struct termios ts;
  pthread_t thrd;
  speed_t spd;

  fdSerial =  open(block->str, O_RDWR);
  if(fdSerial == -1){
    fprintf(stderr," Error opening USB\n");
    exit(1);
  }

  tcgetattr(fdSerial, &ts);

  switch(intPar[0]){
  case 230400:    spd = B230400;  break;
  case 115200:    spd = B115200;  break;
  case 57600:     spd = B57600;   break;
  case 38400:     spd = B38400;   break;
  case 19200:     spd = B19200;   break;
  case 9600:      spd = B9600;    break;
  case 4800:      spd = B4800;    break;
  case 2400:      spd = B2400;    break;
  case 1800:      spd = B1800;    break;
  case 1200:      spd = B1200;    break;
  case 600:       spd = B600;     break;
  case 300:       spd = B300;     break;
  case 200:       spd = B200;     break;
  case 150:       spd = B150;     break;
  case 134:       spd = B134;     break;
  case 110:       spd = B110;     break;
  case 75:        spd = B75;      break;
  case 50:        spd = B50;      break;
#ifdef B460800
  case 460800:    spd = B460800;  break;
#endif
#ifdef B500000
  case 500000:    spd = B500000;  break;
#endif
#ifdef B576000
  case 576000:    spd = B576000;  break;
#endif
#ifdef B921600
  case 921600:    spd = B921600;  break;
#endif
#ifdef B1000000
  case 1000000:   spd = B1000000; break;
#endif
#ifdef B1152000
  case 1152000:   spd = B1152000; break;
#endif
#ifdef B1500000
  case 1500000:   spd = B1500000; break;
#endif
#ifdef B2000000
  case 2000000:   spd = B2000000; break;
#endif
#ifdef B2500000
  case 2500000:   spd = B2500000; break;
#endif
#ifdef B3000000
  case 3000000:   spd = B3000000; break;
#endif
#ifdef B3500000
  case 3500000:   spd = B3500000; break;
#endif
#ifdef B4000000
  case 4000000:   spd = B4000000; break;
#endif
#ifdef B7200
  case 7200:      spd = B7200;    break;
#endif
#ifdef B14400
  case 14400:     spd = B14400;   break;
#endif
#ifdef B28800
  case 28800:     spd = B28800;   break;
#endif
#ifdef B76800
  case 76800:     spd = B76800;   break;
#endif
  default: {
    return;
  }
  }
 
  cfsetispeed(&ts, spd);
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
  tcsetattr(fdSerial, TCSANOW, &ts);
 
  pthread_create(&thrd, NULL, getData, NULL);  

}

static void inout(python_block *block)
{
}

static void end(python_block *block)
{
  int * intPar    = block->intPar;
  
  /* close(intPar[0]); */
}

void ar2inoSetup(int flag, python_block *block)
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


