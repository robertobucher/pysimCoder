/*
  COPYRIGHT (C) 2021 Roberto Bucher (roberto.bucher@supsi.ch)

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
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <math.h>
#include <errno.h>

/****************************************************************************
 * Name: init
 *
 * Description:
 *   Connect the socket.
 *
 ****************************************************************************/

static void init(python_block *block)
{
  int * intPar    = block->intPar;
  struct sockaddr_in server;
  socklen_t addrlen = sizeof(struct sockaddr_in);

  char * IPbuf;
  struct hostent *he;
  const char *hostname = block->str;

#ifdef CG_WITH_ENV_HOST_ADDR
  if (hostname != NULL)
    {
      if (*hostname == '$')
        {
          const char *envname = hostname + 1;
          hostname = getenv(envname);
          if (hostname == NULL)
            {
              printf("environment variable \"%s\" not set\n", envname);
              exit(1);
            }
        }
    }
#endif /*CG_WITH_ENV_HOST_ADDR*/

  if ((he = gethostbyname(hostname)) == NULL)
     {
       printf("gethostbyname failure \"%s\" unresolved\n", hostname);
       exit(1);
     }
  IPbuf =  inet_ntoa(*((struct in_addr*) he->h_addr_list[0]));

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    {
      printf("client socket failure %d\n", errno);
      exit(1);
    }
  intPar[1] = sockfd;

  server.sin_family      = AF_INET;
  server.sin_port        = htons(block->intPar[0]);
  server.sin_addr.s_addr = inet_addr(IPbuf);

  if (connect(sockfd, (struct sockaddr *) &server, addrlen) < 0)
    {
      printf("client: connect to \"%s\" failure: %d\n", hostname, errno);
      close(sockfd);
      exit(1);
    }
}

/****************************************************************************
 * Name: inout
 *
 * Description:
 *   Send data over TCP.
 *
 ****************************************************************************/

static void inout(python_block *block)
{
  int i;
  int * intPar = block->intPar;
  double *u;
  double data[block->nin];

  int s = intPar[1];

  for(i=0;i<block->nin;i++)
    {
      u = block->u[i];
      data[i] = u[0];
    }

  send(s, data, sizeof(data) , 0);
}

/****************************************************************************
 * Name: init
 *
 * Description:
 *   Close the socket.
 *
 ****************************************************************************/

static void end(python_block *block)
{
  int * intPar    = block->intPar;

  close(intPar[1]);
}

/****************************************************************************
 * Name: TCPsocketTx
 *
 * Description:
 *   Call needed function based on input flag.
 *
 ****************************************************************************/

void TCPsocketTx(int flag, python_block *block)
{
  if (flag==CG_OUT){          /* get input */
    inout(block);
  }
  else if (flag==CG_END){     /* termination */
    end(block);
  }
  else if (flag ==CG_INIT){   /* initialisation */
    init(block);
  }
}
