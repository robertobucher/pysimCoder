/*
COPYRIGHT (C) 2024  Roberto Bucher (roberto.bucher@supsi.ch)

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

void minFromNInputs(int Flag, python_block *block)
{
  int i;
  double *y = (double *) block->y[0];
  double **u = (double **) block->u;

  /* Initialize the minimum */

  double min = (double) ((double *) block->u[0])[0];

  switch (Flag) {
  case CG_INIT:
  case CG_OUT:
  case CG_END:
  case CG_STUPD:
    for (i = 0; i < block->nin; ++i) {
      double num = u[i][0];
      if (num < min) {
        min = num;
      }
    }
  default:
    break;
  }

  y[0] = min;
}

void maxFromNInputs(int Flag, python_block *block)
{
  int i;
  double *y = (double *) block->y[0];
  double **u = (double **) block->u;

  /* Initialize the maximum */

  double max = (double) ((double *) block->u[0])[0];

  switch (Flag) {
  case CG_INIT:
  case CG_OUT:
  case CG_END:
    for (i = 0; i < block->nin; ++i) {
      double num = u[i][0];
      if (num > max) {
        max = num;
      }
    }
  default:
    break;
  }

  y[0] = max;
}
