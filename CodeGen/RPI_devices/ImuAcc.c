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

#include <stdint.h>
#include <stdlib.h>
#include <pyblock.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <spiconfIMU.h>

#define REG_OUT_X_L_XL 		0x28
#define CTRL_REG6_XL		        0x20
#define FS_2G_ODR_952Hz		0xC0

#define G 					9.81

static void init(python_block *block)
{
  int * intPar    = block->intPar;
  intPar[0] = spiOpen_IMU();
  if(intPar[0] < 0) exit(1);

  conf_reg_IMU(CTRL_REG6_XL, FS_2G_ODR_952Hz);
}

static void inout(python_block *block)
{
  double * realPar = block->realPar;
  int * intPar    = block->intPar;
  double *yX = block->y[0];
  double *yY = block->y[1];
  double *yZ = block->y[2];

  IMU_data data;

  data = imu_reg_IMU(REG_OUT_X_L_XL);
  yX[0] = (double) data.X*G/1000*realPar[0];
  yY[0] = (double) data.Y*G/1000*realPar[1];
  yZ[0] = (double) data.Z*G/1000*realPar[2];
}

static void end(python_block *block)
{
  spiClose_IMU();
}

void ImuAcc(int flag, python_block *block)
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


