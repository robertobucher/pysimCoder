/*
COPYRIGHT (C) 2016  Roberto Bucher (roberto.bucher@supsi.ch)

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

#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <spiconfIMU.h>

#define CTRL_REG5_XL	        0x1F // enable accelerometer axis out
#define ALL_AXIS			0x38

#define READ 			0x80
#define WRITE			0x00

static int dev_cnt = 0;

static const char *device = "/dev/spidev0.1";
static uint8_t mode;
static uint8_t bits = 8;
static uint32_t speed = 500000;
static uint16_t delay;

static int fd;

int conf_reg_IMU(uint8_t reg, uint8_t val)
{
  int ret;
  uint8_t tx_conf[2] = {0};
  uint8_t rx_conf[2] = {0};
  
  struct spi_ioc_transfer tr_conf = {
				     .tx_buf = (unsigned long)tx_conf,
				     .rx_buf = (unsigned long)rx_conf,
				     .len = 2,
				     .delay_usecs = delay,
				     .speed_hz = speed,
				     .bits_per_word = bits,
  };
  
  tx_conf[0]  =	WRITE | reg;
  tx_conf[1] =	val;
  ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr_conf);
  return(ret);
}

IMU_data imu_reg_IMU(uint8_t reg)
{
  int ret;
  uint8_t tx_data[7] = {0};
  uint8_t rx_data[7] = {0};

  IMU_data data;

  short *x = (short *)&rx_data[1];
  short *y = (short *)&rx_data[3];
  short *z = (short *)&rx_data[5];
  
  struct spi_ioc_transfer tr_data = {
				     .tx_buf = (unsigned long)tx_data,
				     .rx_buf = (unsigned long)rx_data,
				     .len = 7,
				     .delay_usecs = delay,
				     .speed_hz = speed,
				     .bits_per_word = bits,
  };
  
  tx_data[0]  = READ | reg;
  ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr_data);
  data.X = *x;
  data.Y = *y;
  data.Z = *z;
  return(data);
}

int spiOpen_IMU()
{
  int ret;

  if(!dev_cnt){  /* This task is performed only one time */
    fd = open(device, O_RDWR);
    if(fd < 0) return -1;

    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);

    conf_reg_IMU(CTRL_REG5_XL, ALL_AXIS);
  }
  dev_cnt++;
  return fd;
}

void spiClose_IMU()
{
  if(--dev_cnt == 0) close(fd);
}
