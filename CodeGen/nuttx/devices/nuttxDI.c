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
#include <nuttx/config.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <nuttx/ioexpander/gpio.h>

static void init(python_block *block)
{
  int * intPar    = block->intPar;
  int fd;
  int ret;  
  enum gpio_pintype_e pintype;
  
  fd = open(block->str, O_RDWR);
  if(fd<0) {
    fprintf(stderr,"ERROR: Failed to open %s\n", block->str);
    exit(1);
  }

  ret = ioctl(fd, GPIOC_PINTYPE, (unsigned long)((uintptr_t)&pintype));
  if (ret < 0)  {
    fprintf(stderr,"ERROR: Failed to read pintype from %s\n", block->str);
    close(fd);
    exit(1);
  }
  intPar[0] = fd;
}

static void inout(python_block *block)
{
  int * intPar    = block->intPar;
  double *y = block->y[0];
  int ret;
  int fd = intPar[0];
  bool invalue;
  
  ret = ioctl(fd, GPIOC_READ, (unsigned long)((uintptr_t)&invalue));
  if (ret < 0){
    fprintf(stderr,"ERROR: Failed to read value from %s\n", block->str);
    close(fd);
    exit(1);
  }
  if(invalue) y[0] = 1;
  else y[0] = 0;
}

static void end(python_block *block)
{
  int * intPar    = block->intPar;
  close(intPar[0]);
}

void nuttxDI(int flag, python_block *block)
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


