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

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include <nuttx/sensors/dhtxx.h>

static int fd;
static volatile int endrcv = 0;
static pthread_t  rt_rcv;
struct dhtxx_sensor_data_s global_data;

/****************************************************************************
 * Name: save_data
 *
 * Description:
 *   Save humidity and tempeture to global variable.
 *
 ****************************************************************************/

void save_data(struct dhtxx_sensor_data_s data)
{
  global_data.hum = data.hum;
  global_data.temp = data.temp;
}

/****************************************************************************
 * Name: rcv
 *
 * Description:
 *   Schedule reading thread as real time task.
 *
 ****************************************************************************/

void *rcv(void *args)
{
  /* Receiving thread scheduled as RT task */

  struct sched_param param;

  param.sched_priority = (int) args;
  if (sched_setscheduler(0, SCHED_FIFO, &param) == -1)
    {
      perror("sched_setscheduler failed");
      exit(-1);
    }

  int ret;
  struct dhtxx_sensor_data_s data;

  while (!endrcv) /* receiving loop */
    {
      ret = read(fd, &data, sizeof(struct dhtxx_sensor_data_s));
      if (ret < 0)
        {
          fprintf(stderr, "Read error %d\n", data.status);
        }
      else
        {
          save_data(data);
        }
    }
  return 0;
}

/****************************************************************************
 * Name: init
 *
 * Description:
 *   Open the device and start reading task.
 *
 ****************************************************************************/

static void init(python_block *block)
{
  int priority = 90;

  fd = open(block->str, O_RDWR);    /* Open the device */
  if (fd < 0)
    {
      fprintf(stderr, "ERROR: Failed to open the device %s\n", block->str);
      close(fd);
      exit(1);
    }

  /* Start reading task */

  pthread_create(&rt_rcv, NULL, rcv, (void *) priority);
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
  double *y = block->y[0];

  y[0] = (double)global_data.hum;
  y[1] = (double)global_data.temp;

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
  endrcv = 1; /* End receiving thread */
  close(fd);
}

void nuttx_DHTXX(int flag, python_block *block)
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
