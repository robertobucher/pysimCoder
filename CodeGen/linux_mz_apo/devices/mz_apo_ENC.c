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
#define INTPAR_IDX_CHANNEL           0
#define INTPAR_IDX_INIT_VAL          1
#define INTPAR_IDX_VALUE_RAW         2
#define INTPAR_IDX_VALUE_OFFS        3

/*  INITIALIZATION FUNCTION  */
static void init(python_block *block)
{
  int * intPar    = block->intPar;
  uint32_t knob_value;
  int channel = intPar[INTPAR_IDX_CHANNEL];
  int init_val = intPar[INTPAR_IDX_INIT_VAL];

  mem_address_map_t *memadrs_knob;
 
  block->ptrPar = NULL;

  /* Map physical address of knobs to virtual address */
  memadrs_knob = mem_address_map_create(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);

  /* Check for errors */
  if (memadrs_knob == NULL) {
    fprintf(stderr,  "Error when accessing physical address.\n");
    exit(1);
  }

  /* Save memory map structure to ptrPar */
  block->ptrPar = memadrs_knob;

  /* Read actual knobs position value from hardware */
  knob_value = mem_address_reg_rd(memadrs_knob, SPILED_REG_KNOBS_8BIT_o);

  knob_value >>= 8 * (2 - channel);
  knob_value &= 0xff;
  intPar[INTPAR_IDX_VALUE_RAW] = knob_value;
  intPar[INTPAR_IDX_VALUE_OFFS] = init_val - knob_value;

}

/*  INPUT/OUTPUT  FUNCTION  */
static void inout(python_block *block)
{
  int * intPar    = block->intPar;
  double *y = block->y[0];
  uint32_t knob_value;
  int channel = intPar[INTPAR_IDX_CHANNEL];

  mem_address_map_t *memadrs_knob = (mem_address_map_t *)block->ptrPar;

  /* Read actual knobs position value from hardware */
  knob_value = mem_address_reg_rd(memadrs_knob, SPILED_REG_KNOBS_8BIT_o);
  knob_value >>= 8 * (2 - channel);
  knob_value &= 0xff;
  intPar[INTPAR_IDX_VALUE_RAW] += (int8_t)(knob_value - intPar[INTPAR_IDX_VALUE_RAW]);
  
  /* Update output */
  y[0] = intPar[INTPAR_IDX_VALUE_RAW] + intPar[INTPAR_IDX_VALUE_OFFS];
}

/*  TERMINATION FUNCTION  */
static void end(python_block *block)
{
  /* Free memory and unmap from virtual address space*/
  mem_address_map_t *memadrs_knob = (mem_address_map_t *)block->ptrPar;
  mem_address_unmap_and_free(memadrs_knob);
    
  block->ptrPar = NULL;
}

void mz_apo_ENC(int flag, python_block *block)
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


