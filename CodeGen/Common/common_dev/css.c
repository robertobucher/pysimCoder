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
}

static void inout(python_block *block)
{
  double *c, *d, *X;
  int nx, ni, no;
  double * realPar = block->realPar;
  int * intPar    = block->intPar;
  int iC, iD, iX;
  int i;
  double *y;

  ni = intPar[1];
  no = intPar[2];
  nx = intPar[0];

  double tmpCX[no];
  double tmpDU[no];
  double tmpY[no];

  double tmpU[ni];
  double *U;
  for (i=0;i<ni;i++) {
    U = (double *) block->u[i];
    tmpU[i] = U[0];
  }
  iC = intPar[5];
  iD = intPar[6];
  iX = intPar[7];
  c = &realPar[iC];
  d = &realPar[iD];
  X = &realPar[iX];
  matmult(c,no,nx,X,nx,1,tmpCX);
  matmult(d,no,ni,tmpU,ni,1,tmpDU);
  matsum(tmpCX,no,1,tmpDU,no,1,tmpY);
    
  for(i=0;i<no;i++){
    y = (double *) block->y[i];
    y[0] = tmpY[i];
  } 
}

static void update(python_block *block)
{
  double *a, *b, *X;
  int nx, ni, no;
  double * realPar = block->realPar;
  int * intPar    = block->intPar;
  int iA, iB, iC, iD, iX;
  int i;

  ni = intPar[1];
  no = intPar[2];
  nx = intPar[0];

  double h = realPar[0];
  double tmpAX[nx];
  double tmpBU[nx];
  double tmpY[no];

  double tmpX[nx];
  double tmpX1[nx];
  double tmpX2[nx];
  double tmpX3[nx];
  double tmpX4[nx];

  double tmpU[ni];
  double *U;
  for (i=0;i<ni;i++) {
    U = (double *) block->u[i];
    tmpU[i] = U[0];
  }
  iA = intPar[3];
  iB = intPar[4];
  iX = intPar[7];
  a = &realPar[iA];
  b = &realPar[iB];
  X = &realPar[iX];

  /* Runga Kutta */
  /* 1 */
  for(i=0;i<nx;i++) tmpX[i] = X[i];
  matmult(a,nx,nx,tmpX,nx,1,tmpAX);
  matmult(b,nx,ni,tmpU,ni,1,tmpBU);
  matsum(tmpAX,nx,1,tmpBU,nx,1,tmpX1);
  for(i=0;i<nx;i++) tmpX1[i] = h*tmpX1[i];

  /* 2 */
  for(i=0;i<nx;i++) tmpX[i] = X[i]+0.5*tmpX1[i];
  matmult(a,nx,nx,tmpX,nx,1,tmpAX);
  matmult(b,nx,ni,tmpU,ni,1,tmpBU);
  matsum(tmpAX,nx,1,tmpBU,nx,1,tmpX2);
  for(i=0;i<nx;i++) tmpX2[i] = h*tmpX2[i];
    
  /* 3 */
  for(i=0;i<nx;i++) tmpX[i] = X[i]+0.5*tmpX2[i];
  matmult(a,nx,nx,tmpX,nx,1,tmpAX);
  matmult(b,nx,ni,tmpU,ni,1,tmpBU);
  matsum(tmpAX,nx,1,tmpBU,nx,1,tmpX3);
  for(i=0;i<nx;i++) tmpX3[i] = h*tmpX3[i];
    
  /* 4 */
  for(i=0;i<nx;i++) tmpX[i] = X[i]+tmpX3[i];
  matmult(a,nx,nx,tmpX,nx,1,tmpAX);
  matmult(b,nx,ni,tmpU,ni,1,tmpBU);
  matsum(tmpAX,nx,1,tmpBU,nx,1,tmpX4);
  for(i=0;i<nx;i++) tmpX4[i] = h*tmpX4[i];
    
  for(i=0;i<nx;i++) X[i] = X[i] + tmpX1[i]/6 + tmpX2[i]/3 + tmpX3[i]/3 + tmpX4[i]/6;  
}

static void end(python_block *block)
{
}

void css(int flag, python_block *block)
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


