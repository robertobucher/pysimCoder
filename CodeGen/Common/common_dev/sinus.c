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

#include <math.h>
#include <pyblock.h>

double get_run_time(void);

static void init(python_block *block)
{
  double *y = block->y[0];

  y[0]=0.0;
}

static void inout(python_block *block)
{
  double * realPar = block->realPar;
  double *y = block->y[0];

  double t;
  double w,pi=3.1415927;
  double Amp = block->realPar[0];
  double Freq = block->realPar[1];
  double Phase = block->realPar[2];
  double Bias = block->realPar[3];
  double Delay = block->realPar[4];
  
  t = get_run_time();
  if(t<Delay) y[0] = 0.0;
  else {
    w=2*pi*Freq*(t-Delay)-Phase;
    y[0]=Amp*sin(w)+Bias;
  }
}

static void end(python_block *block)
{
  double *y = block->y[0];

  y[0]=0.0;
}

void sinus(int flag, python_block *block)
{
  if (flag==CG_OUT){          /* get input */
    inout(block);
  }
  else if (flag==CG_END){     /* termination */ 
    end(block);
  }
  else if (flag ==CG_INIT){    /* initialisation */
    init(block);
  }
}


