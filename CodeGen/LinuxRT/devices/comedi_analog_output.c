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
extern int ComediDev_AOInUse[];

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
} ComediAnOut ;


static void init(python_block *block)
{
  int * intPar    = block->intPar;
  ComediAnOut *AO;
  int index = block->str[11]-'0';
  lsampl_t data;
  int n_channels;
  const char * board;
  double u, s;

  block->ptrPar = malloc(sizeof(ComediAnOut));
  AO = block->ptrPar;
  AO->channel = intPar[0];
  AO->range = intPar[1];
  AO->aref = intPar[2];
  
  if (!ComediDev[index]) {
    AO->dev = comedi_open(block->str);
    if (!AO->dev) {
      fprintf(stdout, "Comedi open failed\n");
      exit(-1);
    }
    board = comedi_get_board_name(AO->dev);
    printf("COMEDI %s (%s) opened.\n\n", block->str, board);
    ComediDev[index] = AO->dev;
    
    if ((AO->subdev = comedi_find_subdevice_by_type(AO->dev, COMEDI_SUBD_AO, 0)) < 0) {
      fprintf(stdout, "Comedi find_subdevice failed (No analog output)\n");
      comedi_close(AO->dev);
      exit(-1);
    }
    
    if ((comedi_lock(AO->dev, AO->subdev)) < 0) {
      fprintf(stdout, "Comedi lock failed for subdevice %d\n",AO-> subdev);
      comedi_close(AO->dev);
      exit(-1);
    }
  }
  else {
    AO->dev = ComediDev[index];
    AO->subdev = comedi_find_subdevice_by_type(AO->dev, COMEDI_SUBD_AO, 0);
  }
  
  if ((n_channels = comedi_get_n_channels(AO->dev, AO->subdev)) < 0) {
    fprintf(stdout, "Comedi get_n_channels failed for subdevice %d\n", AO->subdev);
    comedi_unlock(AO->dev, AO->subdev);
    comedi_close(AO->dev);
    exit(-1);
  }
  
  if (AO->channel >= n_channels) {
    fprintf(stdout, "Comedi channel not available for subdevice %d\n", AO->subdev);
    comedi_unlock(AO->dev, AO->subdev);
    comedi_close(AO->dev);
    exit(1);
  }
  
  AO->maxdata = comedi_get_maxdata(AO->dev, AO->subdev, AO->channel);
  AO->lnx_range = comedi_get_range(AO->dev, AO->subdev, AO->channel, AO->range);
  if (AO->lnx_range->min >= AO->lnx_range->max) {
    fprintf(stdout, "Comedi get range failed for subdevice %d\n", AO->subdev);
    comedi_unlock(AO->dev, AO->subdev);
    comedi_close(AO->dev);
    exit(-1);
  }
  ComediDev_InUse[index]++;
  ComediDev_AOInUse[index]++;
  AO->range_min = (double)(AO->lnx_range->min);
  AO->range_max = (double)(AO->lnx_range->max);
  printf("AO Channel %d - Range : %1.2f [V] - %1.2f [V]\n", AO->channel,
	 AO->range_min, AO->range_max);

  u = 0.;
  s = (u - AO->range_min)/(AO->range_max - AO->range_min)*AO->maxdata;
  data = (lsampl_t)(floor(s+0.5));
  /* data = comedi_from_phys(u, AOdata->lnx_range, AOdata->maxdata)  */
  comedi_data_write(AO->dev, AO->subdev, AO->channel, AO->range, AO->aref, data);
}

static void inout(python_block *block)
{
  double *u = block->u[0];
  ComediAnOut *AO  = block->ptrPar;
  lsampl_t data;
  double s;

  s = (u[0] - AO->range_min)/(AO->range_max - AO->range_min)*AO->maxdata;
  if (s < 0) {
    data = 0;
  } else if (s > AO->maxdata) {
    data = AO->maxdata;
  } else {
    data = (lsampl_t)(floor(s+0.5));
  }
  comedi_data_write(AO->dev, AO->subdev, AO->channel, AO->range, AO->aref, data);
}

static void end(python_block *block)
{
  int index  = block->str[11]-'0';
  ComediAnOut *AO  = block->ptrPar;
  lsampl_t data;
  double u, s; 
  
  u = 0.;
  s = (u - AO->range_min)/(AO->range_max - AO->range_min)*AO->maxdata;
  data = (lsampl_t)(floor(s+0.5));
  comedi_data_write(AO->dev, AO->subdev, AO->channel, AO->range, AO->aref, data);

  ComediDev_InUse[index]--;
  ComediDev_AOInUse[index]--;
  if (!ComediDev_AOInUse[index]) {
    comedi_unlock(AO->dev, AO->subdev);
  }
  if (!ComediDev_InUse[index]) {
    comedi_close(AO->dev);
    printf("\nCOMEDI %s closed.\n\n", block->str);
    ComediDev[index] = NULL;
  }
}

void comedi_analog_output(int flag, python_block *block)
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


