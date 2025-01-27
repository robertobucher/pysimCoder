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
#include <matop.h>

static void init(python_block *block)
{
  double * gain = block->realPar;
  double *y;
  double *u;

  int i;
  int nin = block->nin;
  int nout = block->nout;
  double tmpY[nout];
  double tmpU[nin];

  for (i=0; i<nin; i++) {
    u = block->u[i];
    tmpU[i]=u[0];
  }
  matmult(gain,nout,nin,tmpU,nin,1,tmpY);
  for(i=0; i<nout; i++){
    y = block->y[i];
    y[0] = tmpY[i];
  }
}

static void inout(python_block *block)
{
  double * gain = block->realPar;
  double *y;
  double *u;

  int i;
  int nin = block->nin;
  int nout = block->nout;
  double tmpY[nout];
  double tmpU[nin];

  for (i=0; i<nin; i++) {
    u = block->u[i];
    tmpU[i]=u[0];
  }
  matmult(gain,nout,nin,tmpU,nin,1,tmpY);
  for(i=0; i<nout; i++){
    y = block->y[i];
    y[0] = tmpY[i];  
  }
}

static void end(python_block *block)
{
  double * gain = block->realPar;
  double *y;
  double *u;

  int i;
  int nin = block->nin;
  int nout = block->nout;
  double tmpY[nout];
  double tmpU[nin];

  for (i=0; i<nin; i++) {
    u = block->u[i];
    tmpU[i]=u[0];
  }
  matmult(gain,nout,nin,tmpU,nin,1,tmpY);
  for(i=0; i<nout; i++){
    y = block->y[i];
    y[0] = tmpY[i];
  }
}
  
void mxmult(int flag, python_block *block)
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


