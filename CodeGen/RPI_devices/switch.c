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
#include <stdio.h>
#include <stdlib.h>

static void init(python_block *block)
{
}

static void inout(python_block *block)
{
  double *y = block->y[0];

  double *u1 = block->u[0];
  double *u2 = block->u[1];
  double *u3 = block->u[2];

  if (block->intPar[1] == 2)
    y[0] = u2[0];
  else{
    switch(block->intPar[0]){
    case 0:
      if(u3[0] < block->realPar[0]){
	y[0] = u2[0];
	if (block->intPar[1]) block->intPar[1]=2;
      }
      else y[0] = u1[0];
      break;

    case 1:
      if(u3[0] >= block->realPar[0]){
	y[0] = u2[0];
	if (block->intPar[1]) block->intPar[1]=2;
      }
      else y[0] = u1[0];
      break;
    }
  }
}

static void end(python_block *block)
{
  /* printf("Init Encoder closed\n"); */
}



void switcher(int flag, python_block *block)
{
  if (flag==OUT){          /* get input */
    inout(block);
  }
  else if (flag==END){     /* termination */ 
    end(block);
  }
  else if (flag ==INIT){    /* initialisation */
    init(block);
  }
}


