/*
COPYRIGHT (C) 2016  Roberto Bucher (roberto.bucher@supsi.ch)

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
#include <sys/types.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

double get_run_time();

void * reest_connection(void * p)
{
  int sock;
  struct  sockaddr_un sock_name;
  int ret;
  char mysocket[80];
  python_block *block = (python_block *) p;

  while(1){
    if(block->intPar[0]==0){
      strcpy(mysocket,block->str);
      if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
	sock = 0;
      }
      else{
	sock_name.sun_family = AF_UNIX;
	strcpy(sock_name.sun_path, (const char *) mysocket);
	ret = connect(sock, (struct sockaddr *) &sock_name, sizeof(sock_name));
	if (ret < 0){
	  sock=0;
	}
      }
      block->intPar[0] = sock;
    }
    usleep(100000);
  }
}

static void initC(python_block *block)
{
  int sock;
  struct  sockaddr_un sock_name;
  int ret;
  char mysocket[80];
  pthread_t thrd;

  signal(SIGPIPE, SIG_IGN);

  strcpy(mysocket,block->str);

  if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
    sock = 0;
  }
  else{
    sock_name.sun_family = AF_UNIX;
    strcpy(sock_name.sun_path, (const char *) mysocket);
    ret = connect(sock, (struct sockaddr *) &sock_name, sizeof(sock_name));
    if (ret < 0){
      sock=0;
    }
  }
  block->intPar[0] = sock;

  pthread_create(&thrd, NULL, reest_connection, (void *) block);
}

static void inoutC(python_block *block)
{
  int i, ret;
  double * u;

  struct {
    double val[block->nin];
  }values;
  
  if(block->intPar[0]){
    for(i=0;i<block->nin;i++){
      u = (double *) block->u[i];
      values.val[i] = u[0];
    }
    ret = send(block->intPar[0],&values,sizeof(values),0);
    if (ret<0) block->intPar[0]=0;
  }
}

static void endC(python_block *block)
{
  char mysocket[80];
  int ret;
  
  if(block->intPar[0]){
    strcpy(mysocket,block->str);
    
    ret = close(block->intPar[0]);
    if(!ret){
      unlink(mysocket);
    }
  }
}

void unixsockC(int flag, python_block *block)
{
  if (flag==CG_OUT){          /* get input */
    inoutC(block);
  }
  else if (flag==CG_END){     /* termination */ 
    endC(block);
  }
  else if (flag ==CG_INIT){    /* initialisation */
    initC(block);
  }
}

void * est_connection(void * p)
{
  int sock, sc;
  struct  sockaddr_un sock_name;
  int ret;
  char mysocket[80];
  python_block *block = (python_block *) p;
  int namelenght;
  int i;
  
  struct {
    double val[block->nout];
  }values;

  strcpy(mysocket,block->str);

  unlink(mysocket);

  if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
    block->intPar[0] = 0;
    return 0;
  }
  
  sock_name.sun_family = AF_UNIX;
  strcpy(sock_name.sun_path, (const char *) mysocket);

  ret = bind (sock,(struct sockaddr *) &sock_name, sizeof(sock_name));
  if (ret) {
    block->intPar[0] = 0;
    return 0;
  }
  
  ret = listen(sock,1);
  if (ret){
    ret=close(sock);
    if(!ret){
      unlink(mysocket);
    }
    block->intPar[0] = 0;
    return 0;
  }

  namelenght=sizeof(sock_name);
  while(1){
    sc = accept (sock, (struct sockaddr *)&sock_name, (socklen_t *)&namelenght);
    if (sc==-1){
      block->intPar[0] = 0;
      ret = close(sock);
      if(!ret){
	unlink(mysocket);
      }
      ret = close(sc);
      if(!ret){
	unlink(mysocket);
      }
    }
    block->intPar[0] = sock;
    block->intPar[1] = sc;

    while(sc){
      ret = recv(sc, &values, sizeof(values),0);
      if(ret==sizeof(values)){
	  for(i=0;i<block->nout;i++)
	    block->realPar[i] = values.val[i];
      }
    }
  }
}
  
static void initS(python_block *block)
{
  int ret;
  int i;
  double *y;
  
  pthread_t thrd;
  pthread_create(&thrd, NULL, est_connection, (void *) block);

}

static void inoutS(python_block *block)
{
  int i;
  double *y;
  
  for(i=0;i<block->nout;i++){
    y = (double *)block->y[i];
    y[0] = block->realPar[i];
  }
}

static void endS(python_block *block)
{
  char mysocket[80];
  int ret;

  strcpy(mysocket,block->str);
    
  ret = close(block->intPar[1]);
  if(!ret){
    unlink(mysocket);
  }
  ret = close(block->intPar[0]);
  if(!ret){
    unlink(mysocket);
  }
}

void unixsockS(int flag, python_block *block)
{
  if (flag==CG_OUT){          /* get input */
    inoutS(block);
  }
  else if (flag==CG_END){     /* termination */ 
    endS(block);
  }
  else if (flag ==CG_INIT){    /* initialisation */
    initS(block);
  }
}


