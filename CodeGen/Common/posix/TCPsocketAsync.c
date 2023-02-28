/*
  COPYRIGHT (C) 2023 Michal Lenc (michallenc@seznam.cz)
                2023 Pavel Píša (pisa@cmp.felk.cvut.cz)

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
#include <pthread.h>
#include <string.h>

#include "TCPdqf.h"

#define BUFFSIZE_DEFAULT 128
#define DATA_TO_APPLY 0x100

int get_priority_for_com(void);

/****************************************************************************
 * Name: TCP_data_read
 *
 * Description:
 *   POSIX thread that receives the data over TCP.
 *
 ****************************************************************************/

static void * TCP_data_read(void * p)
{
  python_block *block = (python_block *) p;
  tcp_txrx_state_t *txrxst = (tcp_txrx_state_t *)block->ptrPar;
  int ret;

  int rxbuff_in_read = 0;

  while (!txrxst->rx_terminate && !txrxst->rx_terminated)
    {
      double *buff = txrxst->rxbuff[rxbuff_in_read];
      int bytes_to_read = block->nout * sizeof(double);
      void *d = buff;
      while (bytes_to_read)
        {
          ret = read(txrxst->sockfd, d, bytes_to_read);
          if (ret <= 0)
            {
              printf("ERROR: TCP read failed\n");
              txrxst->rx_terminated = 1;
              break;
            }

          bytes_to_read -= ret;
          d += ret;
        }

      rxbuff_in_read |= DATA_TO_APPLY;
      rxbuff_in_read = atomic_exchange(&txrxst->rxbuff_to_apply,
                                       rxbuff_in_read);
      rxbuff_in_read &= ~(DATA_TO_APPLY);
    }

  pthread_exit(p);
}

/****************************************************************************
 * Name: TCP_data_send
 *
 * Description:
 *   POSIX thread that sends the data over TCP.
 *
 ****************************************************************************/

