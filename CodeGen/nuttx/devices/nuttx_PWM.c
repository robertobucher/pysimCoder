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

#include <string.h>
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
  int * intPar =  block->intPar;
  struct pwm_info_s info;
  int ret;
  
#ifdef CONFIG_PWM_MULTICHAN
  int fd = intPar[block->nin];
#else
  int fd = intPar[1];
#endif

  if (fd == 0)
    {
      fd = open(block->str, O_RDONLY);
      if (fd < 0)
        {
          fprintf(stderr, "Error opening device: %s\n", block->str);
          exit(1);
        }

#ifdef CONFIG_PWM_MULTICHAN
  /* Set all channels initially to zero  */

  for (int i = 0; i < CONFIG_PWM_NCHANNELS; i++)
    {
      info.channels[i].channel = 0;
      info.channels[i].duty = 0;
    #if defined(PWM_CPOL_HIGH) && defined(PWM_DCPOL_LOW)
      info.channels[i].cpol = PWM_CPOL_HIGH;
      info.channels[i].dcpol = PWM_DCPOL_LOW;
    #endif
    }

  /* Add channel numbr to first block->nin used channels */

  for (int i = 0; i < block->nin; i++)
    {
      info.channels[i].channel = intPar[i];
    }

  /* End the channel list with -1 if there is a free channel */

  if (block->nin < CONFIG_PWM_NCHANNELS)
    {
      info.channels[block->nin].channel = -1;
    }
#else
    info.duty = 0;
#endif
  }

  info.frequency = realPar[0];

  /* Set PWM characteristic and start the output */

  ret = ioctl(fd, PWMIOC_SETCHARACTERISTICS,
              (unsigned long)((uintptr_t) &info));
  if (ret < 0)
    {
      fprintf(stderr, "ioctl(PWMIOC_SETCHARACTERISTICS) failed\n");
      close(fd);
      exit(1);
    }

  ret = ioctl(fd, PWMIOC_START, 0);
  if (ret < 0)
    {
      fprintf(stderr,"ioctl(PWMIOC_START) failed\n");
      close(fd);
      exit(1);
    }

#ifdef CONFIG_PWM_MULTICHAN
  intPar[block->nin] = fd;
#else
  intPar[1] = fd;
#endif

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
  double * realPar = block->realPar;
  int * intPar =  block->intPar;
  struct pwm_info_s info;
  double *val;
  int ret;

  memset(&info, 0, sizeof(info));
  info.frequency = realPar[0];

#ifdef CONFIG_PWM_MULTICHAN
  /* Set all channels initially to zero  */

  for (int i = 0; i < CONFIG_PWM_NCHANNELS; i++)
    {
      info.channels[i].channel = 0;
      info.channels[i].duty = 0;
    #if defined(PWM_CPOL_HIGH) && defined(PWM_DCPOL_LOW)
      info.channels[i].cpol = PWM_CPOL_HIGH;
      info.channels[i].dcpol = PWM_DCPOL_LOW;
    #endif
    }

  if (block->nin < CONFIG_PWM_NCHANNELS)
    {
      info.channels[block->nin].channel = -1;
    }

  int fd = intPar[block->nin];

  /* Set output for used channels */

  for (int i = 0; i < block->nin; i++)
    {
      info.channels[i].channel = intPar[i];

      /* Get value from the input and saturate it */

      val = (double *) block->u[i];
      if (val[0]>realPar[2]) val[0] = realPar[2];
      if (val[0]<realPar[1]) val[0] = realPar[1];

      double value = mapD2wD(val[0], realPar[1], realPar[2]);

      /* Save duty cycle value */

      info.channels[i].duty = (uint32_t) (value*RANGE);
    }
#else
  int fd = intPar[1];

  /* Get value from the input and saturate it */

  val = (double *) block->u[0];
  if (val[0]>realPar[2]) val[0] = realPar[2];
  if (val[0]<realPar[1]) val[0] = realPar[1];

  double value = mapD2wD(val[0], realPar[1], realPar[2]);

  /* Save duty cycle value */

  info.duty = (uint32_t) (value*RANGE);
#endif

  /* Update PWM characteristics */

  ret = ioctl(fd, PWMIOC_SETCHARACTERISTICS, (unsigned long)((uintptr_t)&info));
  if (ret < 0)
    {
      fprintf(stderr,"ioctl(PWMIOC_SETCHARACTERISTICS) failed: %d\n",
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
  int * intPar =  block->intPar;

#ifdef CONFIG_PWM_MULTICHAN
  int fd = intPar[block->nin];
  intPar[block->nin] = 0; /* Needed for reinitialization */
#else
  int fd = intPar[1];
  intPar[1] = 0; /* Needed for reinitialization */
#endif

  /* Stop PWM output */

  int ret = ioctl(fd, PWMIOC_STOP, 0);
  if (ret < 0)
    {
      fprintf(stderr, "ioctl(PWMIOC_STOP) failed: %d\n", errno);
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
