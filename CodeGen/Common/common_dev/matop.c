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

#include <stdio.h>
#include <pyblock.h>

int matmult(double *a, int na, int ma, double *b, int nb, int mb, double* c)
{
  int i, j, k;

  if (ma!=nb){
    fprintf(stderr,"MATMULT: A and B matrices: Incompatible sizes\n");
    return -1;
  }

  for(i=0;i<na;i++){
    for(j=0;j<mb;j++){
      c[i*mb+j] = 0;
      for(k=0;k<ma;k++)
	c[i*mb+j] += a[i*ma+k]*b[k*mb+j];
    }
  }
  return 0;
}

int matsum(double *a, int na, int ma, double *b, int nb, int mb, double* c)
{
  int i;

  if ((na != nb) || (ma != mb)){
    fprintf(stderr,"MATSUM: A and B matrices: Incompatible sizes\n");
    return -1;
  }

  for(i=0; i<na*ma; i++) c[i] = a[i]+b[i];
  return 0;
}

int integralFunc(double t, const double y[], double f[], void *params)
{
  python_block * block = ( python_block *) params;
 
  double * u = block->u[0];
  double * realPar = block->realPar;

  f[0] = u[0];
  realPar[1] = y[0];

  return 0;
}

int cssFunc(double t, const double y[], double f[], void *params)
{
  python_block * block = ( python_block *) params;
  double * realPar = block->realPar;
  int * intPar    = block->intPar;
 
  double *a, *b, *X;
  int nx, ni, no;
  int iA, iB, iC, iD, iX;
  int i;

  ni = intPar[1];
  no = intPar[2];
  nx = intPar[0];

  double tmpAX[nx];
  double tmpBU[nx];
  double tmpY[no];

  double tmpX[nx];
  
  double tmpU[ni];
  double *u;
  for (i=0;i<ni;i++) {
    u = (double *) block->u[i];
    tmpU[i] = u[0];
  }
  
  iA = intPar[3];
  iB = intPar[4];
  iX = intPar[7];
  a = &realPar[iA];
  b = &realPar[iB];
  X = &realPar[iX];

  for(i=0;i<nx;i++) tmpX[i] = X[i];
  matmult(a,nx,nx,tmpX,nx,1,tmpAX);
  matmult(b,nx,ni,tmpU,ni,1,tmpBU);
  matsum(tmpAX,nx,1,tmpBU,nx,1,tmpX);
  
  for(i=0;i<nx;i++) {
    f[i] = tmpX[i];
    realPar[iX+ i] = y[i];
  }

  return 0;
}
