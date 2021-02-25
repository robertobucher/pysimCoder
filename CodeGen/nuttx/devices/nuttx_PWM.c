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
#include <commonFun.h>
#include <nuttx/config.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include <nuttx/timers/pwm.h>

#define RANGE   65536

static int fd;
static struct pwm_info_s info;
static double freq;

static void init(python_block *block)
{
  double * realPar = block->realPar;
  int i;
  int ret;
  
  if(fd==0){
    fd = open(block->str, O_RDONLY);
    if(fd<0) exit(1);
    freq = realPar[2];
    
#ifdef CONFIG_PWM_MULTICHAN
    for(i=0; i<CONFIG_PWM_NCHANNELS;i++){
      info.channels[i].channel = i;
      info.channels[i].duty  = 0;
    }
#else
  info.duty = 0;
#endif
  }

  info.frequency = freq;
    
  ret = ioctl(fd, PWMIOC_SETCHARACTERISTICS,
              (unsigned long)((uintptr_t) &info));
    
  ret = ioctl(fd, PWMIOC_START, 0);  
}

static void inout(python_block *block)
{
  double * realPar = block->realPar;
  int * intPar    = block->intPar;
  int ch = intPar[0];
  double *u = block->u[0];
  int ret;
  
  double val = u[0];
  if (val>realPar[1]) val = realPar[1];
  if (val<realPar[0]) val = realPar[0];
   
  double value = mapD2wD(val, realPar[0], realPar[1]);
  info.frequency = freq;
#ifdef CONFIG_PWM_MULTICHAN
  info.channels[ch].duty = (uint32_t) (value*RANGE);
#else
  info.duty = (uint32_t) (value*RANGE);
#endif
  
  ret = ioctl(fd, PWMIOC_SETCHARACTERISTICS,
              (unsigned long)((uintptr_t) &info));
}

static void end(python_block *block)
{
  close(fd);
}

void nuttx_PWM(int flag, python_block *block)
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


