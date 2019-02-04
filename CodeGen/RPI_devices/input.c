/*
COPYRIGHT (C) 2016  Roberto Bucher (roberto.bucher@supsi.ch)

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
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

double get_run_time();

void step(int Flag, python_block *block)
{
  double t;
  double initTime = block->realPar[0];
  double Val = block->realPar[1];
  double * y;

  y = (double *) block->y[0];
  switch(Flag){
  case OUT:
    t = get_run_time();
    if(t<initTime) y[0] = 0.0;
    else           y[0] = Val;
    break;
  case INIT:
    y[0]=0.0;
    break;
  case END:
    y[0]=0.0;
    break;
  default:
    break;
  }
}
  
void sinus(int Flag, python_block *block)
{
  double t;
  double *y = (double *) block->y[0];

  double w,pi=3.1415927;
  double Amp = block->realPar[0];
  double Freq = block->realPar[1];
  double Phase = block->realPar[2];
  double Bias = block->realPar[3];
  double Delay = block->realPar[4];

  switch(Flag){
  case OUT:
    t = get_run_time();
    if(t<Delay) y[0] = 0.0;
    else {
      w=2*pi*Freq*(t-Delay)-Phase;
      y[0]=Amp*sin(w)+Bias;
    }
    break;
  case INIT:
    y[0]=0.0;
    break;
  case END:
    y[0]=0.0;
    break;
  default:
    break;
  }
}

void square(int Flag, python_block *block)
{
  double t;
  double *y = (double *) block->y[0];

  double Amp = block->realPar[0];
  double Period = block->realPar[1];
  double Width = block->realPar[2];
  double Bias = block->realPar[3];
  double Delay = block->realPar[4];

  double v;

  switch(Flag){
  case OUT:
    t = get_run_time();
    if(t<Delay) y[0]=0.0;
    else{
      v = (t-Delay)/Period;
      v = (v-(int) v)*Period;
      if (v<Width) y[0] = Amp+Bias;
      else         y[0] = Bias;
    }      
    break;
  case INIT:
    y[0]=0.0;
    break;
  case END:
    y[0]=0.0;
    break;
  default:
    break;
  }
}

void constant(int Flag, python_block *block)
{
  double *y;
  
  y = (double *) block->y[0];

  switch(Flag){
  case OUT:
  case INIT:
  case END:
    y[0] = block->realPar[0];
    break;
  default:
    break;
  }
}
  
void extdata(int Flag, python_block *block)
{
  FILE * fp;
  double * pData;
  int i;
  double *y;
  
  int npts=block->intPar[0];
  y = (double *) block->y[0];

  switch(Flag){
  case OUT:
    pData = block->ptrPar;
    y[0]=pData[block->intPar[1]];
    block->intPar[1] = ++(block->intPar[1]) % block->intPar[0];
    break;
  case INIT:
    pData=(double *) calloc(npts,sizeof(double));
    fp=fopen(block->str,"r");
    if(fp!=NULL){     
      block->intPar[1]=0;
      for(i=0;i<npts;i++) {
	if(feof(fp)) break;
	fscanf(fp,"%lf",&(pData[i]));
      }
      fclose(fp);
      block->ptrPar = pData;
    }
    else{
      fprintf(stderr, "File %s not found!\n",block->str);
      exit(1);
    }
    break;
  case END:
    y[0] = 0.0;
    free(block->ptrPar);
    break;
  default:
    break;
  }
}
  
