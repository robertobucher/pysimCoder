/*
COPYRIGHT (C) 2021  Roberto Bucher (roberto.bucher@supsi.ch)

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
#include <math.h>

/****************************************************************************
 * Name: forward_park
 *
 * Description:
 *   Computes forward Park transformation.
 *
 ****************************************************************************/

void forward_park(int Flag, python_block *block)
{
  double *d = block->y[0];
  double *q = block->y[1];
  double *alpha = block->u[0];
  double *beta = block->u[1];
  double *rot_angle = block->u[2];

  switch(Flag)
    {
      case CG_OUT:
      case CG_INIT:
      case CG_END:
        d[0] = alpha[0] * cos(rot_angle[0]) + beta[0] * sin(rot_angle[0]);
        q[0] = -alpha[0] * sin(rot_angle[0]) + beta[0] * cos(rot_angle[0]);
        break;
      default:
        break;
    }
}

/****************************************************************************
 * Name: inverse_park
 *
 * Description:
 *   Computes inverse Park transformation.
 *
 ****************************************************************************/

void inverse_park(int Flag, python_block *block)
{
  double *alpha = block->y[0];
  double *beta= block->y[1];
  double *d = block->u[0];
  double *q = block->u[1];
  double *rot_angle = block->u[2];

  switch(Flag)
    {
      case CG_OUT:
      case CG_INIT:
      case CG_END:
        alpha[0] = d[0] * cos(rot_angle[0]) - q[0] * sin(rot_angle[0]);
        beta[0] = d[0] * sin(rot_angle[0]) + q[0] * cos(rot_angle[0]);
        break;
      default:
        break;
    }
}
