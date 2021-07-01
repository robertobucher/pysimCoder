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

#include <nuttx/timers/pwm.h>

#define RANGE   65536

/****************************************************************************
 * Name: init
 *
 * Description:
 *   Open the device and set characteristics of PWM.
 *
 ****************************************************************************/

static void init(python_block *block)
{
  double * realPar = block->realPar;
  struct pwm_info_s info;
  int ret;
#ifdef CONFIG_PWM_MULTICHAN
  int fd = block->intPar[CONFIG_PWM_NCHANNELS];
#else
  int fd = block->intPar[1];
#endif

  if (fd == 0)
    {
      fd = open(block->str, O_RDONLY);
      if( fd < 0)
        {
          fprintf(stderr, "Error opening device: %s\n", block->str);
          exit(1);
        }

#ifdef CONFIG_PWM_MULTICHAN
  for (int i = 0; i < CONFIG_PWM_NCHANNELS; i++)
    {
      info.channels[i].channel = i + 1;
      info.channels[i].duty = 0;
    }
  if (block->nin != CONFIG_PWM_NCHANNELS)
    {
      fprintf(stderr, "Number of inputs is not equal to number of channels\n");
      close(fd);
      exit(1);
    }
#else
    info.duty = 0;
#endif
  }

  info.frequency = realPar[2];

  ret = ioctl(fd, PWMIOC_SETCHARACTERISTICS,
              (unsigned long)((uintptr_t) &info));
  if (ret < 0)
    {
      fprintf(stderr, "pwm_main: ioctl(PWMIOC_SETCHARACTERISTICS) failed\n");
      close(fd);
      exit(1);
    }

  ret = ioctl(fd, PWMIOC_START, 0);
  if (ret < 0)
    {
      fprintf(stderr,"pwm_main: ioctl(PWMIOC_START) failed\n");
      close(fd);
      exit(1);
    }

#ifdef CONFIG_PWM_MULTICHAN
  block->intPar[CONFIG_PWM_NCHANNELS] = fd;
#else
  block->intPar[1] = fd;
#endif

  block->ptrPar = (void *) &info;
}

/****************************************************************************
 * Name: inout
 *
 * Description:
 *   Compute and set PWM duty cycle based on input value.
 *
 ****************************************************************************/

static void inout(python_block *block)
{
  struct pwm_info_s *info = (struct pwm_info_s *)block->ptrPar;
  double * realPar = block->realPar;
  double *val;
  int ret;

  info->frequency = realPar[2];
#ifdef CONFIG_PWM_MULTICHAN
  int fd = block->intPar[CONFIG_PWM_NCHANNELS];
  for (int i = 0; i < CONFIG_PWM_NCHANNELS; i++)
    {
      val = (double *) block->u[i];
      if (val[0]>realPar[1]) val[0] = realPar[1];
      if (val[0]<realPar[0]) val[0] = realPar[0];

      double value = mapD2wD(val[0], realPar[0], realPar[1]);
      info->channels[block->intPar[i] - 1].duty = (uint32_t) (value*RANGE);
    }
#else
  int fd = block->intPar[1];
  val = (double *) block->u[0];
  if (val[0]>realPar[1]) val[0] = realPar[1];
  if (val[0]<realPar[0]) val[0] = realPar[0];

  double value = mapD2wD(val[0], realPar[0], realPar[1]);
  info->duty = (uint32_t) (value*RANGE);
#endif

  ret = ioctl(fd, PWMIOC_SETCHARACTERISTICS, (unsigned long)((uintptr_t)info));
  if (ret < 0) {
    fprintf(stderr,"pwm_main: ioctl(PWMIOC_SETCHARACTERISTICS) failed: %d\n",
	   errno);
    close(fd);
    exit(1);
  }
}

/****************************************************************************
 * Name: end
 *
 * Description:
 *   Stop PWM pulse and close the device.
 *
 ****************************************************************************/

static void end(python_block *block)
{
#ifdef CONFIG_PWM_MULTICHAN
  int fd = block->intPar[CONFIG_PWM_NCHANNELS];
#else
  int fd = block->intPar[1];
#endif

  int ret = ioctl(fd, PWMIOC_STOP, 0);
  if (ret < 0)
    {
      fprintf(stderr, "pwm_main: ioctl(PWMIOC_STOP) failed: %d\n", errno);
      close(fd);
      exit(1);
    }

  close(fd);
}

/****************************************************************************
 * Name: nuttx_PWM
 *
 * Description:
 *   Call needed function based on input flag.
 *
 ****************************************************************************/

void nuttx_PWM(int flag, python_block *block)
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
