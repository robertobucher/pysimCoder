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
  double iVal = block->realPar[1];
  double fVal = block->realPar[2];
  double * y;

  y = (double *) block->y[0];
  switch(Flag){
  case CG_OUT:
    t = get_run_time();
    if(t<initTime) y[0] = iVal;
    else           y[0] = fVal;
    break;
  case CG_INIT:
    y[0]=0.0;
    break;
  case CG_END:
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
  case CG_OUT:
    t = get_run_time();
    if(t<Delay) y[0] = 0.0;
    else {
      w=2*pi*Freq*(t-Delay)-Phase;
      y[0]=Amp*sin(w)+Bias;
    }
    break;
  case CG_INIT:
    y[0]=0.0;
    break;
  case CG_END:
    y[0]=0.0;
    break;
  default:
    break;
  }
}

void squareSignal(int Flag, python_block *block)
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
  case CG_OUT:
    t = get_run_time();
    if(t<Delay) y[0]=0.0;
    else{
      v = (t-Delay)/Period;
      v = (v-(int) v)*Period;
      if (v<Width) y[0] = Amp+Bias;
      else         y[0] = Bias;
    }      
    break;
  case CG_INIT:
    y[0]=0.0;
    break;
  case CG_END:
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
  case CG_OUT:
  case CG_INIT:
  case CG_END:
    y[0] = block->realPar[0];
    break;
  default:
    break;
  }
}
