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
#include <string.h>
#include <spiconfADC.h>
#include <stdio.h>

static int dev_cnt = 0;

static uint8_t mode=1;
static uint8_t bits = 8;
static uint32_t speed = 100000;
static uint16_t delay;

static int fd;

uint16_t adc_reg_ADC(uint8_t reg1)
{
  int ret;
  uint16_t value;
  int i;
  
  uint8_t tx[4] = {0};
  uint8_t rx[4] = {0};

  
  tx[0] = reg1 & 0xFF;
  tx[1] = 0x8B;
  
  struct spi_ioc_transfer tr_data = {
				     .tx_buf = (unsigned long)tx,
				     .rx_buf = (unsigned long)rx,
				     .len = 4,
				     .delay_usecs = delay,
				     .speed_hz = speed,
				     .bits_per_word = bits,
  };
    
  ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr_data);

  value = (rx[0]*255+rx[1]) >> 4;
  
  return((value) & 0x0FFF);
}

int spiOpen_ADC(int dev)
{
  int ret;
  char device[] = "/dev/spidev0.0";

  if(dev!=0) device[13] = '1';

  if(!dev_cnt){  /* This task is performed only one time */
    fd = open(device, O_RDWR);
    if(fd < 0) return -1;

    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
  }
  dev_cnt++;
  return fd;
}

void spiClose_ADC()
{
  if(--dev_cnt == 0) close(fd);
}
