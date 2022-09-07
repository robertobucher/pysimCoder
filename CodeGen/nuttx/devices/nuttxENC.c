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

#include <nuttx/sensors/qencoder.h>

static void init(python_block *block)
{
  int * intPar    = block->intPar;
  int fd;

  fd = open(block->str, O_RDONLY);
  if(fd < 0)
    {
      fprintf(stderr,"Error opening device: %s\n", block->str);
      exit(1);
    }

  intPar[1] = fd;

  if (intPar[0])
    {
      int ret = ioctl(fd, QEIOC_RESET, 0);
      if (ret < 0)
        {
          fprintf(stderr,"qe_main: ioctl(QEIOC_RESET) failed: %d\n", errno);
          close(fd);
          exit(1);
        }
    }
}

static void inout(python_block *block)
{
  int * intPar    = block->intPar;
  double * realPar = block->realPar;
  double *y = block->y[0];
  int ret;
  int fd;
  int32_t position;
  double scale = 1;

  /* Set scale only if realPar[0] (ENC resolution) is not zero */

  if (realPar[0] != 0)
    {
      scale = 1/realPar[0];
    }

  fd = intPar[1];

  /* Do we have more outputs */

  if (block->nout == 3)
    {
      /* Yes, we want to get position, index and index count */

      struct qe_index_s index;

      double *y_index = block->y[1];
      double *y_icnt = block->y[2];

      ret = ioctl(fd, QEIOC_GETINDEX, (unsigned long)((uintptr_t)&index));
      if (ret < 0)
        {
          fprintf(stderr, "qe_main: ioctl(QEIOC_GETINDEX) failed: %d\n", errno);
          close(fd);
          exit(1);
        }

      y_index[0] = index.indx_pos*scale;
      y_icnt[0] = index.indx_cnt;
      position = index.qenc_pos;
    }
  else
    {
      /* No, only position is required */

      ret = ioctl(fd, QEIOC_POSITION, (unsigned long)((uintptr_t) &position));
      if (ret < 0)
        {
          fprintf(stderr, "qe_main: ioctl(QEIOC_POSITION) failed");
          close(fd);
          exit(1);
        }
    }
  
  /* Scale resolution */

  y[0] = position*scale;
}

static void end(python_block *block)
{
  int * intPar    = block->intPar;
  close(intPar[1]);
}

void nuttxENC(int flag, python_block *block)
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