static void * TCP_data_send(void * p)
{
  python_block *block = (python_block *) p;
  tcp_txrx_state_t *txrxst = (tcp_txrx_state_t *)block->ptrPar;

  int ret;
  int terminate = 0;
  int empty = 0;
  while (!terminate || !empty)
    {
      void *d;
      int send_bytes;
      int to_send;
      pthread_mutex_lock(&txrxst->tcp_lock);
      terminate = txrxst->tx_terminate;
      while ((empty = tcp_dqf_base_is_empty(&txrxst->dqf)) && !terminate)
        {
          pthread_cond_wait(&txrxst->tcp_cond, &txrxst->tcp_lock);
          terminate = txrxst->tx_terminate;
        }

      to_send = tcp_dqf_count_cont(txrxst);

      pthread_mutex_unlock(&txrxst->tcp_lock);

      d = txrxst->buff + (txrxst->dqf.locout & txrxst->dqf.locmask);

      send_bytes = to_send * sizeof(txrxst->buff[0]);

      while (send_bytes)
        {
          ret = write(txrxst->sockfd, d, send_bytes);
          if (ret <= 0)
            {
              fprintf(stderr, "ERROR: TCP Send failed: %d\n", errno);
              terminate = empty = 1;
              break;
            }

          send_bytes -= ret;
          d += ret;
        }

      tcp_dqf_skip(txrxst, to_send - (send_bytes / sizeof(double)));
    }

  txrxst->tx_terminated = 1;
  pthread_cond_broadcast(&txrxst->tcp_cond);
  pthread_exit(p);
}

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
  tcp_txrx_state_t *txrxst = malloc(sizeof(tcp_txrx_state_t));

  char * IPbuf;
  struct hostent *he;
  const char *hostname = block->str;

  pthread_attr_t attr;
  struct sched_param schparam;
  int priority_com;

  memset(txrxst, 0, sizeof(tcp_txrx_state_t));

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

  txrxst->sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (txrxst->sockfd < 0)
    {
      printf("client socket failure %d\n", errno);
      exit(1);
    }

  server.sin_family      = AF_INET;
  server.sin_port        = htons(block->intPar[0]);
  server.sin_addr.s_addr = inet_addr(IPbuf);

  if (connect(txrxst->sockfd, (struct sockaddr *) &server, addrlen) < 0)
    {
      printf("client: connect to \"%s\" failure: %d\n",
              hostname, errno);
      close(txrxst->sockfd);
      exit(1);
    }

  int buffsize = intPar[1];
  int bs;
  if (buffsize == 0)
    {
      buffsize = BUFFSIZE_DEFAULT;
    }

  for (bs = buffsize - 1, buffsize = 1; bs; bs >>= 1, buffsize <<= 1);

  pthread_cond_init(&txrxst->tcp_cond, NULL);
  pthread_mutex_init(&txrxst->tcp_lock, NULL);

  txrxst->tx_terminate = txrxst->tx_terminated = 0;
  txrxst->rx_terminate = txrxst->rx_terminated = 0;

  if (block->nin > 0)
    {
      txrxst->buff = malloc(buffsize * sizeof(double));
    }
  if (block->nout > 0)
    {
      txrxst->rxbuff[0] = malloc(block->nout * sizeof(double));
      txrxst->rxbuff[1] = malloc(block->nout * sizeof(double));
      txrxst->rxbuff[2] = malloc(block->nout * sizeof(double));

      txrxst->rxbuff_in_apply = 1;
      txrxst->rxbuff_to_apply = 2;
    }

  tcp_dqf_init(txrxst, buffsize - 1);

  block->ptrPar = (void *)txrxst;

  priority_com = get_priority_for_com();

  if (priority_com > 0)
    {
      /* Set communication task priority */

      pthread_attr_init(&attr);
      pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
      pthread_attr_setschedpolicy(&attr, SCHED_FIFO);

      /* Set low priority */

      schparam.sched_priority = priority_com;
      pthread_attr_setschedparam(&attr, &schparam);
      if (block->nin > 0)
        {
          pthread_create(&txrxst->send_thrd, &attr, TCP_data_send,
                         (void *) block);
        }

      if (block->nout > 0)
        {
          pthread_create(&txrxst->rcv_thrd, NULL, TCP_data_read,
                         (void *) block);
        }

      pthread_attr_destroy(&attr);
    }
  else
    {
      /* Run thread with main application priority */

      if (block->nin > 0)
        {
          pthread_create(&txrxst->send_thrd, NULL, TCP_data_send,
                         (void *) block);
        }

      if (block->nout > 0)
        {
          pthread_create(&txrxst->rcv_thrd, NULL, TCP_data_read,
                         (void *) block);
        }
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
  double *u;
  double *y;
  int i;    
  tcp_txrx_state_t *txrxst = (tcp_txrx_state_t *)block->ptrPar;

  if (block->nin > 0)
    {
      if (tcp_dqf_free(txrxst) < block->nin)
      {
        txrxst->dqf.lostcount += 1;
        return;
      }

      for (i = 0; i < block->nin; i++)
        {
          u = block->u[i];
          tcp_dqf_put(txrxst, &u[0]);
        }

      pthread_cond_broadcast(&txrxst->tcp_cond);
    }

  if (block->nout > 0)
    {
      txrxst->rxbuff_in_apply = atomic_exchange(&txrxst->rxbuff_to_apply,
                                                txrxst->rxbuff_in_apply);
      if(txrxst->rxbuff_in_apply & DATA_TO_APPLY)
        {
          txrxst->rxbuff_in_apply &= ~(DATA_TO_APPLY);
          double *d = txrxst->rxbuff[txrxst->rxbuff_in_apply];
          for (i = 0; i < block->nout; i++)
            {
              y = block->y[i];
              y[0] = d[i];
            }
        }
    }
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
  tcp_txrx_state_t *txrxst = (tcp_txrx_state_t *)block->ptrPar;
  int terminated = 0;
  int ret;

  if (block->nin)
    {
      txrxst->tx_terminate = 1;
      pthread_cond_broadcast(&txrxst->tcp_cond);

      while (!terminated)
        {
          ret = 0;

          pthread_mutex_lock(&txrxst->tcp_lock);
          terminated = txrxst->tx_terminated;
          if (!terminated)
            {
              struct timespec ts;
              clock_gettime(CLOCK_REALTIME, &ts);
              ts.tv_sec += 2;
              ret = pthread_cond_timedwait(&txrxst->tcp_cond,
                                           &txrxst->tcp_lock, &ts);
            }
          pthread_mutex_unlock(&txrxst->tcp_lock);
          if (ret == ETIMEDOUT)
            {
              pthread_cancel(txrxst->send_thrd);
              break;
            }
        }

      pthread_join(txrxst->send_thrd, NULL);
      free(txrxst->buff);
    }
  if (block->nout)
    {
      txrxst->rx_terminate = 1;
      pthread_cancel(txrxst->rcv_thrd);
      pthread_join(txrxst->rcv_thrd, NULL);

      free(txrxst->rxbuff[0]);
      free(txrxst->rxbuff[1]);
      free(txrxst->rxbuff[2]);
    }

  close(txrxst->sockfd);
  free(txrxst);
}

/****************************************************************************
 * Name: TCPsocketAsync
 *
 * Description:
 *   Call needed function based on input flag.
 *
 ****************************************************************************/

void TCPsocketAsync(int flag, python_block *block)
{
  if (flag == CG_OUT)
    {
      inout(block);
    }
  else if (flag == CG_END)
    {
      end(block);
    }
  else if (flag == CG_INIT)
    {
      init(block);
    }
}
