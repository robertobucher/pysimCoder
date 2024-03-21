/*
  Communication with Zynq equipped by 3-phase
  motor driver connected to MicroZed on MZ_APO board.
  MZ_APO and 3-phase motor driver boards have been
  designed by Petr Porazil for PiKRON company.
  The Zynq VHDL design by Pavel Pisa, partially
  inspired by previous work done together with
  Martin Prudek.

  (C) 2017 by Pavel Pisa ppisa@pikron.com
*/

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include "zynq_3pmdrv1_mc.h"

#define Z3PMDRV1_REG_BASE_PHYS     0x43c20000
#define Z3PMDRV1_REG_SIZE          0x00001000

#define Z3PMDRV1_REG_IRC_POS_o         0x0008
#define Z3PMDRV1_REG_IRC_IDX_POS_o     0x000C

#define Z3PMDRV1_REG_PWM1_o            0x0010
#define Z3PMDRV1_REG_PWM2_o            0x0014
#define Z3PMDRV1_REG_PWM3_o            0x0018

#define Z3PMDRV1_REG_PWMX_VAL_m    0x00003fff
#define Z3PMDRV1_REG_PWMX_EN_m     0x40000000
#define Z3PMDRV1_REG_PWMX_SHDN_m   0x80000000

#define Z3PMDRV1_REG_ADC_SQN_STAT_o    0x0020

#define Z3PMDRV1_REG_ADSQST_SQN_m  0x000001ff
#define Z3PMDRV1_REG_ADSQST_HAL1_m 0x00010000
#define Z3PMDRV1_REG_ADSQST_HAL2_m 0x00020000
#define Z3PMDRV1_REG_ADSQST_HAL3_m 0x00040000
#define Z3PMDRV1_REG_ADSQST_ST1_m  0x00100000
#define Z3PMDRV1_REG_ADSQST_ST2_m  0x00200000
#define Z3PMDRV1_REG_ADSQST_ST3_m  0x00400000
#define Z3PMDRV1_REG_ADSQST_PWST_m 0x01000000

#define Z3PMDRV1_REG_ADC1_o            0x0024
#define Z3PMDRV1_REG_ADC2_o            0x0028
#define Z3PMDRV1_REG_ADC3_o            0x002C

char *memdev="/dev/mem";

static inline
uint32_t z3pmdrv1_reg_rd(z3pmdrv1_state_t *z3pmcst, unsigned reg_offs)
{
	return *(volatile uint32_t*)((char*)z3pmcst->regs_base_virt + reg_offs);
}

static inline
void z3pmdrv1_reg_wr(z3pmdrv1_state_t *z3pmcst, unsigned reg_offs, uint32_t val)
{
	*(volatile uint32_t*)((char*)z3pmcst->regs_base_virt + reg_offs) = val;
}

int z3pmdrv1_transfer(z3pmdrv1_state_t *z3pmcst, int flags)
{
	uint32_t sqn_stat;
	uint32_t pwm1, pwm2, pwm3;
	uint32_t pwm1_fl = 0;
	uint32_t pwm2_fl = 0;
	uint32_t pwm3_fl = 0;
	uint32_t idx;

	pwm1 = z3pmcst->pwm[0];
	pwm2 = z3pmcst->pwm[1];
	pwm3 = z3pmcst->pwm[2];

	if (pwm1 & Z3PMDRV1_PWM_ENABLE)
	  pwm1_fl |= Z3PMDRV1_REG_PWMX_EN_m;
	if (pwm1 & Z3PMDRV1_PWM_SHUTDOWN)
	  pwm1_fl |= Z3PMDRV1_REG_PWMX_SHDN_m;
	if (pwm2 & Z3PMDRV1_PWM_ENABLE)
	  pwm2_fl |= Z3PMDRV1_REG_PWMX_EN_m;
	if (pwm2 & Z3PMDRV1_PWM_SHUTDOWN)
	  pwm2_fl |= Z3PMDRV1_REG_PWMX_SHDN_m;
	if (pwm3 & Z3PMDRV1_PWM_ENABLE)
	  pwm3_fl |= Z3PMDRV1_REG_PWMX_EN_m;
	if (pwm3 & Z3PMDRV1_PWM_SHUTDOWN)
	  pwm3_fl |= Z3PMDRV1_REG_PWMX_SHDN_m;

	pwm1 &= Z3PMDRV1_PWM_VALUE_m;
	pwm2 &= Z3PMDRV1_PWM_VALUE_m;
	pwm3 &= Z3PMDRV1_PWM_VALUE_m;

	if (pwm1 > Z3PMDRV1_REG_PWMX_VAL_m)
		pwm1 = Z3PMDRV1_REG_PWMX_VAL_m;
	if (pwm2 > Z3PMDRV1_REG_PWMX_VAL_m)
		pwm2 = Z3PMDRV1_REG_PWMX_VAL_m;
	if (pwm3 > Z3PMDRV1_REG_PWMX_VAL_m)
		pwm3 = Z3PMDRV1_REG_PWMX_VAL_m;

	z3pmdrv1_reg_wr(z3pmcst, Z3PMDRV1_REG_PWM1_o, pwm1 | pwm1_fl);
	z3pmdrv1_reg_wr(z3pmcst, Z3PMDRV1_REG_PWM2_o, pwm2 | pwm2_fl);
	z3pmdrv1_reg_wr(z3pmcst, Z3PMDRV1_REG_PWM3_o, pwm3 | pwm3_fl);

	z3pmcst->act_pos = z3pmdrv1_reg_rd(z3pmcst, Z3PMDRV1_REG_IRC_POS_o);
	idx = z3pmdrv1_reg_rd(z3pmcst, Z3PMDRV1_REG_IRC_IDX_POS_o);

	if (idx ^ z3pmcst->index_pos) {
		z3pmcst->index_occur += 1;
	}
	z3pmcst->index_pos = idx;

	if (flags & 1) {
		z3pmcst->curadc_sqn_last = z3pmcst->curadc_sqn;
		z3pmcst->curadc_cumsum_last[0] = z3pmcst->curadc_cumsum[0];
		z3pmcst->curadc_cumsum_last[1] = z3pmcst->curadc_cumsum[1];
		z3pmcst->curadc_cumsum_last[2] = z3pmcst->curadc_cumsum[2];
	}

	sqn_stat = z3pmdrv1_reg_rd(z3pmcst, Z3PMDRV1_REG_ADC_SQN_STAT_o);
	z3pmcst->curadc_sqn = sqn_stat & Z3PMDRV1_REG_ADSQST_SQN_m;

	z3pmcst->curadc_cumsum[2] = z3pmdrv1_reg_rd(z3pmcst, Z3PMDRV1_REG_ADC3_o);

	z3pmcst->curadc_cumsum[0] = z3pmdrv1_reg_rd(z3pmcst, Z3PMDRV1_REG_ADC2_o);

	z3pmcst->curadc_cumsum[1] = z3pmdrv1_reg_rd(z3pmcst, Z3PMDRV1_REG_ADC1_o);

	z3pmcst->hal_sensors =
		((sqn_stat & Z3PMDRV1_REG_ADSQST_HAL1_m)?1:0) |
		((sqn_stat & Z3PMDRV1_REG_ADSQST_HAL2_m)?2:0) |
		((sqn_stat & Z3PMDRV1_REG_ADSQST_HAL3_m)?4:0);

	return 0;
}


