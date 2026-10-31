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

#ifndef TCP_DQF
#define TCP_DQF

#include <stdatomic.h>
#include <semaphore.h>

typedef struct tcp_dqf_base {
  unsigned int locin;
  unsigned int locout;
  unsigned int locmask;
  unsigned int lostcount;
} tcp_dqf_base_t;

typedef struct tcp_txrx_state {
  tcp_dqf_base_t dqf;
  char tx_terminate, tx_terminated;
  char rx_terminate, rx_terminated;
  int sockfd;
  double *buff;
  double *rxbuff[3];
  int rxbuff_to_apply;
  int rxbuff_in_apply;
  pthread_mutex_t tcp_lock;
  pthread_cond_t tcp_cond;
  pthread_t rcv_thrd, send_thrd;
} tcp_txrx_state_t;

static inline unsigned int
tcp_dqf_base_is_full(tcp_dqf_base_t *dqf)
{
  unsigned int locx = dqf->locin ^ dqf->locout;
  return (locx && !(locx & dqf->locmask));
}

static inline unsigned int
tcp_dqf_base_is_empty(tcp_dqf_base_t *dqf)
{
  return (dqf->locin == dqf->locout);
}

static inline int
tcp_dqf_put(tcp_txrx_state_t *cdqf, double *data)
{
  unsigned int locin;
  if (tcp_dqf_base_is_full(&cdqf->dqf))
    {
      return 0;
    }
  locin = cdqf->dqf.locin;
  cdqf->buff[(locin & cdqf->dqf.locmask)] = *data;
  atomic_store_explicit(&cdqf->dqf.locin, locin + 1, memory_order_release);
  return 1;
}

static inline int
tcp_dqf_get(tcp_txrx_state_t *cdqf, double *data)
{
  unsigned int locout;
  if (tcp_dqf_base_is_empty(&cdqf->dqf))
    {
      return 0;
    }
  locout = cdqf->dqf.locout;
  *data = cdqf->buff[(locout & cdqf->dqf.locmask)];
  cdqf->dqf.locout = ++locout;
  return 1;
}

static inline void
tcp_dqf_skip(tcp_txrx_state_t *cdqf, unsigned int skip)
{
  unsigned int locout, locdiff;
  locdiff = cdqf->dqf.locin - cdqf->dqf.locout;
  if (skip > locdiff)
    {
      skip = locdiff;
    }
  locout = cdqf->dqf.locout + skip;
  atomic_store_explicit(&cdqf->dqf.locout, locout, memory_order_relaxed);
}

static inline unsigned int
tcp_dqf_free(tcp_txrx_state_t *cdqf)
{
  unsigned int locout = atomic_load_explicit(&cdqf->dqf.locout, memory_order_relaxed);
  unsigned loccount = (cdqf->dqf.locmask + 1) - (cdqf->dqf.locin - locout);
  return loccount;
}

static inline unsigned int
tcp_dqf_count(tcp_txrx_state_t *cdqf)
{
  unsigned loccount = cdqf->dqf.locin - cdqf->dqf.locout;
  return loccount;
}

static inline int
tcp_dqf_count_cont(tcp_txrx_state_t *cdqf)
{
  unsigned int locin = atomic_load_explicit(&cdqf->dqf.locin, memory_order_acquire);
  int loccount = locin - cdqf->dqf.locout;
  int locremain = cdqf->dqf.locmask + 1 - (cdqf->dqf.locout & cdqf->dqf.locmask);
  return loccount < locremain ? loccount : locremain;
}

static inline void
tcp_dqf_init(tcp_txrx_state_t *cdqf, unsigned int locmask)
{
  cdqf->dqf.locmask = locmask;
  cdqf->dqf.locout = cdqf->dqf.locin = 0;
  cdqf->dqf.lostcount = 0;
}

#endif /* TCP_DQF */
