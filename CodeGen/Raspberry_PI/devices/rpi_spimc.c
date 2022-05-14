/*
  Communication with Raspberry Pi equipped by 3-phase
  motor driver and RPI-MI-1 FPGA board designed
  by Petr Porazil for PiKRON company.
  The VHDL design by Martin Prudek.

  (C) 2015 by Martin Prudek prudemar@fel.cvut.cz
  (C) 2015 by Pavel Pisa pisa@cmp.felk.cvut.cz
 */

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include "rpi_spimc.h"

#define SPIMC_INDEX_BITS 12
#define SPIMC_INDEX_MASK ((1 << SPIMC_INDEX_BITS) - 1)

static uint8_t spimc_mode = 0;
static uint8_t spimc_bits = 8;
static uint32_t spimc_speed = 500000;
static uint16_t spimc_delay = 0;

static void pabort(const char *s)
{
        perror(s);
        abort();
}

int spimc_transfer(spimc_state_t *spimcst)
{
	uint8_t *tx = spimcst->tx_buf;
	uint8_t *rx = spimcst->rx_buf;
	int ret;
	uint32_t pwm1, pwm2, pwm3;
	uint32_t idx;
	int32_t  idxdiff;

	memset(tx, 0, SPIMC_TRANSFER_SIZE);
	memset(rx, 0, SPIMC_TRANSFER_SIZE);

	/*Data format:
	 * rx[0] - bity 127 downto 120 the first income bit..127
	 * rx[1] - bity 119 downto 112
	 * rx[2] - bity 111 downto 104
	 * rx[3] - bity 103 downto 96
	 * tx[4] - bity 95 downto 88
	 * tx[5] - bity 87 downto 80
	 * tx[6] - bity 79 downto 72
	 * tx[7] - bity 71 downto 64
	 * tx[8] - bity 63 downto 56
	 * tx[9] - bity 55 downto 48
	 * tx[10] - bity 47 downto 40
	 * tx[11] - bity 39 downto 32
	 * tx[12] - bity 31 downto 24
	 * tx[13] - bity 23 downto 16
	 * tx[14] - bity 15 downto 8
	 * tx[15] - bity 7 downto 0
	 *
	 * bit 127 - ADC reset
	 * bit 126 - enable PWM1
	 * bit 125 - enable PWM2
	 * bit 124 - enable PWM3
	 * bit 123 - shutdown1
	 * bit 122 - shutdown2
	 * bit 121 - shutdown3
	 * 	.
	 * bits 47 .. 32 - match PWM1
	 * bits 31 .. 16 - match PWM2
	 * bits 15 .. 0  - match PWM3
	 */

	pwm1 = spimcst->pwm[0];
	pwm2 = spimcst->pwm[1];
	pwm3 = spimcst->pwm[2];

	tx[0] = 0;
	if (pwm1 & SPIMC_PWM_ENABLE)
	  tx[0] |= 1 << 6;
	if (pwm1 & SPIMC_PWM_SHUTDOWN)
	  tx[0] |= 1 << 3;
	if (pwm2 & SPIMC_PWM_ENABLE)
	  tx[0] |= 1 << 5;
	if (pwm2 & SPIMC_PWM_SHUTDOWN)
	  tx[0] |= 1 << 2;
	if (pwm3 & SPIMC_PWM_ENABLE)
	  tx[0] |= 1 << 4;
	if (pwm3 & SPIMC_PWM_SHUTDOWN)
	  tx[0] |= 1 << 1;

	pwm1 &= SPIMC_PWM_VALUE_m;
	pwm2 &= SPIMC_PWM_VALUE_m;
	pwm3 &= SPIMC_PWM_VALUE_m;

	/* keep the cap*/
	if (pwm1 > 2047) pwm1 = 2047;
	if (pwm2 > 2047) pwm2 = 2047;
	if (pwm3 > 2047) pwm3 = 2047;

	/*pwm1*/
	tx[10] = pwm1 >> 8;   /*MSB*/
	tx[11] = pwm1 & 0xff; /*LSB*/

	/*pwm2*/
	tx[12] = pwm2 >> 8;   /*MSB*/
	tx[13] = pwm2 & 0xff; /*LSB*/

	/*pwm3*/
	tx[14] = pwm3 >> 8;   /*MSB*/
	tx[15] = pwm3 & 0xff; /*LSB*/

	struct spi_ioc_transfer tr = {
		.tx_buf = (uintptr_t)tx,
		.rx_buf = (uintptr_t)rx,
		.len = SPIMC_TRANSFER_SIZE,
		.delay_usecs = spimc_delay,
		.speed_hz = spimc_speed,
		.bits_per_word = spimc_bits,
	};

	ret = ioctl(spimcst->spi_fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
		return -1;

	/*prichozi data:
	 * rx[0] - bity 127 downto 120 the first income bit..127
	 * rx[1] - bity 119 downto 112
	 * rx[2] - bity 111 downto 104
	 * rx[3] - bity 103 downto 96
	 * rx[4] - bity 95 downto 88
	 * rx[5] - bity 87 downto 80
	 * rx[6] - bity 79 downto 72
	 * rx[7] - bity 71 downto 64
	 * rx[8] - bity 63 downto 56
	 * rx[9] - bity 55 downto 48
	 * rx[10] - bity 47 downto 40
	 * rx[11] - bity 39 downto 32
	 * rx[12] - bity 31 downto 24
	 * rx[13] - bity 23 downto 16
	 * rx[14] - bity 15 downto 8
	 * rx[15] - bity 7 downto 0	the last income bit..0
	 */

	/* position from IRC counter */
	spimcst->act_pos = ((uint32_t)rx[0] << 24) |
			   ((uint32_t)rx[1] << 16) |
			   ((uint32_t)rx[2] << 8) |
			   ((uint32_t)rx[3] << 0);

	/*halove sondy
	 * hal1 - bit95
	 * hal2 - bit94
	 * hal3 - bit93
	 */
	spimcst->hal_sensors = ((0x80 & rx[4]) >> 7) |
                               ((0x40 & rx[4]) >> 5) |
                               ((0x20 & rx[4]) >> 3);

	/* index position
	 * bits 92 downto 81
	 * 	92..88 in rx[4] last 5 bits (from left)
	 * 	87..81 in rx[5] first 7 bits (from left)
	 */
	idx = 0x1F & rx[4];
	idx <<= 8;
	idx |= 0xFE & rx[5];
	idx >>= 1;

	if ((idx ^ spimcst->index_pos) & SPIMC_INDEX_MASK) {
		idxdiff = (idx - spimcst->act_pos +
		           (1 << (SPIMC_INDEX_BITS - 1))) & SPIMC_INDEX_MASK;
		idxdiff -= 1 << (SPIMC_INDEX_BITS - 1);
		idx = spimcst->act_pos + idxdiff;
		spimcst->index_pos = idx;
		spimcst->index_occur += 1;
	}

	/* current measurments count
	 * bits 80 downto 72
	 * bit 80 in rx[5]
	 * bits 79..72 in rx[6]
	 */

	spimcst->curadc_sqn = 0x01 & rx[5];
	spimcst->curadc_sqn <<= 8;
	spimcst->curadc_sqn |= rx[6];


	/** currents
	 * ch2 - bits 71 downto 48
	 * 	71..64 in rx[7] - all byte
	 * 	63..56 in rx[8] - all byte
	 * 	55..48 in rx[9] - all byte
	 * ch0 - bits 47 downto 24
	 * 	47..40 in rx[10] - all byte
	 * 	39..32 in rx[11] - all byte
	 * 	31..24 in rx[12] - all byte
	 * ch1 - bits 23 downto 0
	 * 	23..16 in rx[13] - all byte
	 * 	15..8 in rx[14] - all byte
	 * 	7..0 in rx[15] - all byte
	 */

	spimcst->curadc_cumsum[2] = rx[7];
	spimcst->curadc_cumsum[2] <<= 8;
	spimcst->curadc_cumsum[2] |= rx[8];
	spimcst->curadc_cumsum[2] <<= 8;
	spimcst->curadc_cumsum[2] |= rx[9];

	spimcst->curadc_cumsum[0] = rx[10];
	spimcst->curadc_cumsum[0] <<= 8;
	spimcst->curadc_cumsum[0] |= rx[11];
	spimcst->curadc_cumsum[0] <<= 8;
	spimcst->curadc_cumsum[0] |=rx [12];

	spimcst->curadc_cumsum[1] = rx[13];
	spimcst->curadc_cumsum[1] <<= 8;
	spimcst->curadc_cumsum[1] |= rx[14];
	spimcst->curadc_cumsum[1] <<= 8;
	spimcst->curadc_cumsum[1] |= rx[15];

	return 0;
}

int spimc_init(spimc_state_t *spimcst)
{
	int ret = 0;
	int fd;

	spimcst->spi_fd = -1;

	fd = open(spimcst->spi_dev, O_RDWR);
	if (fd < 0) {
		pabort("can't open device");
	}
	printf("device open\n");
	/*
	 * spi spimc_mode
	 */
	ret = ioctl(fd, SPI_IOC_WR_MODE, &spimc_mode);
	if (ret == -1)
		pabort("can't set spi mode");

	ret = ioctl(fd, SPI_IOC_RD_MODE, &spimc_mode);
	if (ret == -1)
		pabort("can't get spi mode");

	/*
	 * bits per word
	 */
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &spimc_bits);
	if (ret == -1)
		pabort("can't set bits per word");

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &spimc_bits);
	if (ret == -1)
		pabort("can't get bits per word");

	/*
	 * max spimc_speed hz
	 */
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &spimc_speed);
	if (ret == -1)
		pabort("can't set max speed hz");

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &spimc_speed);
	if (ret == -1)
		pabort("can't get max speed hz");

	printf("spi spimc_mode: %d\n", spimc_mode);
	printf("bits per word: %d\n", spimc_bits);
	printf("delay: %d\n", spimc_delay);
	printf("max spimc_speed: %d Hz (%d KHz)\n", spimc_speed, spimc_speed/1000);

	spimcst->spi_fd = fd;

	return ret;
}
