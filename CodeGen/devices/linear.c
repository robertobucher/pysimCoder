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
#include <matop.h>

void css(int Flag, python_block *block)
{
  double *a, *b, *c, *d, *X;
  int nx, ni, no;
  double * realPar = block->realPar;
  int * intPar    = block->intPar;
  int iA, iB, iC, iD, iX;
  int i;
  double *y;

  ni = intPar[1];
  no = intPar[2];
  nx = intPar[0];

  double h = realPar[0];
  double tmpAX[nx];
  double tmpBU[nx];
  double tmpCX[no];
  double tmpDU[no];
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

  switch(Flag){
  case OUT:
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
    break;
  case STUPD:
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
    break;
  case INIT:
  case END:
    break;
  default:
    break;
  }
}
  
void integral(int Flag, python_block *block)
{
  double * realPar = block->realPar;
  int * intPar    = block->intPar;
  int i;
  double *y;

  double h = realPar[0];
  double *U = block->u[0];

  switch(Flag){
  case OUT:
    y = (double *) block->y[0];
    y[0] = realPar[1];
    break;

  case STUPD:
    /* Runga Kutta */
    realPar[1] = realPar[1] + U[0]*h;
    break;
  case INIT:
  case END:
    break;
  default:
    break;
  }
}
  
void dss(int Flag, python_block *block)
{
  double *a, *b, *c, *d, *X;
  int nx, ni, no;
  double * realPar = block->realPar;
  int * intPar    = block->intPar;
  int iA, iB, iC, iD, iX;
  int i;
  double *y;

  ni = intPar[1];
  no = intPar[2];
  nx = intPar[0];

  double tmpAX[nx];
  double tmpBU[nx];
  double tmpCX[no];
  double tmpDU[no];
  double tmpY[no];

  double tmpU[ni];
  double *U;
  for (i=0;i<ni;i++) {
    U = (double *) block->u[i];
    tmpU[i] = U[0];
  }

  switch(Flag){
  case OUT:
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
    break;
  case STUPD:
    iA = intPar[3];
    iB = intPar[4];
    iX = intPar[7];
    a = &realPar[iA];
    b = &realPar[iB];
    X = &realPar[iX];
    matmult(a,nx,nx,X,nx,1,tmpAX);
    matmult(b,nx,ni,tmpU,ni,1,tmpBU);
    matsum(tmpAX,nx,1,tmpBU,nx,1,X);
    break;
  case INIT:
  case END:
    break;
  default:
    break;
  }
}
  
void unitDelay(int Flag, python_block *block)
{
  double * realPar = block->realPar;
  double *y = block->y[0];
  double *u = block->u[0];

  switch(Flag){
  case OUT:
    y[0] = realPar[0];
    break;

  case STUPD:
    realPar[0] = u[0];
    break;
  case INIT:
  case END:
    break;
  default:
    break;
  }
}
  
void mxmult(int Flag, python_block *block)
{

    /* blk = RCPblk('mxmult',pin,pout,0,realPar,[n,m]) */

  int i;
  double *y;
  double *u;

  int nin = block->intPar[1];
  int nout = block->intPar[0];
  double tmpY[nout];
  double tmpU[nin];

  double * gain = block->realPar;

  switch(Flag){
  case OUT:
  case INIT:
  case END:
    for (i=0; i<nin; i++) {
      u = block->u[i];
      tmpU[i]=u[0];
    }
    matmult(gain,nout,nin,tmpU,nin,1,tmpY);
    for(i=0; i<nout; i++){
      y = block->y[i];
      y[0] = tmpY[i];
    }
    break;
  default:
    break;
  }
}
  
void sum(int Flag, python_block *block)
{
  int i;
  double *y;
  double *u;
  
  y = (double *) block->y[0];

  switch(Flag){
  case OUT:
  case INIT:
  case END:
    y[0] = 0.0;
    for (i=0;i<block->nin;i++){
      u = (double *) block->u[i];
      y[0] += block->realPar[i]*u[0];
    }
    break;
  default:
    break;
  }
}
  
