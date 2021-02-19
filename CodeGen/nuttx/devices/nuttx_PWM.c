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

#define FREQ      10000
#define RANGE   65536

static void init(python_block *block)
{
  int * intPar    = block->intPar;
  int fd;
  int ret;
  
  struct pwm_info_s info;

  fd = open(block->str, O_RDONLY);
  if(fd<0) exit(1);
  intPar[0] = fd;

  info.frequency = FREQ;
  info.duty = (uint32_t) RANGE/2;
  
  ret = ioctl(fd, PWMIOC_SETCHARACTERISTICS,
              (unsigned long)((uintptr_t) &info));
  ret = ioctl(fd, PWMIOC_START, 0);  

}

static void inout(python_block *block)
{
  double * realPar = block->realPar;
  int * intPar    = block->intPar;
  double *u = block->u[0];
  int ret;
  int fd = intPar[0];

  
  
  struct pwm_info_s info;

  double val = u[0];
  if (val>realPar[1]) val = realPar[1];
  if (val<realPar[0]) val = realPar[0];
   
  double value = mapD2wD(val, realPar[0], realPar[1]);
  info.frequency = FREQ;
  info.duty = (uint32_t) (value*RANGE);
  ret = ioctl(fd, PWMIOC_SETCHARACTERISTICS,
              (unsigned long)((uintptr_t) &info));
}

static void end(python_block *block)
{
  int * intPar    = block->intPar;
  close(intPar[0]);
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


