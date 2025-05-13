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

#include <pyblock.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>

static void init(python_block *block)
{
  int nin = block->nin;
  int ntot = nin*sizeof(double);
  int * intPar = block->intPar;
  int i;
  
  int shm_fd = shm_open(block->str, O_CREAT | O_RDWR, 0666);
  ftruncate(shm_fd, ntot);
  char  *ptr = mmap(0, ntot, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  
  if (ptr == MAP_FAILED) {
    exit(1);
  }

  for(i=0;i<ntot;i++) ptr[i] = 0x00;
  
  intPar[0] = shm_fd;
  block->ptrPar = (void *) ptr;
}

static void inout(python_block *block)
{
   double *u;
  int nin = block->nin;
  int i;
  double * ptr = (double *) block->ptrPar;
 
  for(i=0;i<nin;i++){
    u = block->u[i];
    ptr[i] = u[0];
  }
}

static void end(python_block *block)
{
  int * intPar    = block->intPar;
  close(intPar[0]);
}

void shmemOut(int flag, python_block *block)
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


