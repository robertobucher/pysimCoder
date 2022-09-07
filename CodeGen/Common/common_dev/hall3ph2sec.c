/*
COPYRIGHT (C) 2022  Michal Lenc (michallenc@seznam.cz)

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

static const signed char hall_pos_table[8] =
{
  [0] = -1,
  [7] = -1,
  [1] =  0,    /*0*/
  [5] =  1,    /*1*/
  [4] =  2,    /*2*/
  [6] =  3,    /*3*/
  [2] =  4,    /*4*/
  [3] =  5,    /*5*/
};

static const signed char hall_pos_table_inverted[8] =
{
  [0] = -1,
  [7] = -1,
  [1] =  0,    /*0*/
  [5] =  5,    /*1*/
  [4] =  4,    /*2*/
  [6] =  3,    /*3*/
  [2] =  2,    /*4*/
  [3] =  1,    /*5*/
};

/****************************************************************************
 * Name: hall2sec
 *
 * Description:
 *   Converts signal from 3 phase HALL sensor (used in PMSM for example) to
 *   a sector number.
 *
 ****************************************************************************/

void hall3ph2sec(int Flag, python_block *block)
{
  double *hall_a = block->u[0];
  double *hall_b = block->u[1];
  double *hall_c = block->u[2];
  double *pos = block->y[0];
  double *error = block->y[1];

  int *intPar = block->intPar;
  double mode = intPar[0];
  int pos_new;
  int hall = 0;

  switch(Flag)
    {
      case CG_OUT:
      case CG_INIT:
      case CG_END:
        hall |= hall_a[0] != 0;
        hall |= (hall_b[0] != 0) << 1;
        hall |= (hall_c[0] != 0) << 2;
        if (mode == 0)
          {
            pos_new = hall_pos_table[hall];
          }
        else
          {
            pos_new = hall_pos_table_inverted[hall];
          }

        if (pos_new != -1)
          {
            pos[0] = pos_new;
            error[0] = 0;
          }
        else
          {
            error[0] = 1;
          }
        break;
      default:
        break;
    }
}
