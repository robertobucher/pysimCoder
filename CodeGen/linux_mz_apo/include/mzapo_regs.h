/*******************************************************************
  Simple program to check LCD functionality on MicroZed
  based MZ_APO board designed by Petr Porazil at PiKRON

  mzapo_regs.h       - definition of the MZ_APO design registers

  (C) Copyright 2017 by Pavel Pisa
      e-mail:   pisa@cmp.felk.cvut.cz
      homepage: http://cmp.felk.cvut.cz/~pisa
      company:  http://www.pikron.com/
      license:  any combination of GPL, LGPL, MPL or BSD licenses

 *******************************************************************/

#ifndef MZAPO_REGS_H
#define MZAPO_REGS_H

/*
  Complete description of the educational MZ_APO design registers
  can be found at

    https://cw.fel.cvut.cz/wiki/courses/b35apo/documentation/mz_apo/start

  The peripherals VHDL sources can be found in the repository

    https://gitlab.fel.cvut.cz/canbus/zynq/zynq-can-sja1000-top/tree/master/system/ip

  MZ_APO hardware and mechanical design sources

    https://gitlab.com/pikron/projects/mz_apo/microzed_apo

*/

/* SPI connected knobs and LEDs registers and keyboard */

#define SPILED_REG_BASE_PHYS  0x43c40000
#define SPILED_REG_SIZE       0x00004000

#define SPILED_REG_LED_LINE_o           0x004
#define SPILED_REG_LED_RGB1_o           0x010
#define SPILED_REG_LED_RGB2_o           0x014
#define SPILED_REG_LED_KBDWR_DIRECT_o   0x018

#define SPILED_REG_KBDRD_KNOBS_DIRECT_o 0x020
#define SPILED_REG_KNOBS_8BIT_o         0x024

/* Parallel LCD registers */

#define PARLCD_REG_BASE_PHYS  0x43c00000
#define PARLCD_REG_SIZE       0x00004000

#define PARLCD_REG_CMD_o                0x0008
#define PARLCD_REG_DATA_o               0x000C

/* RC model servos and optional PS2 peripheral */

#define SERVOPS2_REG_BASE_PHYS 0x43c50000
#define SERVOPS2_REG_SIZE      0x4000

#define SERVOPS2_REG_CR_o               0x0000
#define SERVOPS2_REG_PWMPER_o           0x000C
#define SERVOPS2_REG_PWM1_o             0x0010
#define SERVOPS2_REG_PWM2_o             0x0014
#define SERVOPS2_REG_PWM3_o             0x0018
#define SERVOPS2_REG_PWM4_o             0x001C

/* Simple audio PWM output */

#define AUDIOPWM_REG_BASE_PHYS 0x43c60008
#define AUDIOPWM_REG_SIZE      0x4000

#define AUDIOPWM_REG_CR_o               0x0000
#define AUDIOPWM_REG_PWMPER_o           0x0008
#define AUDIOPWM_REG_PWM_o              0x000C

/* Optional DC Motor Simple Driver Peripherals for PSR Subject */

#define DCSPDRV_REG_BASE_PHYS_0 0x43c20000
#define DCSPDRV_REG_BASE_PHYS_1 0x43c30000
#define DCSPDRV_REG_SIZE      0x4000

#define DCSPDRV_REG_CR_o                0x0000
#define DCSPDRV_REG_CR_PWM_A_DIRECT_m          0x00000010
#define DCSPDRV_REG_CR_PWM_B_DIRECT_m          0x00000020
#define DCSPDRV_REG_CR_PWM_ENABLE_m            0x00000040
#define DCSPDRV_REG_CR_IRC_RESET_m             0x00000100

#define DCSPDRV_REG_SR_o                0x0004
#define DCSPDRV_REG_SR_IRC_A_MON_m             0x00000100
#define DCSPDRV_REG_SR_IRC_B_MON_m             0x00000200
#define DCSPDRV_REG_SR_IRC_IRQ_MON_m           0x00000400

#define DCSPDRV_REG_PERIOD_o            0x0008
#define DCSPDRV_REG_PERIOD_MASK_m              0x3fffffff

#define DCSPDRV_REG_DUTY_o              0x000C
#define DCSPDRV_REG_DUTY_MASK_m                0x3fffffff
#define DCSPDRV_REG_DUTY_DIR_A_m               0x40000000
#define DCSPDRV_REG_DUTY_DIR_B_m               0x80000000

#define DCSPDRV_REG_IRC_o               0x0010

#endif /*MZAPO_REGS_H*/
