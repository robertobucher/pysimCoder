/*
COPYRIGHT (C) 2022  Roberto Bucher (roberto.bucher@supsi.ch)

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
#include "firmata.h"
#include "encoder.h"

extern t_firmata * af_firmataInstance;

static void init(python_block *block)
{
  int * intPar    = block->intPar;
  initialize_encoder(af_firmataInstance, intPar[0], intPar[1], intPar[2]);
}

static void inout(python_block *block)
{
  int * intPar    = block->intPar;
  double *y = block->y[0];

  if (af_firmataInstance == NULL) {
    printf("af - encoder inout - firmataInstance is empty - waiting\n");
    return;
  }

  while (!af_firmataInstance->isReady) {
    //Wait until device is up
    firmata_pull(af_firmataInstance);
  }
  get_encoderPos(af_firmataInstance, 0);
  firmata_pull(af_firmataInstance);

  y[0] = 1.0*af_firmataInstance->encoder_counter;
}

void encoderRead_af(int flag, python_block *block)
{
    if (flag == CG_OUT) {
        inout(block);
    } else if (flag == CG_STUPD) {
    } else if (flag == CG_END) {
    } else if (flag == CG_INIT) {
      init(block);
    }
}


