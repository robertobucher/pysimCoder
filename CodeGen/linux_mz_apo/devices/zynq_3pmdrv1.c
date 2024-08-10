/*
COPYRIGHT (C) 2022  Dion Beqiri (beqirdio@fel.cvut.cz)

Description: C-code for driving 3-Phase Motor with MCU running NuttX OS


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
#include <stdlib.h>
#include <string.h>

/* supporting files for SPI on NuttX */
#include "zynq_3pmdrv1_mc.h"

/* parameters index definition */

const unsigned char pxmc_lpc_bdc_hal_pos_table[8] =
{
  [0] = 0xff,
  [7] = 0xff,
  [1] = 0, /*0*/
  [5] = 1, /*1*/
  [4] = 2, /*2*/
  [6] = 3, /*3*/
  [2] = 4, /*4*/
  [3] = 5, /*5*/
};

/*  INITIALIZATION FUNCTION  */
static void init(python_block *block)
{
  z3pmdrv1_state_t *mcst;

  block->ptrPar = NULL;

  mcst = malloc(sizeof(*mcst));
  if (mcst == NULL) {
      fprintf(stderr, "malloc mcst failed");
      return;
  }
  memset(mcst, 0, sizeof(*mcst));

  if (z3pmdrv1_init(mcst) < 0) {
      fprintf(stderr, "z3pmdrv1_init mcst failed");
      return;
  }

  block->ptrPar = (void*) mcst;

  z3pmdrv1_transfer(mcst, 1);

  // Initialize conditions

  mcst->curadc_offs[0] = 0; /*2072*/
  mcst->curadc_offs[1] = 0; /*2077*/
  mcst->curadc_offs[2] = 0; /*2051*/

  mcst->pos_offset = -mcst->act_pos;

}

/*  INPUT/OUTPUT  FUNCTION  */
static void inout(python_block *block)
{
  double *pwm_val[3] = {block->u[0], block->u[1], block->u[2]};
  double *pwm_en[3] = {block->u[3], block->u[4], block->u[5]};

  z3pmdrv1_state_t *mcst = (z3pmdrv1_state_t *)block->ptrPar;
  int i;
  double pwm;

  mcst->curadc_sqn_last = mcst->curadc_sqn;

  for (i = 0; i < Z3PMDRV1_CHAN_COUNT; i++)
      mcst->curadc_cumsum_last[i] = mcst->curadc_cumsum[i];

  for (i = 0; i < Z3PMDRV1_CHAN_COUNT; i++) {
      if (pwm_en[i][0]) {
          pwm = pwm_val[i][0] * 2048;
          if (pwm > 2047)
              pwm = 2047;
          if (pwm < 0)
              pwm = 0;
          mcst->pwm[i] = (uint32_t)pwm | Z3PMDRV1_PWM_ENABLE;
      } else {
          mcst->pwm[i] = 0 | Z3PMDRV1_PWM_SHUTDOWN;
      }
  }

  z3pmdrv1_transfer(mcst, 1);

  /* Update outputs */
  double *cur_adc[3] = {block->y[0], block->y[1], block->y[2]};
  double *irc_pos = block->y[3];
  double *irc_idx = block->y[4];
  double *irc_idx_occ = block->y[5];
  double *hal_sec = block->y[6];

  uint32_t curadc_sqn_diff;
  uint32_t curadc_val_diff;
  int diff_to_last_fl = 1;
 #if 0
  static unsigned long sqn_accum;
  static unsigned long sqn_accum_over;
  static unsigned long runs;
  static unsigned long runs_over;
  static unsigned long runs_miss;
  static unsigned int hist[512];
 #endif
  curadc_sqn_diff = mcst->curadc_sqn;
  if (diff_to_last_fl) {
    curadc_sqn_diff -= mcst->curadc_sqn_last;
    curadc_sqn_diff &= 0x1ff;
  }

  if ((curadc_sqn_diff > 1) && (curadc_sqn_diff <= 450)) {
      for (i = 0; i < Z3PMDRV1_CHAN_COUNT; i++) {
          curadc_val_diff = mcst->curadc_cumsum[i];
          if (diff_to_last_fl) {
            curadc_val_diff -= mcst->curadc_cumsum_last[i];
            curadc_val_diff &= 0xffffff;
          }
          cur_adc[i][0] = (double)curadc_val_diff / (double)curadc_sqn_diff -
                          (double)mcst->curadc_offs[i];
      }
     #if 0
      runs++;
      sqn_accum += curadc_sqn_diff;
     #endif
  }
 #if 0
  else {
      if (curadc_sqn_diff) {
        sqn_accum_over += curadc_sqn_diff;
        runs_over++;
      } else runs_miss++;
  }

  hist[curadc_sqn_diff]++;
  if (runs >= 1000) {
    fprintf(stderr, "aver sqn diff %ld runs %ld\n", sqn_accum / runs, runs);
    if (runs_over)
      fprintf(stderr, "over sqn diff %ld runs %ld\n", sqn_accum_over / runs_over, runs_over);
    if (runs_miss)
      fprintf(stderr, "missed runs %ld\n", runs_miss);
    for (i = 0; i < 512; i++) {
      fprintf(stderr, " %d", hist[i]);
      hist[i] = 0;
    }
    fprintf(stderr, "\n");
    runs = 0;
    runs_over = 0;
    runs_miss = 0;
    sqn_accum = 0;
    sqn_accum_over = 0;
  }
 #endif

  irc_pos[0] = (double)(int32_t)(mcst->act_pos + mcst->pos_offset);
  irc_idx[0] = (double)(int32_t)(mcst->index_pos + mcst->pos_offset);
  irc_idx_occ[0] = (double)mcst->index_occur;
  hal_sec[0] = (double)pxmc_lpc_bdc_hal_pos_table[mcst->hal_sensors];
}

/*  TERMINATION FUNCTION  */
static void end(python_block *block)
{
  z3pmdrv1_state_t *mcst = (z3pmdrv1_state_t *)block->ptrPar;

  if (mcst != NULL) {
      block->ptrPar = NULL;
      free(mcst);
  }

}

void zynq_3pmdrv1(int flag, python_block *block)
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
