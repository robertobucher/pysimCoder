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
#include <stdio.h>

#include <nuttx/analog/adc.h>
#include <nuttx/analog/ioctl.h>

#define NCHANNELS 3          /* the same as in board/xxxx_adc.c  */
#define ADC_RES 4095

static int fd;

static void init(python_block *block)
{
  if(fd==0){
    fd = open(block->str, O_RDONLY);
    if(fd<0) {
      fprintf(stderr,"Error opening device: %s\n", block->str);
      exit(1);
    }
  }
}

static void inout(python_block *block)
{
  double * realPar = block->realPar;
  int * intPar = block->intPar;
  double *y = block->y[0];
  int ret;
  int ch = intPar[0];
  int readsize = NCHANNELS*sizeof(struct adc_msg_s);
  int nbytes;
  
  struct adc_msg_s sample[NCHANNELS];

#ifdef CONFIG_EXAMPLES_ADC_SWTRIG
  ret = ioctl(fd, ANIOC_TRIGGER, 0);
  if (ret < 0)
    {
      int errcode = errno;
      fprintf(stderr,"adc_main: ANIOC_TRIGGER ioctl failed: %d\n", errcode);
      close(fd);
      exit(1);
    }
#endif
      
  nbytes = read(fd, sample, readsize);

  if (nbytes <= 0){
    int errval = errno;
    if (errval != EINTR)
      {
	fprintf(stderr,"adc_main: read %s failed: %d\n",
	       block->str, errval);
	close(fd);
	exit(1);
      }
  }
  
  y[0] = maprD2D((double) sample[ch].am_data/ADC_RES, realPar[0], realPar[1]);
}

static void end(python_block *block)
{
  int * intPar    = block->intPar;

  close(intPar[1]);
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


