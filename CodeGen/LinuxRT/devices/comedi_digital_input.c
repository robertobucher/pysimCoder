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
} ComediDigIn ;

static void init(python_block *block)
{
  int * intPar    = block->intPar;
  int subdev_type = -1;
  int index = block->str[11]-'0';
  int n_channels;
  const char * board;
  ComediDigIn *DI;

  block->ptrPar = malloc(sizeof(ComediDigIn));
  if (block->ptrPar  == NULL ) {
    fprintf(stdout, "Error by malloc\n"); 
    exit(-1); 
  }
  DI = block->ptrPar;
  DI->channel = intPar[0];

  if (!ComediDev[index]) {
    DI->dev = comedi_open(block->str);
    if (!DI->dev) {
      fprintf(stdout, "Comedi open failed\n");
      exit(-1);
    }
    board = comedi_get_board_name(DI->dev);
    printf("COMEDI %s (%s) opened.\n\n", block->str, board);
    ComediDev[index] = DI->dev;

    if ((DI->subdev = comedi_find_subdevice_by_type(DI->dev, COMEDI_SUBD_DI, 0)) < 0) {
      fprintf(stdout, "Comedi find_subdevice failed (No digital input)\n");
     }
    else {
      subdev_type = COMEDI_SUBD_DI;
    }  
    if(DI->subdev == -1){
      if ((DI->subdev = comedi_find_subdevice_by_type(DI->dev, COMEDI_SUBD_DIO, 0)) < 0) {
	fprintf(stdout, "Comedi find_subdevice failed (No digital I/O)\n");
	comedi_close(DI->dev);
	exit(-1);
      }
      else{
	subdev_type = COMEDI_SUBD_DIO;
      }  
    }
    
    if ((comedi_lock(DI->dev, DI->subdev)) < 0) {
      fprintf(stdout, "Comedi lock failed for subdevice %d\n", DI->subdev);
      comedi_close(DI->dev);
      exit(-1);
    }
  }
  else {
    DI->dev = ComediDev[index];
    if((DI->subdev = comedi_find_subdevice_by_type(DI->dev, COMEDI_SUBD_DI, 0)) < 0){
      DI->subdev = comedi_find_subdevice_by_type(DI->dev, COMEDI_SUBD_DIO, 0);
      subdev_type =COMEDI_SUBD_DIO;
    }else subdev_type =COMEDI_SUBD_DI; 
  }
   
  if ((n_channels = comedi_get_n_channels(DI->dev, DI->subdev)) < 0) {
    fprintf(stdout, "Comedi get_n_channels failed for subdev %d\n", DI->subdev);
    comedi_unlock(DI->dev, DI->subdev);
    comedi_close(DI->dev);
    exit(-1);
  }

  if (DI->channel >= n_channels) {
    fprintf(stdout, "Comedi channel not available for subdevice %d\n", DI->subdev);
    comedi_unlock(DI->dev, DI->subdev);
    comedi_close(DI->dev);
    exit(-1);
  }

  if(subdev_type == COMEDI_SUBD_DIO){
    if ((comedi_dio_config(DI->dev, DI->subdev, DI->channel, COMEDI_INPUT)) < 0) {
      fprintf(stdout, "Comedi DIO config failed for subdevice %d\n", DI->subdev);
      comedi_unlock(DI->dev, DI->subdev);
      comedi_close(DI->dev);
      exit(-1);
    }	
  }	
  ComediDev_InUse[index]++;
  ComediDev_DIOInUse[index]++;
}

static void inout(python_block *block)
{
  double *y = block->y[0];
  ComediDigIn *DI  = block->ptrPar;
  unsigned int bit;

  comedi_dio_read(DI->dev, DI->subdev, DI->channel, &bit);
  y[0] = (double) bit;
}

static void end(python_block *block)
{
  ComediDigIn *DI  = block->ptrPar;
  int index  = block->str[11]-'0';
  
  ComediDev_InUse[index]--;
  ComediDev_DIOInUse[index]--;
  if (!ComediDev_DIOInUse[index]) {
    comedi_unlock(DI->dev, DI->subdev);
  }
  if (!ComediDev_InUse[index]) {
    comedi_close(DI->dev);
    printf("\nCOMEDI %s closed.\n\n", block->str);
    ComediDev[index] = NULL;
  }
}

void comedi_digital_input(int flag, python_block *block)
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


