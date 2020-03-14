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

void absV(int Flag, python_block *block)
{
  int i;
  double *y;
  double *u;  

  switch(Flag){
  case CG_OUT:
  case CG_INIT:
  case CG_END:
    for(i=0;i<block->nin;i++){
      u = (double *) block->u[i];
      y = (double *) block->y[i];
      y[0] = fabs(u[0]);
    }
    break;
  default:
    break;
  }
}
  
void saturation(int Flag, python_block *block)
{
  double out;
  double satP = block->realPar[0];
  double satN = block->realPar[1];
  double * u = (double *) block->u[0];
  double * y = (double *) block->y[0];

  switch(Flag){
  case CG_OUT:
  case CG_INIT:
  case CG_END:
    out = u[0];
    if (out > satP) out = satP;
    if (out < satN) out = satN;
    y[0] = out;
    break;
  default:
    break;
  }
}
  
void prod(int Flag, python_block *block)
{
  int i;
  double *y;
  double *u;
  
  y = (double *) block->y[0];

  switch(Flag){
  case CG_OUT:
  case CG_INIT:
  case CG_END:
    y[0] = 1.0;
    for (i=0;i<block->nin;i++){
      u = (double *) block->u[i];
      y[0] *= u[0];
    }
    break;
  default:
    break;
  }
}
  
void lut(int Flag, python_block *block)
{
  int i;
  double *y = block->y[0];
  double *u = block->u[0];  

  switch(Flag){
  case CG_INIT:
  case CG_END:
    break;
  case CG_OUT:
    y[0] = block->realPar[0];
    for(i=1;i<block->intPar[0];i++){
      y[0] = y[0]*u[0] + block->realPar[i];
    }
    break;
  default:
    break;
  }
}
  
void deadzone(int Flag, python_block *block)
{
  double start = block->realPar[0];
  double end = block->realPar[1];
  double * u = (double *) block->u[0];
  double * y = (double *) block->y[0];

  switch(Flag){
  case CG_OUT:
  case CG_INIT:
  case CG_END:
    if ((u[0] >= start) && (u[0] <= end)) y[0] = 0.0;
    else y[0] = u[0];
    break;
  default:
    break;
  }
}
  
void trigo(int Flag, python_block *block)
{
  int i;
  double *y;
  double *u;  

  switch(Flag){
  case CG_OUT:
  case CG_INIT:
  case CG_END:
    u = (double *) block->u[0];
    y = (double *) block->y[0];
    switch(block->intPar[0]){
    case 1:
      y[0] = sin(u[0]);
      break;
    case 2:
      y[0] = cos(u[0]);
      break;
    case 3:
      y[0] = tan(u[0]);
      break;
    default:
      y[0]=u[0];
      break;
    }
  default:
    break;
  }
}
  
