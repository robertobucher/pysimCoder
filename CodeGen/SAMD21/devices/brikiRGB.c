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
#include <myFun.h>

#define INPUT 1
#define OUTPUT 2

static void init(python_block *block)
{
  initRGB();
}

static void inout(python_block *block)
{
  double *u0 = block->u[0];
  double *u1 = block->u[1];
  double *u2 = block->u[2];
  double *u3 = block->u[3];
  
  double br = u0[0];
  double r = u1[0];
  double g = u2[0];
  double b = u3[0];

  if(br>1) br =1.0;
  if(br<0) br=0.0;
  if(r>1) r =1.0;
  if(r<0) r=0.0;
  if(g>1) g =1.0;
  if(g<0) g=0.0;
  if(b>1) b =1.0;
  if(b<0) b=0.0;
  
  setRGB((int) (100*br), (int) (255*r), (int) (255*g), (int) (255*b),  1);  
}

static void end(python_block *block)
{
}

void brikiRGB(int flag, python_block *block)
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


