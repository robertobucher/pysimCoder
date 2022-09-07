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
#include <math.h>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795029
#endif

/****************************************************************************
 * Name: pmsm_align
 *
 * Description:
 *  This function computes the electric angle of PMSM. There are four inputs
 *  required:
 *     - Hall sector number
 *     - Encoder position
 *     - Last position of encoder's index
 *     - Index count
 *
 *  The function takes those inputs and determines whether the HALL sensors
 *  or encoder are used for the motor's angle. HALL sensors are used until
 *  the encoder hits its index.
 *
 ****************************************************************************/

void pmsm_align(int Flag, python_block *block)
{
  double *pos_hall = block->u[0];
  double *pos_enc = block->u[1];
  double *pos_indx = block->u[2];
  double *indx_cnt = block->u[3];
  double *angle_out = block->y[0];
  double *align_reset = block->u[4];

  int indx_change = 0;

  double *realPar = block->realPar;
  int *intPar = block->intPar;
  double hall_offset = realPar[0];
  double encoder_res = realPar[1];
  double angle_offset = realPar[2];

  switch(Flag)
    {
      case CG_INIT:
        intPar[0] = 0;
      case CG_OUT:
        if (align_reset[0] == 1)
          {
            intPar[0] = 0;
          }
        if (intPar[1] != indx_cnt[0])
          {
            indx_change = 1;
          }

        intPar[1] = indx_cnt[0];
        if (intPar[0] < 2)
          {
            intPar[0] += 1;
          }
        else
          {
            if (indx_change)
              {
                intPar[0] = 3;
              }
          }
      case CG_END:
        if (intPar[0] != 3)
          {
            angle_out[0] = (pos_hall[0] + hall_offset) * 3.14/3;
          }
        else
          {
            angle_out[0] = (pos_enc[0] - pos_indx[0])*M_PI*2/encoder_res + angle_offset;
          }
        break;
      default:
        break;
    }
}
