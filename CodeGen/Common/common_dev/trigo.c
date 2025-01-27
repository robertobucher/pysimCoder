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

#include <pyblock.h>
#include <math.h>

static void init(python_block *block)
{
  int * intPar    = block->intPar;
  double *y = block->y[0];
  double *u = block->u[0];

  switch(intPar[0]){
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
}

static void inout(python_block *block)
{
  int * intPar    = block->intPar;
  double *y = block->y[0];
  double *u = block->u[0];
  
  switch(intPar[0]){
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
}

static void end(python_block *block)
{
  int * intPar    = block->intPar;
  double *y = block->y[0];
  double *u = block->u[0];

  switch(intPar[0]){
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
}

void trigo(int flag, python_block *block)
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


