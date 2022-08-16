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
extern int ComediDev_AIInUse[];

//** Comedi static structure inside block->work  
typedef struct
{
  comedi_t *dev;
  int subdev;
  unsigned int channel; 
  unsigned int range; 
  unsigned int aref; 
  lsampl_t maxdata;
  comedi_range *lnx_range;
  double range_min;
  double range_max;
} ComediAnIn ;

static void init(python_block *block)
{
  ComediAnIn *AI ; //** define the local datastructure 
  int * intPar    = block->intPar;
  int index = block->str[11]-'0';
  int n_channels;
  const char * board;
  
  block->ptrPar = malloc(sizeof(ComediAnIn));
  if (block->ptrPar  == NULL ) {
    fprintf(stdout, "Error by malloc\n"); 
    exit(-1); 
  }
  AI = block->ptrPar;
  AI->channel = intPar[0];
  AI->range = intPar[1];
  AI->aref = intPar[2];
  
  if (!ComediDev[index]) {
    AI->dev = comedi_open(block->str);
    if (!AI->dev) {
      fprintf(stdout, "Comedi open failed\n");
      exit(-1);
    }
    board = comedi_get_board_name(AI->dev);
    printf("COMEDI %s (%s) opened.\n\n", block->str, board);
    ComediDev[index] = AI->dev;
    
    if ((AI->subdev = comedi_find_subdevice_by_type(AI->dev, COMEDI_SUBD_AI, 0)) < 0) {
      fprintf(stdout, "Comedi find_subdevice failed (No analog input)\n");
      comedi_close(AI->dev);
      exit(-1);
    }
    
    if ((comedi_lock(AI->dev, AI->subdev)) < 0) {
      fprintf(stdout, "Comedi lock failed for subdevice %d\n", AI->subdev);
      comedi_close(AI->dev);
      exit(-1);
    }
  }
  else {
    AI->dev = ComediDev[index];
    AI->subdev = comedi_find_subdevice_by_type(AI->dev, COMEDI_SUBD_AI, 0);
  }
  
  if ((n_channels = comedi_get_n_channels(AI->dev, AI->subdev)) < 0) {
    fprintf(stdout, "Comedi get_n_channels failed for subdevice %d\n", AI->subdev);
    comedi_unlock(AI->dev, AI->subdev);
    comedi_close(AI->dev);
    exit(-1);
  }
  
  if (AI->channel >= n_channels) {
    fprintf(stdout, "Comedi channel not available for subdevice %d\n", AI->subdev);
    comedi_unlock(AI->dev, AI->subdev);
    comedi_close(AI->dev);
    exit(1);
  }
  
  AI->lnx_range = comedi_get_range(AI->dev, AI->subdev, AI->channel, AI->range);
  if (AI->lnx_range->min >= AI->lnx_range->max) {
    fprintf(stdout, "Comedi get range failed for subdevice %d\n", AI->subdev);
    comedi_unlock(AI->dev, AI->subdev);
    comedi_close(AI->dev);
    exit(-1);
  }
  
  ComediDev_InUse[index]++;
  ComediDev_AIInUse[index]++;
  AI->range_min = (double)( AI->lnx_range->min);
  AI->range_max = (double) (AI->lnx_range->max);
  printf("AI Channel %d - Range : %1.2f [V] - %1.2f [V] (maxdata 0x%lx)\n",
	 AI->channel, AI->range_min, AI->range_max,
	 (unsigned long) comedi_get_maxdata(AI->dev, AI->subdev, AI->channel));
}

static void inout(python_block *block)
{
  int * intPar    = block->intPar;
  double *y = block->y[0];
  ComediAnIn *AI  = block->ptrPar;
  
  lsampl_t data; 
  double x;

  AI->maxdata = comedi_get_maxdata(AI->dev, AI->subdev, AI->channel);
  comedi_data_read(AI->dev, AI->subdev, AI->channel, AI->range, AI->aref, &data);
  x = data;
  x /= AI->maxdata;
  x *= (AI->range_max - AI->range_min);
  x += AI->range_min;
  y[0] = x;
  
}

static void end(python_block *block)
{
  int index  = block->str[11]-'0';
  ComediAnIn *AI  = block->ptrPar;
  
  ComediDev_InUse[index]--;
  ComediDev_AIInUse[index]--;
  if (!ComediDev_AIInUse[index]) {
    comedi_unlock(AI->dev, AI->subdev);
  }
  if (!ComediDev_InUse[index]) {
    comedi_close(AI->dev);
    printf("\nCOMEDI %s closed.\n\n", block->str);
    ComediDev[index] = NULL;
  }
}

void comedi_analog_input(int flag, python_block *block)
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


