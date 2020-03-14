/*
  COPYRIGHT (C) 2009  Roberto Bucher (roberto.bucher@supsi.ch)

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
#include <stdlib.h>

double get_run_time();

static void init(python_block *block)
{
  int * intPar    = block->intPar;

  FILE *fp;
  double *pData;
  double val;
  
  int ch = intPar[0];
  int npts = intPar[1];
  int i, j;

  pData = (double *) calloc(ch*npts, sizeof(double));
  
  fp=fopen(block->str,"r");
  if(fp!=NULL){
    block->intPar[2]=0;
    for(i=0;i<npts;i++) {
      if(feof(fp)) break;
      for(j=0;j<ch;j++){
	fscanf(fp,"%lf",&val);
	pData[j*npts+i] = val;
      }
    }
    fclose(fp);
    block->ptrPar = pData;
  }
  else exit(1);
}

static void inout(python_block *block)
{
  int * intPar    = block->intPar;
  double *y;
  int i, n;
  int npts;

  double *pData = block->ptrPar;
  n = intPar[2];
  npts = intPar[1];
  
  for(i=0;i<intPar[0]; i++){
    y = block->y[i];
    y[0] = pData[i*npts+block->intPar[2]];
  }
  block->intPar[2] = ++(block->intPar[2]) % block->intPar[1];
}

static void update(python_block *block)
{
}

static void end(python_block *block)
{
  int * intPar    = block->intPar;
  double *y;
  int i;

  for(i=0;i<intPar[0];i++){
    y = block->y[i];
    y[0] = 0.0;
  }
  free(block->ptrPar);  
}

void extdata(int flag, python_block *block)
{
  if (flag==CG_OUT){          /* get input */
    inout(block);
  }
  else if (flag == CG_STUPD){
    update(block);
  }
  else if (flag==CG_END){     /* termination */ 
    end(block);
  }
  else if (flag ==CG_INIT){    /* initialisation */
    init(block);
  }
}


