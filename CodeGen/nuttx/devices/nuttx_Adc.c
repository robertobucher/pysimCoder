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

#include <nuttx/analog/adc.h>
#include <nuttx/analog/ioctl.h>

#define ADC_RES 4095

static void init(python_block *block)
{
  int * intPar    = block->intPar;
  int fd;

  fd = open(block->str, O_RDONLY);
  if(fd<0) exit(1);
  intPar[0] = fd;
}

static void inout(python_block *block)
{
  double * realPar = block->realPar;
  int * intPar = block->intPar;
  double *y;
  int ret;
  int i;
  int nbytes;
  int nout = block->nout;
  int fd = intPar[0];
  int readsize = nout*sizeof(struct adc_msg_s);
  
  struct adc_msg_s sample[nout];

  ret = ioctl(fd, ANIOC_TRIGGER, 0);
  nbytes = read(fd, sample, readsize);
  for(i=0;i< nout;i++){
    y = block->y[i];
    y[0] = maprD2D((double) sample[i].am_data/ADC_RES, realPar[0], realPar[1]);
  }
}

static void end(python_block *block)
{
  int * intPar    = block->intPar;

  close(intPar[0]);
}

void nuttx_Adc(int flag, python_block *block)
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


