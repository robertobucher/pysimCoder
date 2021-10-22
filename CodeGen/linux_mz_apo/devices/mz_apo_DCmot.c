/*
COPYRIGHT (C) 2021  Dion Beqiri (beqirdio@fel.cvut.cz)

Description: C-code for reading encoders on the MZ_APO education kit
				using PysimCoder Simulator

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
#include <stdio.h>
#include <stdint.h>

/* supporting files for MZ_APO board */
#include <mzapo_regs.h>
#include <phys_address_access.h>

/* parameters index definition */
#define INTPAR_IDX_MOT_ID           0
#define INTPAR_IDX_MOT_POS          1

/*  INITIALIZATION FUNCTION  */
static void init(python_block *block)
{
  int * intPar = block->intPar;
  
  /* ----- Init ptrPar ----- */
  mem_address_map_t *memadrs_dcmot;
  block->ptrPar = NULL;
  
  /* Map physical address of DC motor interface to virtual address */
  if (intPar[INTPAR_IDX_MOT_ID] == 0) {
      memadrs_dcmot = mem_address_map_create(DCSPDRV_REG_BASE_PHYS_0, DCSPDRV_REG_SIZE, 0);
  } else {
      memadrs_dcmot = mem_address_map_create(DCSPDRV_REG_BASE_PHYS_1, DCSPDRV_REG_SIZE, 0);
  }
  /* Check for errors */
	if (memadrs_dcmot == NULL) {
      fprintf(stderr,  "Error when accessing physical address.\n");
      exit(1);
	}
  /* Save memory map structure to block->ptrPar */
  block->ptrPar = memadrs_dcmot;
  
  /* ----- Init intPar[INTPAR_IDX_MOT_POS] ----- */
  int32_t *irc_pos_state;
  intPar[INTPAR_IDX_MOT_POS] = (int)NULL;
  /* Alloc memory for position state */
  irc_pos_state = malloc(sizeof(*irc_pos_state));
  /* Check for errors */
  if (irc_pos_state == NULL) {
      fprintf(stderr, "Error when calling malloc.\n");
      exit(1);
  }
  /* Update position */
  *irc_pos_state = 0;
  /* Save position to intPar[INTPAR_IDX_MOT_POS] */
  intPar[INTPAR_IDX_MOT_POS] = (int)irc_pos_state;

  /* Reset IRC counter (and disable DC motor PWM) */
	mem_address_reg_wr(memadrs_dcmot, DCSPDRV_REG_CR_o, DCSPDRV_REG_CR_IRC_RESET_m);
	/* Set frequency of DC motor PWM to 20 kHz (period is given in multiples of 10 ns) */
	mem_address_reg_wr(memadrs_dcmot, DCSPDRV_REG_PERIOD_o, 5000 & DCSPDRV_REG_PERIOD_MASK_m);
	/* Set DC motor PWM duty cycle to 0 (given in multiples of 10 ns, hence it should be less than 5000) */
	mem_address_reg_wr(memadrs_dcmot, DCSPDRV_REG_DUTY_o, 0);
	/* Enable DC motor PWM */
	mem_address_reg_wr(memadrs_dcmot, DCSPDRV_REG_CR_o, DCSPDRV_REG_CR_PWM_ENABLE_m);

}

/*  INPUT/OUTPUT  FUNCTION  */
static void inout(python_block *block)
{
  int * intPar    = block->intPar;
  double *u = block->u[0];
  double *y = block->y[0];
  double pwm_inp = u[0];
    
  mem_address_map_t *memadrs_dcmot = (mem_address_map_t *)block->ptrPar;
  int32_t *irc_pos = (int32_t *)intPar[INTPAR_IDX_MOT_POS];
  
  /* Get IRC position */
  *irc_pos = mem_address_reg_rd(memadrs_dcmot, DCSPDRV_REG_IRC_o);
  
  /* Set PWM */
  double pwm;
  pwm = pwm_inp * 5000;
  if (pwm > 5000) pwm = 5000;
  if (pwm < -5000) pwm = -5000;
  /* Set direction of Motor*/
  if (pwm > 0) {
      mem_address_reg_wr(memadrs_dcmot, DCSPDRV_REG_DUTY_o, (uint32_t)  pwm | DCSPDRV_REG_DUTY_DIR_A_m);
  } else {
      mem_address_reg_wr(memadrs_dcmot, DCSPDRV_REG_DUTY_o, (uint32_t) -pwm | DCSPDRV_REG_DUTY_DIR_B_m);
  }
  /*  Update output from IRC  */ 
  y[0] = *irc_pos;
}

/*  TERMINATION FUNCTION  */
static void end(python_block *block)
{
  int * intPar    = block->intPar;
  mem_address_map_t *memadrs_dcmot = (mem_address_map_t *)block->ptrPar;
    
  /* Set PWM to 0 */
  mem_address_reg_wr(memadrs_dcmot, DCSPDRV_REG_DUTY_o, 0);
  
  /* Disable PWM */
  mem_address_reg_wr(memadrs_dcmot, DCSPDRV_REG_CR_o, 0);
  
  int32_t *irc_pos = (int32_t *)intPar[INTPAR_IDX_MOT_POS];
  
  if (memadrs_dcmot != NULL) {
      block->ptrPar = NULL;
      free(memadrs_dcmot);
  }
  if (irc_pos != NULL) {
      intPar[INTPAR_IDX_MOT_POS] = (int)NULL;
      free(irc_pos);
  }
  
}

void mz_apo_DCmot(int flag, python_block *block)
{
  if (flag==CG_OUT){          /* input / output */
    inout(block);
  }
  else if (flag==CG_END){     /* termination */
    end(block);
  }
  else if (flag ==CG_INIT){    /* initialisation */
    init(block);
  }
}
