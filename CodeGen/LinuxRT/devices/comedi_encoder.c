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

/*DEFINES*/
 
#define ZEROCNT 8388608
//** Comedi static structure inside block->work  
typedef struct{
  comedi_t *dev;
  int subdev;
  unsigned int channel;
  int a;
  int b;
  int z;
  int initial_value;
} ComediEnc;

extern void *ComediDev[];
extern int ComediDev_InUse[];
extern int ComediDev_CNTInUse[];

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
      

	retval = comedi_set_clock_source(device,subdevice,0,40,0);
	//NI_GPCT_PFI_CLOCK_SRC_BITS((subdevice==11)?8:3) can replace the 4th argument
	if(retval< 0)comedi_perror("comedi_set_clock_source");
	
	//changing the mode to NORMAL_BITS, X1 ,X2,X4, TWO_PULSE_BITS or SYNC_SOURCE_BITS
	//(all defined in comedi.h) does not have any effect
	counter_mode = NI_GPCT_COUNTING_MODE_QUADRATURE_X4_BITS;
	
	// output pulse on terminal count (doesn't really matter for this application)
	counter_mode |= NI_GPCT_OUTPUT_TC_PULSE_BITS;
	
	// Don't alternate the reload source between the load a and load b registers.
	// Doesn't really matter here, since we aren't going to be reloading the counter
	counter_mode |= NI_GPCT_RELOAD_SOURCE_FIXED_BITS;

       	// count on up/down signal, if disabled: counts only down whether the rotation
	//sense is clock-wise or counter-clock-wise
	counter_mode|=NI_GPCT_COUNTING_DIRECTION_HW_UP_DOWN_BITS;
	
	// don't stop on terminal count, if disabled: no visible difference
	counter_mode |= NI_GPCT_STOP_ON_GATE_BITS;
	
	// don't disarm on terminal count or gate signal, if disabled: no visible difference
	counter_mode |= NI_GPCT_NO_HARDWARE_DISARM_BITS;
	
	retval = comedi_set_counter_mode(device, subdevice, 0, counter_mode);
	if(retval < 0) return retval;

	retval = comedi_arm(device, subdevice, NI_GPCT_ARM_IMMEDIATE); 
        if(retval < 0) return retval; 

	return 0;
}

static void init(python_block *block)
{
  int * intPar    = block->intPar;
  int retval;
  int index = block->str[11]-'0';
  const char * board;
  int n_channels;
  ComediEnc *ENC;

  block->ptrPar = malloc(sizeof(ComediEnc));
  if (block->ptrPar  == NULL ) {
    fprintf(stdout, "Error by malloc\n"); 
    exit(-1); 
  }
 
  ENC = (ComediEnc *) block->ptrPar;
  ENC->channel  = block->intPar[0];

  ENC->a = NI_GPCT_SOURCE_ENCODER_A;
  ENC->b = NI_GPCT_SOURCE_ENCODER_B;
  ENC->z = NI_GPCT_SOURCE_ENCODER_Z;
  ENC->initial_value= ZEROCNT;

  if (!ComediDev[index]) {
    ENC->dev  = comedi_open(block->str);
    if (!(ENC->dev)){
      fprintf(stdout, "Comedi open failed\n");
      exit(-1);
    }
    board = comedi_get_board_name(ENC->dev);
    printf("COMEDI %s (%s) opened.\n\n", block->str, board);
    ComediDev[index] = ENC->dev;

    
    ENC->subdev = comedi_find_subdevice_by_type(ENC->dev,
						     COMEDI_SUBD_COUNTER, 0);
    printf("COMEDI_SUBD_COUNTER=%d\n",COMEDI_SUBD_COUNTER);
    printf("a=%d b=%d Subdev: %d\n",ENC->a, ENC->b, ENC->subdev);
    printf("channel %d\n",ENC->channel);
    
    if(ENC->subdev<0){
      fprintf(stdout, "Comedi find_subdevice failed (No Incremental counter)\n");
      comedi_close(ENC->dev);
      exit(-1);
    }
    
    if ((comedi_lock(ENC->dev, ENC->subdev)) < 0) {
      fprintf(stdout, "Comedi lock failed for subdevice %d\n", ENC->subdev);
      comedi_close(ENC->dev);
      exit(-1);
    }
    printf("Comedi locked\n");
  }
  else {
    ENC->dev = ComediDev[index];
    ENC->subdev = comedi_find_subdevice_by_type(ENC->dev, COMEDI_SUBD_COUNTER, 0);
  }

  if ((n_channels = comedi_get_n_channels(ENC->dev, ENC->subdev)) < 0) {
    fprintf(stdout, "Comedi get_n_channels failed for subdevice %d\n", ENC->subdev);
    comedi_unlock(ENC->dev, ENC->subdev);
    comedi_close(ENC->dev);
    exit(-1);
  }
  printf("n_channels: %d\n", n_channels);
  
  if (ENC->channel >= n_channels) {
    fprintf(stdout, "Comedi channel not available for subdevice %d\n", ENC->subdev);
    comedi_unlock(ENC->dev, ENC->subdev);
    comedi_close(ENC->dev);
    exit(1);
  }

  ComediDev_InUse[index]++;
  ComediDev_CNTInUse[index]++;
  printf("Comedi started...\n");

  retval = ni_gpct_start_encoder(ENC->dev, ENC->subdev,
				 ENC->initial_value, ENC->a,
				 ENC->b, ENC->z);

  if(retval < 0){
    fprintf(stdout, "Error by starting encoder\n");
    comedi_unlock(ENC->dev, ENC->subdev);
    comedi_close(ENC->dev);
    exit(-1);
  }
  fprintf(stdout, "Inizialization ok\n");
}

static void inout(python_block *block)
{
  int * intPar    = block->intPar;
  double *y = block->y[0];
  ComediEnc *ENC = (ComediEnc *)  block->ptrPar;
 
  lsampl_t encData;   
  comedi_data_read(ENC->dev, ENC->subdev, ENC->channel, 0,
		   AREF_GROUND,  &encData);
  
  *y = (double) ((long int) encData)-ZEROCNT;
}

static void end(python_block *block)
{
   int index  = block->str[11]-'0';
  ComediEnc *ENC  = block->ptrPar;

  ComediDev_InUse[index]--;
  ComediDev_CNTInUse[index]--;
  if (!ComediDev_CNTInUse[index]) {
    comedi_unlock(ENC->dev, ENC->subdev);
  }
  if (!ComediDev_InUse[index]) {
    comedi_close(ENC->dev);
    printf("\nCOMEDI %s closed.\n\n", block->str);
    ComediDev[index] = NULL;
  }
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




