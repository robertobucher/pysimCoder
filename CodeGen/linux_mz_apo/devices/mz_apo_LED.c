/*
COPYRIGHT (C) 2024  David Storek (storedav@fel.cvut.cz)

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
#include <math.h>

/* supporting files for MZ_APO board */
#include "mzapo_regs.h"
#include "phys_address_access.h"

/* parameters index definition */
#define REALPAR_IDX_MIN_VAL          0
#define REALPAR_IDX_MAX_VAL          1

#define INTPAR_IDX_LED_OFFSET        0
#define INTPAR_IDX_LED_LEN           1

#define LEDS_NR                      32

double calculate_percentage_in_range(double val, double min, double max){
  double min_max_dist = max - min;
  double input_in_range = val - min;

  double percentage_filled = input_in_range / min_max_dist;
  percentage_filled = percentage_filled < 0.0 ? 0.0 : percentage_filled;
  percentage_filled = percentage_filled > 1.0 ? 1.0 : percentage_filled;

  return percentage_filled;
}

uint32_t get_LED_mask(int offset, int len){
    int start = offset;
    int end = start + abs(len);

    start = start < 0 ? 0 : start;
    start = start > LEDS_NR ? LEDS_NR : start;

    end = end > LEDS_NR ? LEDS_NR : end;
    end = end < 0 ? 0 : end;

    return (0xffffffff >> (start + LEDS_NR - end)) << (LEDS_NR - end);
}

void set_LEDs(mem_address_map_t *memadrs_LEDs, uint32_t mask, uint32_t values){
  uint32_t current_led_status = mem_address_reg_rd(memadrs_LEDs, SPILED_REG_LED_LINE_o);
  mem_address_reg_wr(memadrs_LEDs, SPILED_REG_LED_LINE_o, (current_led_status & ~mask) | (values & mask));
}

/*  INITIALIZATION FUNCTION  */
static void init(python_block *block)
{
  double * realPar    = block->realPar;
  double min_val = realPar[REALPAR_IDX_MIN_VAL];
  double max_val = realPar[REALPAR_IDX_MAX_VAL];

  int * intPar    = block->intPar;
  int led_offset = intPar[INTPAR_IDX_LED_OFFSET];
  int led_len = intPar[INTPAR_IDX_LED_LEN];

  double *u = block->u[0];
  double input_val = u[0];
  mem_address_map_t *memadrs_LEDs;

  block->ptrPar = NULL;

  /* Map physical address of knobs to virtual address */
  memadrs_LEDs = mem_address_map_create(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);

  /* Check for errors */
  if (memadrs_LEDs == NULL) {
    fprintf(stderr,  "Error when accessing physical address.\n");
    exit(1);
  }

  /* Save memory map structure to ptrPar */
  block->ptrPar = memadrs_LEDs;

  /*Set leds to OFF*/
  set_LEDs(memadrs_LEDs, get_LED_mask(led_offset, led_len), 0);

//   fprintf(stderr, "init 6\n");

}

/*  INPUT/OUTPUT  FUNCTION  */
static void inout(python_block *block)
{
  double * realPar    = block->realPar;
  double min_val = realPar[REALPAR_IDX_MIN_VAL];
  double max_val = realPar[REALPAR_IDX_MAX_VAL];

  int * intPar    = block->intPar;
  int led_offset = intPar[INTPAR_IDX_LED_OFFSET];
  int led_len = intPar[INTPAR_IDX_LED_LEN];

  double *u = block->u[0];
  double input_val = u[0];

  mem_address_map_t *memadrs_LEDs = (mem_address_map_t *)block->ptrPar;

  /*calculate percentage of shown LEDs*/
  double percentage_filled = calculate_percentage_in_range(input_val, min_val, max_val);

  /*translate to LEDs*/
  int leds_enabled_nr = (int)round(percentage_filled * abs(led_len));
  uint32_t leds_shining;
  if(led_len > 0){
    // shine increases left to right
    leds_shining = 0xffffffff << (LEDS_NR - leds_enabled_nr);
  }else{
    // shine increases right to left
    leds_shining = 0xffffffff >> (abs(led_len) - leds_enabled_nr);
  }

  if(led_offset < 0){
    leds_shining = leds_shining << (led_offset < -32 ? 32 : -led_offset);
  }else{
    leds_shining = leds_shining >> (led_offset > 32 ? 32 : led_offset);
  }

  /*write enabled leds to mem IO*/
  set_LEDs(memadrs_LEDs, get_LED_mask(led_offset, led_len), leds_shining);

//   printf("input: %f, percentage: %f\n", input_val, percentage_filled);
}

/*  TERMINATION FUNCTION  */
static void end(python_block *block)
{
  /* Free memory and unmap from virtual address space*/
  mem_address_map_t *memadrs_LEDs = (mem_address_map_t *)block->ptrPar;

  /*Set leds to OFF*/
  int * intPar    = block->intPar;
  int led_offset = intPar[INTPAR_IDX_LED_OFFSET];
  int led_len = intPar[INTPAR_IDX_LED_LEN];
  set_LEDs(memadrs_LEDs, get_LED_mask(led_offset, led_len), 0);

  mem_address_unmap_and_free(memadrs_LEDs);

  block->ptrPar = NULL;
}

void mz_apo_LED(int flag, python_block *block)
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


