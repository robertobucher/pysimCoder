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
#include <stdio.h>
#include <stdlib.h>

/****************************************************************************
 * Name: init
 *
 * Description:
 *   Not used.
 *
 ****************************************************************************/

static void init(python_block *block)
{
}

/****************************************************************************
 * Name: inout
 *
 * Description:
 *   Compute the output signal.
 *
 ****************************************************************************/

static void inout(python_block *block)
{

  double *u = block->u[0];
  double *realPar = block->realPar;
  double *y = block->y[0];

  double error = u[0];
  double Kp = realPar[0];
  double Ki = realPar[1];
  double Kd = realPar[2];
  double min_val = realPar[3];
  double max_val = realPar[4];

  double error_last = realPar[5];
  double integral_sum = realPar[6];

  /* Set integral sum */

  if (Ki == 0)
    {
      integral_sum = 0;
    }
  else
    {
      integral_sum += error * Ki;
    }

  /* Compute the output value */

  double action = Kp * error + integral_sum + Kd * (error - error_last);
  error_last = error;

  /* Anti-windup protection */

  if (action > max_val)
    {
      integral_sum = integral_sum - (action - max_val);
      action = max_val;
    }
  else if (action < min_val)
    {
      integral_sum = integral_sum - (action - min_val);
      action = min_val;
    }

  realPar[5] = error_last;
  realPar[6] = integral_sum;
  y[0] = action;
}

/****************************************************************************
 * Name: end
 *
 * Description:
 *   Not used.
 *
 ****************************************************************************/

static void end(python_block *block)
{
}

/****************************************************************************
 * Name: discretePID
 *
 * Description:
 *   Call needed function based on input flag.
 *
 ****************************************************************************/

void discretePID(int flag, python_block *block)
{
  if (flag==CG_OUT){          /* get input */
    inout(block);
  }
  else if (flag==CG_END){     /* termination */
    end(block);
  }
  else if (flag ==CG_INIT){   /* initialisation */
    init(block);
  }
}
