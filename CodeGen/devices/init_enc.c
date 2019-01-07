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

double get_run_time();

static void init(python_block *block)
{
}

static void inout(python_block *block)
{
  double *y = block->y[0];
  double *u = block->u[0];

  double t=get_run_time();
  if (t >= block->realPar[0]) 
    y[0] = u[0]-block->realPar[2]-block->realPar[3];
  else {
    block->realPar[3] = u[0];
    y[0] = block->realPar[1];
  }

}

static void end(python_block *block)
{
  /* printf("Init Encoder closed\n"); */
}



void init_enc(int flag, python_block *block)
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


