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

double get_run_time();

void print(int Flag, python_block *block)
{
  int i; 
  double t;
  double *u;

  switch(Flag){
  case OUT:
    t = get_run_time();
    printf("%lf\t",t);
    for(i=0;i<block->nin;i++){
      u = (double *) block->u[i];
      printf("%lf\t",u[0]);
    }
    printf("\n");
    break;
  case INIT:
    break;
  case END:
    break;
  default:
    break;
  }
}
