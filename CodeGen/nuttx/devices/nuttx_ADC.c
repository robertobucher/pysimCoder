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

/****************************************************************************
 * Name: init
 *
 * Description:
 *   Open the device and get the number of configured channels.
 *
 ****************************************************************************/

static void init(python_block *block)
{
  int *intPar = block->intPar;
  int sw_trig = intPar[block->nout + 1];
  int fd = intPar[block->nout + 2];

  /* Open the device */

  if (fd == 0)
    {
      int oflag = O_RDONLY;
      if (sw_trig == 0)
        {
          oflag |= O_NONBLOCK;
        }
      fd = open(block->str, oflag);
      if (fd < 0)
        {
          fprintf(stderr,"Error opening device: %s\n", block->str);
          exit(1);
        }
    }

  /* Get the number of configured ADC channels.
   * The number of configured channels is defined by a macro in
   * board section in NuttX so there is no other way then ioctl
   * command to get this number.
   */

  int ret = ioctl(fd, ANIOC_GET_NCHANNELS, 0);
  if (ret < 0)
    {
      int errcode = errno;
      printf("ANIOC_GET_NCHANNELS ioctl failed: %d\n", errcode);
      close(fd);
      exit(1);
    }

  /* Check if the number of configured channels is greater or equal to the
   * number of channels defined by the user
   */

  if (ret < block->nout)
    {
      printf("ERROR: Number of ADC block outputs can not be less than \
              the number of configured channels");
      close(fd);
      exit(1);
    }

  /* Reset the ADC's FIFO */

  ioctl(fd, ANIOC_RESET_FIFO, 0);

  /* Save fd and number of configured channels to block parameters */

  intPar[block->nout + 2] = fd;
  intPar[block->nout + 3] = ret;
}

/****************************************************************************
 * Name: inout
 *
 * Description:
 *   Read data from ADC.
 *
 ****************************************************************************/

static void inout(python_block *block)
{
  double * realPar = block->realPar;
  int * intPar = block->intPar;
  double *y = block->y[0];
  int i, j, ret;
  int res     = intPar[block->nout];
  int sw_trig = intPar[block->nout + 1];
  int fd      = intPar[block->nout + 2];
  int conf_ch = intPar[block->nout + 3];
  int readsize = conf_ch*sizeof(struct adc_msg_s);
  int nbytes;

  /* Buffers for reading */

  struct adc_msg_s sample[conf_ch];

  /* Check if we need to SW trigger the ADC */

  if (sw_trig != 0)
    {
      ret = ioctl(fd, ANIOC_TRIGGER, 0);
      if (ret < 0)
        {
          int errcode = errno;
          fprintf(stderr,"adc_main: ANIOC_TRIGGER ioctl failed: %d\n", errcode);
          close(fd);
          exit(1);
        }
    }

  /* Read the data from all configured channels. */

  nbytes = read(fd, sample, readsize);
  if (nbytes <= 0)
    {
      int errval = errno;
      if (errval == EAGAIN)
        {
          return;
        }
      if (errval != EINTR)
        {
          fprintf(stderr,"adc_main: read %s failed: %d\n",
                  block->str, errval);
          close(fd);
          exit(1);
        }
    }

  /* Flush all remaining data from the fifo. We need newer data afterwards. */

  ioctl(fd, ANIOC_RESET_FIFO, 0);

  /* Passing the outputs to only those channels set in block parameters by the user */

  for (i = 0; i < block->nout; i++)
    {
      y = block->y[i];
      for (j = 0; j < conf_ch; j++)
        {
          if (sample[j].am_channel == intPar[i])
            {
              y[0] = maprD2D((double) sample[j].am_data/res, realPar[0], realPar[1]);
              break;
            }
        }
    }
}

/****************************************************************************
 * Name: end
 *
 * Description:
 *   Close the device.
 *
 ****************************************************************************/

static void end(python_block *block)
{
  int *intPar    = block->intPar;

  close(intPar[block->nout + 2]);
  intPar[block->nout + 2] = 0; /* Needed for reinitialization */
}

/****************************************************************************
 * Name: nuttx_ADC
 *
 * Description:
 *   Call needed function based on input flag.
 *
 ****************************************************************************/

void nuttx_ADC(int flag, python_block *block)
{
  /* get input */
  /* termination */
  /* initialisation */

  if (flag == CG_OUT)
    {
      inout(block);
    }
  else if (flag == CG_END)
    {
      end(block);
    }
  else if (flag == CG_INIT)
  {
    init(block);
  }
}
