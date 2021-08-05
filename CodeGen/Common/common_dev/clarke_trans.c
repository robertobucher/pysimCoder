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
 * Name: forward_clarke
 *
 * Description:
 *   Computes forward Clarke transformation. It is possible to use both
 *   calculations for 2 and 3 inputs.
 *
 ****************************************************************************/

void forward_clarke(int Flag, python_block *block)
{
  double *alpha = block->y[0];
  double *beta = block->y[1];
  double *cur1 = block->u[0];
  double *cur2 = block->u[1];

  double cur_alp;
  double cur_bet;

  switch(Flag)
    {
      case CG_OUT:
      case CG_INIT:
      case CG_END:
        if (block->nin == 2)
          {
            /* Only two inputs are used */

            cur_alp = cur1[0];
            cur_bet = 2 * cur2[0] + cur1[0];
          }
        else
          {
            /* We read data from all 3 inputs */

            double *cur3 = block->u[2];

            cur_alp = (-cur2[0] - cur3[0] + 2 * cur1[0]) / 3;
            cur_bet = (cur2[0] - cur3[0]) + 2 * cur2[0] + cur1[0] - (2 * cur3[0] + cur1[0]);

            /* Make an average */

            cur_bet = cur_bet / 3;
          }

        cur_bet = cur_bet / sqrt(3);

        /* Save alpha and beta to outputs */

        alpha[0] = cur_alp;
        beta[0] = cur_bet;
        break;
      default:
        break;
    }
}

/****************************************************************************
 * Name: inverse_clarke
 *
 * Description:
 *   Computes inverse Clarke transformation.
 *
 ****************************************************************************/

void inverse_clarke(int Flag, python_block *block)
{
  double *cur1 = block->y[0];
  double *cur2 = block->y[1];
  double *cur3 = block->y[2];
  double *alpha = block->u[0];
  double *beta = block->u[1];

  switch(Flag)
    {
      case CG_OUT:
      case CG_INIT:
      case CG_END:
        cur1[0] = alpha[0];
        cur2[0] = -0.5f * alpha[0] + 0.5f * sqrt(3) * beta[0]; 
        cur3[0] = -0.5f * alpha[0] - 0.5f * sqrt(3) * beta[0]; 
        break;
      default:
        break;
    }
}