/*
 * The support function which returns pointer to the virtual
 * address at which starts remapped physical region in the
 * process virtual memory space.
 */
static inline
void *map_phys_address(off_t region_base, size_t region_size, int opt_cached)
{
	unsigned long mem_window_size;
	unsigned long pagesize;
	unsigned char *mm;
	unsigned char *mem;
	int fd;

	/*
	 * Open a device ("/dev/mem") representing physical address space
	 * in POSIX systems
	 */
	fd = open(memdev, O_RDWR | (!opt_cached? O_SYNC: 0));
	if (fd < 0) {
		fprintf(stderr, "cannot open %s\n", memdev);
		return NULL;
	}

	/*
	 * The virtual to physical address mapping translation granularity
	 * corresponds to memory page size. This call obtains the page
	 * size used by running operating system at given CPU architecture.
	 * 4kB are used by Linux running on ARM, ARM64, x86 and x86_64 systems.
	 */
	pagesize=sysconf(_SC_PAGESIZE);

	/*
	 * Extend physical region start address and size to page size boundaries
	 * to cover complete requested region.
	 */
	mem_window_size = ((region_base & (pagesize-1)) +
			  region_size + pagesize-1) & ~(pagesize-1);

	/*
	 * Map file (in our case physical memory) range at specified offset
	 * to virtual memory ragion/area (see VMA Linux kernel structures)
	 * of the process.
	 */
	mm = mmap(NULL, mem_window_size, PROT_WRITE|PROT_READ,
		MAP_SHARED, fd, region_base & ~(pagesize-1));

	/* Report failure if the mmap is not allowed for given file or its region */
	if (mm == MAP_FAILED) {
		return NULL;
	}

	/*
	 * Add offset in the page to the returned pointer for non-page-aligned
	 * requests.
	 */
	mem = mm + (region_base & (pagesize-1));

	return mem;
}

int z3pmdrv1_init(z3pmdrv1_state_t *z3pmcst)
{
	int ret = 0;
	uint32_t sqn_stat;

	if (z3pmcst->regs_base_phys == 0) {
		z3pmcst->regs_base_phys = Z3PMDRV1_REG_BASE_PHYS;
	}

	z3pmcst->regs_base_virt = map_phys_address(z3pmcst->regs_base_phys,
					Z3PMDRV1_REG_SIZE, 0);

	if (z3pmcst->regs_base_virt == NULL) {
		ret = -1;
		return ret;
	}

	sqn_stat = z3pmdrv1_reg_rd(z3pmcst, Z3PMDRV1_REG_ADC_SQN_STAT_o);
	z3pmcst->curadc_sqn = sqn_stat & Z3PMDRV1_REG_ADSQST_SQN_m;
	z3pmcst->curadc_sqn_last = z3pmcst->curadc_sqn;

	z3pmcst->curadc_cumsum[2] = z3pmdrv1_reg_rd(z3pmcst, Z3PMDRV1_REG_ADC3_o);

	z3pmcst->curadc_cumsum[0] = z3pmdrv1_reg_rd(z3pmcst, Z3PMDRV1_REG_ADC2_o);

	z3pmcst->curadc_cumsum[1] = z3pmdrv1_reg_rd(z3pmcst, Z3PMDRV1_REG_ADC1_o);

	z3pmcst->curadc_cumsum_last[0] = z3pmcst->curadc_cumsum[0];
	z3pmcst->curadc_cumsum_last[1] = z3pmcst->curadc_cumsum[1];
	z3pmcst->curadc_cumsum_last[2] = z3pmcst->curadc_cumsum[2];

	z3pmcst->index_pos = z3pmdrv1_reg_rd(z3pmcst, Z3PMDRV1_REG_IRC_IDX_POS_o);

	z3pmcst->curadc_use_diff_to_last_fl = 1;

	return ret;
}
