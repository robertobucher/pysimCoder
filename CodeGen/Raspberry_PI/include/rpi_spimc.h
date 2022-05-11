#ifndef _RPI_SPIMC_H
#define _RPI_SPIMC_H

#include <stdint.h>

#define SPIMC_TRANSFER_SIZE 16
#define SPIMC_CHAN_COUNT    3

#define SPIMC_PWM_VALUE_m   0x0ffff
#define SPIMC_PWM_ENABLE    0x10000
#define SPIMC_PWM_SHUTDOWN  0x20000

typedef struct spimc_state_t {
  char     *spi_dev;
  int      spi_fd;
  uint32_t pwm[SPIMC_CHAN_COUNT];
  uint32_t act_pos;
  uint32_t index_pos;
  uint32_t index_occur;
  uint32_t pos_offset;
  int32_t  curadc_val[SPIMC_CHAN_COUNT];
  int32_t  curadc_offs[SPIMC_CHAN_COUNT];
  uint8_t  hal_sensors;
  uint16_t curadc_sqn;
  uint16_t curadc_sqn_last;
  uint32_t curadc_cumsum[SPIMC_CHAN_COUNT];
  uint32_t curadc_cumsum_last[SPIMC_CHAN_COUNT];
  uint8_t tx_buf[SPIMC_TRANSFER_SIZE];
  uint8_t rx_buf[SPIMC_TRANSFER_SIZE];
} spimc_state_t;

int spimc_init(spimc_state_t *spimcst);

int spimc_transfer(spimc_state_t *spimcst);

#endif /*_RPI_SPIMC_H*/