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
 *   Set the output values based on input 2.
 *
 ****************************************************************************/

static void inout(python_block *block)
{
  double *y1 = block->y[0];
  double *y2 = block->y[1];

  double *u = block->u[0];
  double *change = block->u[1];

  if (change[0] >= block->realPar[0])
    {
      y1[0] = 0;
      y2[0] = u[0];
    }
  else
    {
      y1[0] = u[0];
      y2[0] = 0;
    }
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
 * Name: switch_output
 *
 * Description:
 *   Call needed function based on input flag.
 *
 ****************************************************************************/

void switch_output(int flag, python_block *block)
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
