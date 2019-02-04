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

