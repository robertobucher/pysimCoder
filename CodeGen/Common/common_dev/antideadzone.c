/*
COPYRIGHT (C) 2025 Stepan Pressl (pressl.stepan@gmail.com)

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

static void init(python_block *block)
{
}

static void inout(python_block *block)
{
  double *y = (double *) block->y[0];
  double *u = (double *) block->u[0];
  double upper = block->realPar[0];
  double lower = block->realPar[1];

  y = (double *) block->y[0];
  if (u[0] >= 0) {
    y[0] = u[0] + upper;
  } else {
    y[0] = u[0] + lower;
  }
}

static void end(python_block *block)
{
}

void antideadzone(int flag, python_block *block)
{
  if (flag == CG_OUT) {
    /* input/output function */
    inout(block);
  } else if (flag == CG_END) {
    /* termination */
    end(block);
  } else if (flag == CG_INIT) {
    /* block initialization */
    init(block);
  }
}
