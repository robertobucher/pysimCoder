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
#include <math.h>

double get_Tsamp();

static void init(python_block *block)
{
  double * realPar = block->realPar;
  double *accX = block->u[0];
  double *accY = block->u[1];
  double *accZ = block->u[2];

  double sign = 1.0;
  if(accZ[0]<0) sign=-1.0;

  realPar[1] = get_Tsamp();
  realPar[2] = atan2f(accX[0], sign*sqrt(accY[0]*accY[0]+accZ[0]*accZ[0]));
}

static void inout(python_block *block)
{
  double * realPar = block->realPar;
  double *y = block->y[0];

  y[0] = realPar[2];
}

static void update(python_block *block)
{
  double * realPar = block->realPar;
  double *accX = block->u[0];
  double *accY = block->u[1];
  double *accZ = block->u[2];
  double *vY = block->u[3];

  double sign = 1.0;
  if(accZ[0]<0) sign=-1.0;
  double acc = atan2f(accX[0], sign*sqrt(accY[0]*accY[0]+accZ[0]*accZ[0]));
  realPar[2] = (realPar[2]+vY[0]*realPar[1])*realPar[0] + acc*(1-realPar[0]);
}

static void end(python_block *block)
{
}

void compFilt(int flag, python_block *block)
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


