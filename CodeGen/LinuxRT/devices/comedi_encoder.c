/*
COPYRIGHT (C) 2022  Roberto Bucher (roberto.bucher@supsi.ch)

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

//** -------------------------- COMEDI ENCODER INPUT --------------------------------------

//**  General includes 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/types.h>
#include <asm/types.h>
#include <math.h>

/* Specific for python block developement */
#include <pyblock.h>

/* Specific for Comedilib */
#include <comedilib.h>

#define COMEDI_SUBD_INCREMENTALCOUNTER  17  /* Incremental Counter */

//** Comedi static structure inside block->work  
typedef struct{
  char ComediDevName[20]; //** 
  comedi_t *dev; //** Comedi interface handler*/
  int subdevice;       //** Comedi subdevice
  int channel   ;        //** channel selection
  int a;
  int b;
  int z;
  int initial_value;
} ComediEnc;

int ni_gpct_start_encoder(comedi_t *device, unsigned subdevice,
	unsigned int initial_value,
	int a, int b, int z)
{
	int retval;
	lsampl_t counter_mode;


	retval = comedi_reset(device, subdevice);
	/* set initial counter value by writing to channel 0 */
	retval = comedi_data_write(device, subdevice, 0, 0, 0, initial_value);
	/* set "load a" register to initial_value by writing to channel 1 */
	retval = comedi_data_write(device, subdevice, 1, 0, 0, initial_value);
	/* set "load b" register to initial_value by writing to channel 2 */
	retval = comedi_data_write(device, subdevice, 2, 0, 0, initial_value);

	comedi_set_gate_source(device, subdevice, 0, 0, NI_GPCT_DISABLED_GATE_SELECT);
	comedi_set_gate_source(device, subdevice, 0, 1, NI_GPCT_DISABLED_GATE_SELECT);
	/* note, the comedi_set_other_source calls will fail on 660x boards, since they
	 * don't support user selection of the inputs used for the A/B/Z signals. */
	comedi_set_other_source(device, subdevice, 0, NI_GPCT_SOURCE_ENCODER_A, a);
	comedi_set_other_source(device, subdevice, 0, NI_GPCT_SOURCE_ENCODER_B, b);
	comedi_set_other_source(device, subdevice, 0, NI_GPCT_SOURCE_ENCODER_Z, z);

	counter_mode = (NI_GPCT_COUNTING_MODE_QUADRATURE_X4_BITS |
		NI_GPCT_COUNTING_DIRECTION_HW_UP_DOWN_BITS);
	if (z != NI_GPCT_DISABLED_GATE_SELECT) {
		counter_mode |= (NI_GPCT_INDEX_ENABLE_BIT |
			NI_GPCT_INDEX_PHASE_HIGH_A_HIGH_B_BITS);
	}
	retval = comedi_set_counter_mode(device, subdevice, 0, counter_mode);
	if(retval < 0) return retval;

	/* retval = comedi_arm(device, subdevice, NI_GPCT_ARM_IMMEDIATE); */
	/* if(retval < 0) return retval; */

	return 0;

/* > insn.insn       = INSN_CONFIG; */
/* > insn.n          = 1; // Is relevant to config! :) */
/* > config_data[0]  = INSN_CONFIG_GPCT_QUADRATURE_ENCODER; */
/* > config_data[1]  = quadraturemode; // {GPCT_X1, GPCT_X2, or GPCT_X4} */
/* > config_data[2]  = indexphase; // {GPCT_IndexPhaseHighHigh,LH,LL,HL} */
/* > config_data[3]  = indexreset; // {0,GPCT_RESET_COUNTER_ON_INDEX} */
/* > insn.data       = config_data; */
/* > insn.subdev     = subdev; */
/* > insn.chanspec   = CR_PACK(channel,0,0); */
}

static void init(python_block *block)
{
  int * intPar    = block->intPar;
  int retval;
  ComediEnc *ENCdata ; //** define the local datastructure 
  comedi_t *dev = NULL ; //** define the local datastructure 

  block->ptrPar = malloc(sizeof(ComediEnc));
  if (block->ptrPar  == NULL ) { //** in case of error exit
    fprintf(stdout, "Error by malloc\n"); 
    exit(-1); 
  }
 
  ENCdata = (ComediEnc *) block->ptrPar; // local structure to python_block 
  sprintf(ENCdata->ComediDevName,block->str); // create device
  ENCdata->dev  = comedi_open(ENCdata->ComediDevName);
  
  if (!(ENCdata->dev)){
    fprintf(stdout, "No device\n"); 
    exit(-1);
  }

  ENCdata->channel  = block->intPar[0] ; // channel selection
  ENCdata->a = NI_GPCT_DISABLED_OTHER_SELECT;
  ENCdata->b = NI_GPCT_DISABLED_OTHER_SELECT;
  ENCdata->z = NI_GPCT_DISABLED_OTHER_SELECT;
  ENCdata->initial_value= 0;
    
  ENCdata->subdevice = comedi_find_subdevice_by_type(ENCdata->dev,
						     COMEDI_SUBD_COUNTER, 0);
  if(ENCdata->subdevice<0){
    fprintf(stdout, "Comedi find_subdevice failed (No Incremental counter)\n");
    comedi_close(ENCdata->dev);
    exit(-1);
  }
  
  retval = ni_gpct_start_encoder(ENCdata->dev, ENCdata->subdevice,
				 ENCdata->initial_value, ENCdata->a,
				 ENCdata->b, ENCdata->z);

  if(retval < 0){
    fprintf(stdout, "Error by starting encoder\n");
    comedi_close(ENCdata->dev);
    exit(-1);
  }
    fprintf(stdout, "Inizialization ok\n");
}

static void inout(python_block *block)
{
  int * intPar    = block->intPar;
  double *y = block->y[0];
  ComediEnc *ENCdata = (ComediEnc *)  block->ptrPar;
  
}

static void update(python_block *block)
{
  /* double * realPar = block->realPar; */
  /* int * intPar    = block->intPar; */
  /* double *y = block->y[0]; */
  /* double *u = block->u[0]; */
  
}

static void end(python_block *block)
{
  /* double * realPar = block->realPar; */
  /* int * intPar    = block->intPar; */
  /* double *y = block->y[0]; */
  /* double *u = block->u[0]; */

}

void comedi_encoder(int flag, python_block *block)
{
  if (flag==CG_OUT){          /* get input */
    inout(block);
  }
  else if (flag==CG_END){     /* termination */ 
    end(block);
  }
  else if (flag ==CG_INIT){    /* initialisation */
    init(block);
  }
}




