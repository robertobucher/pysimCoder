/*
COPYRIGHT (C) 2007  Dan D. V. Bhanderi (dan@bhanderi.dk)
COPYRIGHT (C) 2022  Roberto Bucher (roberto.bucher@supsi.ch)

Developed after the comedi blocks of ert_linux fir simulink

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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <comedilib.h>

#include <pyblock.h>

extern void *ComediDev[];
extern int ComediDev_InUse[];
extern int ComediDev_DIOInUse[];

//** Comedi static structure inside block->work  
typedef struct 
{
  comedi_t *dev;
  int subdev;
  unsigned int channel;
  double threshold;
} ComediDigOut ;

static void init(python_block *block)
{
  double * realPar = block->realPar;
  int * intPar    = block->intPar;
  int subdev_type = -1;
  int index = block->str[11]-'0';
  int n_channels;
  const char * board;
  ComediDigOut *DO;

  block->ptrPar = malloc(sizeof(ComediDigOut));
  DO = block->ptrPar;
  DO->channel = intPar[0];
  DO->threshold = realPar[0];

 if (!ComediDev[index]) {
    DO->dev = comedi_open(block->str);
    if (!DO->dev) {
      fprintf(stdout, "Comedi open failed\n");
      exit(-1);
    }
    
    board = comedi_get_board_name(DO->dev);
    printf("COMEDI %s (%s) opened.\n\n", block->str, board);
    ComediDev[index] = DO->dev;
    
    if ((DO->subdev = comedi_find_subdevice_by_type(DO->dev, COMEDI_SUBD_DO, 0)) >= 0){
      subdev_type = COMEDI_SUBD_DO;
    }
    if(DO->subdev == -1){
      if ((DO->subdev = comedi_find_subdevice_by_type(DO->dev, COMEDI_SUBD_DIO, 0)) < 0) {
	fprintf(stdout, "Comedi find_subdevice failed (No digital Output)\n");
	comedi_close(DO->dev);
	exit(-1);
      }
      else{
	subdev_type = COMEDI_SUBD_DIO;
      }  
    }  
    if ((comedi_lock(DO->dev, DO->subdev)) < 0) {
      fprintf(stdout, "Comedi lock failed for subdevice %d\n", DO->subdev);
      comedi_close(DO->dev);
      exit(-1);
    }
  }
  else {
    DO->dev = ComediDev[index];
    if((DO->subdev = comedi_find_subdevice_by_type(DO->dev, COMEDI_SUBD_DO, 0)) < 0){
      DO->subdev = comedi_find_subdevice_by_type(DO->dev, COMEDI_SUBD_DIO, 0);
      subdev_type =COMEDI_SUBD_DIO;
    }else subdev_type =COMEDI_SUBD_DO; 
  }
  
  if ((n_channels = comedi_get_n_channels(DO->dev, DO->subdev)) < 0) {
    fprintf(stdout, "Comedi get_n_channels failed for subdev %d\n", DO->subdev);
    comedi_unlock(DO->dev, DO->subdev);
    comedi_close(DO->dev);
    exit(-1);
  }
  
  if (DO->channel >= n_channels) {
    fprintf(stdout, "Comedi channel not available for subdevice %d\n", DO->subdev);
    comedi_unlock(DO->dev, DO->subdev);
    comedi_close(DO->dev);
    exit(-1);
  }
	
  if(subdev_type == COMEDI_SUBD_DIO){
    if ((comedi_dio_config(DO->dev, DO->subdev, DO->channel, COMEDI_OUTPUT)) < 0) {
      fprintf(stdout, "Comedi DIO config failed for subdevice %d\n", DO->subdev);
      comedi_unlock(DO->dev, DO->subdev);
      comedi_close(DO->dev);
      exit(-1);
    }
  }
  
  ComediDev_InUse[index]++;
  ComediDev_DIOInUse[index]++;
  comedi_dio_write(DO->dev, DO->subdev, DO->channel, 0);
}

static void inout(python_block *block)
{
  
  double *u = block->u[0];
  unsigned int bit = 0;
  ComediDigOut *DO = block->ptrPar;
  
  if(u[0]>=DO->threshold){
    bit = 1;
  }
  comedi_dio_write(DO->dev, DO->subdev, DO->channel, bit);
}

static void end(python_block *block)
{
  ComediDigOut *DO = block->ptrPar;
  int index = block->str[11]-'0';
  
  comedi_dio_write(DO->dev, DO->subdev, DO->channel, 0);
  ComediDev_InUse[index]--;
  ComediDev_DIOInUse[index]--;
  if (!ComediDev_DIOInUse[index]) {
    comedi_unlock(DO->dev, DO->subdev);
  }
  if (!ComediDev_InUse[index]) {
    comedi_close(DO->dev);
    printf("\nCOMEDI %s closed.\n\n", block->str);
    ComediDev[index] = NULL;
  }
}

void comedi_digital_output(int flag, python_block *block)
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


