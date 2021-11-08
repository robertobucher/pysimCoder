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
#include <pthread.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>

static void * getData(void * p)
{
  int i;
  python_block *block = (python_block *) p;
  int recv_len, maxlen;
  int s = block->intPar[1];
  double *y;
  double data[block->nout];

  maxlen = block->nout*sizeof(double);

  while(1){
    recv_len = recvfrom(s, data, maxlen, 0, NULL,NULL);
    for( i=0;i<block->nout;i++){
      y = block->y[i];
      y[0] = data[i];
    }
  }
}

static void init(python_block *block)
{
  pthread_t thrd;
  int ret;
  int s;
  static struct sockaddr_in client;

  if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) exit(1);

  block->intPar[1] = s;

  client.sin_family      = AF_INET;
  client.sin_port         = htons(block->intPar[0]);
  client.sin_addr.s_addr = inet_addr(block->str);
  ret = bind (s, (struct sockaddr *) &client, sizeof(client));

  if(ret!=0) exit(1);

  pthread_create(&thrd, NULL, getData, (void *) block);
}

static void inout(python_block *block)
{
}

static void end(python_block *block)
{
  int * intPar = block->intPar;

  close(intPar[1]);
}

void UDPsocketRx(int flag, python_block *block)
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
