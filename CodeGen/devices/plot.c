/*
COPYRIGHT (C) 2009  Roberto Bucher (roberto.bucher@supsi.ch)

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

double get_run_time();

static void init(python_block *block)
{
  FILE * fp;

  char fname[50];

  sprintf(fname,"/tmp/%s", block->str);
  fp=fopen(fname,"w");
  if(fp==NULL) exit(1);
  block->ptrPar = fp;
}

static void inout(python_block *block)
{
  double t;
  int i;
  double *u = block->u[0];
  FILE * fp = (FILE *) block->ptrPar;

  t = get_run_time();
  fprintf(fp,"%lf\t", t);
  for(i=0;i<block->nin;i++){
    u = (double *) block->u[i];
    fprintf(fp, "%lf\t", u[0]);
  }
  fprintf(fp, "\n"); 
}

static void end(python_block *block)
{
  FILE * fp = (FILE *) block->ptrPar;
  fclose(fp);
}

void plot(int flag, python_block *block)
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


