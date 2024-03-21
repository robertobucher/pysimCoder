#ifndef _ZYNQ_3PMDRV1_MC_H
#define _ZYNQ_3PMDRV1_MC_H

#include <stdint.h>

#define Z3PMDRV1_CHAN_COUNT    3

#define Z3PMDRV1_PWM_VALUE_m   0x0ffff
#define Z3PMDRV1_PWM_ENABLE    0x40000000
#define Z3PMDRV1_PWM_SHUTDOWN  0x80000000

typedef struct z3pmdrv1_state_t {
  uintptr_t regs_base_phys;
  void     *regs_base_virt;
  uint32_t pwm[Z3PMDRV1_CHAN_COUNT];
  uint32_t act_pos;
  uint32_t index_pos;
  uint32_t index_occur;
  uint32_t pos_offset;
  int32_t  curadc_val[Z3PMDRV1_CHAN_COUNT];
  int32_t  curadc_offs[Z3PMDRV1_CHAN_COUNT];
  uint8_t  hal_sensors;
  uint16_t curadc_sqn;
  uint16_t curadc_sqn_last;
  uint32_t curadc_cumsum[Z3PMDRV1_CHAN_COUNT];
  uint32_t curadc_cumsum_last[Z3PMDRV1_CHAN_COUNT];
  int      curadc_use_diff_to_last_fl;
} z3pmdrv1_state_t;

int z3pmdrv1_init(z3pmdrv1_state_t *z3pmcst);

int z3pmdrv1_transfer(z3pmdrv1_state_t *z3pmcst, int flags);

#endif /*_ZYNQ_3PMDRV1_MC_H*/
