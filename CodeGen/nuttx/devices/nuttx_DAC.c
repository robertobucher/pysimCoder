/*
  COPYRIGHT (C) 2021  Roberto Bucher (roberto.bucher@supsi.ch)

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
#include <nuttx/arch.h>

#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <nuttx/analog/dac.h>

/****************************************************************************
 * Name: init
 *
 * Description:
 *   Open the device and start reading task.
 *
 ****************************************************************************/

static void init(python_block *block)
{
  int * intPar = block->intPar;
  int fd = intPar[1];

  if (fd == 0)
    {
      fd = open(block->str, O_WRONLY | O_NONBLOCK);
      if (fd < 0)
        {
          fprintf(stderr, "Error opening device: %s\n", block->str);
          exit(1);
        }
    }

  intPar[1] = fd;
}

/****************************************************************************
 * Name: inout
 *
 * Description:
 *   Copy the data into the output signals.
 *
 ****************************************************************************/

static void inout(python_block *block)
{
  struct dac_msg_s msgs[1];
  int ret;
  int * intPar = block->intPar;
  double *u = block->u[0];

  int data  = (int)u[0];
  int channel = intPar[0];
  int fd = intPar[1];

  msgs[0].am_channel = channel;
  msgs[0].am_data = data;

  ret = write(fd, &msgs[0], sizeof(msgs));
  if (ret != sizeof(*msgs))
    {
      fprintf(stderr, "write failed: ret=%d\n", ret);
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
  close(block->intPar[1]);
  block->intPar[1] = 0;     /* Needed for reinitialization */
}

void nuttx_DAC(int flag, python_block *block)
{
  if (flag==CG_OUT){          /* get input */
    inout(block);
  }
  else if (flag==CG_END){     /* termination */
    end(block);
  }
  else if (flag ==CG_INIT){   /* initialisation */
    init(block);
  }
}
