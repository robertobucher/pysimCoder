/*
COPYRIGHT (C) 2024 Michal Lenc (michallenc@seznam.cz)

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

#include <math.h>
#include <pyblock.h>
#include <stdio.h>
#include <fenv.h>
#include <stdlib.h>

static void init(python_block *block) {}

static void inout(python_block *block) {
  double *y = block->y[0];  /* Block output z = x % y */
  double *u1 = block->u[0]; /* Input x */
  double *u2 = block->u[1]; /* Input y */

  /* Everythink is double, so we can just call fmod */

  if (!isinf(u1[0]) && (u2[0] != 0)) {
    y[0] = fmod(u1[0], u2[0]);
  }
}

static void end(python_block *block) {}

void pysim_modulo(int flag, python_block *block) {
  if (flag == CG_OUT) {
    /* input/output function */
    inout(block);
  } else if (flag == CG_END) {
    /* termination */
    end(block);
  } else if (flag == CG_INIT) {
    /* block initialization*/
    init(block);
  }
}
