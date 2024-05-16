/*
  COPYRIGHT (C) 2022  Michal Lenc (michallenc@seznam.cz)

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

#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <errno.h>

#include <shv/chainpack/cchainpack.h>
#include <shv/chainpack/ccpon.h>

#include "ulut/ul_utdefs.h"

#include "shv_com.h"
#include "shv_tree.h"

int shv_write_err = 0;
int shv_init_done = 0;

int get_priority_for_com(void);

/****************************************************************************
 * Name: cid_alloc
 *
 * Description:
 *   Allocate (or reallocate) for CIDs.
 *
 ****************************************************************************/

int cid_alloc(shv_con_ctx_t * shv_ctx)
{
  if (shv_ctx->cid_capacity < shv_ctx->cid_cnt)
    {
      if (shv_ctx->cid_ptr == NULL)
        {
          shv_ctx->cid_ptr = (int *)malloc(shv_ctx->cid_cnt * sizeof(int));
          shv_ctx->cid_capacity = shv_ctx->cid_cnt;
        }
      else
        {
          int* tmp = (int*)realloc(shv_ctx->cid_ptr,
                                   (shv_ctx->cid_cnt)*sizeof(int));
          if (tmp == NULL)
            {
              fprintf(stderr, "ERROR cannot realloc\n");
              return -1;
            }

          shv_ctx->cid_ptr = tmp;
          shv_ctx->cid_capacity = shv_ctx->cid_cnt;
        }
    }

  return 1;
}

/****************************************************************************
 * Name: tcp_init
 *
 * Description:
 *   Initialize TCP client.
 *
 ****************************************************************************/

int tcp_init(void)
{
  int sockfd;
  struct sockaddr_in servaddr;
  uint16_t port;
  char *shv_broker_ip;
  char* shv_broker_port;

  /* Socket creation */

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1)
    {
      printf("ERROR: Socket creation failed.\n");
      exit(0);
    }
  else
    {
      printf("Socket successfully created.\n");
    }

  memset(&servaddr, 0, sizeof(servaddr));

  /* Get IP address and PORT from environment variables */

  shv_broker_ip = getenv("SHV_BROKER_IP");
  if (!shv_broker_ip)
    {
      printf("Unable to get SHV_BROKER_IP env variable.");
      return -1;
    }

  shv_broker_port = getenv("SHV_BROKER_PORT");
  if (!shv_broker_port)
    {
      printf("Unable to get SHV_BROKER_PORT env variable.");
      return -1;
    }
  else
    {
      port = atoi(shv_broker_port);
    }

  /* Assign server IP and PORT */

  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr(shv_broker_ip);
  servaddr.sin_port = htons(port);

  /* Connect the client socket to server socket */

  if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0)
    {
      printf("ERROR: Connection with the server failed.\n");
      return -1;
    }
  else
    {
      printf("Connected to the server.\n");
    }

  return sockfd;
}

/****************************************************************************
 * Name: tcp_terminate
 *
 * Description:
 *   Terminates TCP client.
 *
 ****************************************************************************/

void tcp_terminate(int fd)
{
  int ret;

  ret = close(fd);
  if (ret < 0)
    {
      printf("ERROR: tcp_terminate() cannot close connection to the server, \
              errno = %d\n", errno);
    }
  if (ret == 0)
    {
      printf("Client successfully disconnected.\n");
    }
}

/****************************************************************************
 * Name: shv_overflow_handler
 *
 * Description:
 *  Overflow handler responsible for data send.
 *
 ****************************************************************************/

void shv_overflow_handler(struct ccpcp_pack_context *ctx, size_t size_hint)
{

  shv_con_ctx_t *shv_ctx = UL_CONTAINEROF(ctx, shv_con_ctx_t, pack_ctx);
  size_t to_send = ctx->current - ctx->start;
  char * ptr_data = shv_ctx->shv_data;
  int ret = 0;

  if (shv_ctx->shv_send)
    {
      while ((shv_write_err == 0) && (to_send > 0))
        {
          ret = write(shv_ctx->stream_fd, ptr_data,
                      to_send);
          if (ret <= 0)
            {
              printf("ERROR: Write error, ret = %d\n", ret);
              shv_write_err = 1;
              if (ret == -1)
                {
                  printf("ERROR: Write error, errno = %d\n", errno);
                }
              break;
            }

          to_send -= ret;
          ptr_data += ret;
        }
    }

  shv_ctx->shv_len += ctx->current-ctx->start;
  ctx->current = ctx->start;
  ctx->end = ctx->start + SHV_BUF_LEN;
}

/****************************************************************************
 * Name: shv_underrflow_handler
 *
 * Description:
 *  Underflow handler responsible for data send.
 *
 ****************************************************************************/

size_t shv_underrflow_handler(struct ccpcp_unpack_context * ctx)
{
  int i;

  shv_con_ctx_t *shv_ctx = UL_CONTAINEROF(ctx, shv_con_ctx_t, unpack_ctx);

  i = read(shv_ctx->stream_fd, shv_ctx->shv_rd_data,
           sizeof(shv_ctx->shv_rd_data));
  if(i > 0)
    {
      ctx->start = shv_ctx->shv_rd_data;
      ctx->current = ctx->start;
      ctx->end = ctx->start + i;
    }

  return i;
}

/****************************************************************************
 * Name: shv_pack_head_request
 *
 * Description:
 *  Packs the head of the message for request send.
 *
 ****************************************************************************/

void shv_pack_head_request(shv_con_ctx_t *shv_ctx, char *met, char *path)
{
  cchainpack_pack_meta_begin(&shv_ctx->pack_ctx);

  cchainpack_pack_int(&shv_ctx->pack_ctx, 1);
  cchainpack_pack_int(&shv_ctx->pack_ctx, 1);

  cchainpack_pack_int(&shv_ctx->pack_ctx, TAG_REQUEST_ID);
  cchainpack_pack_int(&shv_ctx->pack_ctx, shv_ctx->rid);

  cchainpack_pack_int(&shv_ctx->pack_ctx, TAG_SHV_PATH);
  cchainpack_pack_string(&shv_ctx->pack_ctx, path, strlen(path));

  cchainpack_pack_int(&shv_ctx->pack_ctx, TAG_METHOD);
  cchainpack_pack_string(&shv_ctx->pack_ctx, met, strlen(met));

  cchainpack_pack_container_end(&shv_ctx->pack_ctx);
}

/****************************************************************************
 * Name: shv_pack_head_reply
 *
 * Description:
 *  Packs the head of the message for client reply.
 *
 ****************************************************************************/

void shv_pack_head_reply(shv_con_ctx_t *shv_ctx, int rid)
{
  cchainpack_pack_meta_begin(&shv_ctx->pack_ctx);

  cchainpack_pack_int(&shv_ctx->pack_ctx, 1);
  cchainpack_pack_int(&shv_ctx->pack_ctx, 1);

  cchainpack_pack_int(&shv_ctx->pack_ctx, TAG_REQUEST_ID);
  cchainpack_pack_int(&shv_ctx->pack_ctx, rid);

  if (shv_ctx->cid_cnt == 1)
    {
      cchainpack_pack_int(&shv_ctx->pack_ctx, TAG_CALLER_IDS);
      cchainpack_pack_int(&shv_ctx->pack_ctx, shv_ctx->cid_ptr[0]);
    }
  else
    {
      cchainpack_pack_list_begin(&shv_ctx->pack_ctx);
      for (int i = 0; i < shv_ctx->cid_cnt; i++)
        {
          cchainpack_pack_int(&shv_ctx->pack_ctx, shv_ctx->cid_ptr[i]);
        }

      cchainpack_pack_container_end(&shv_ctx->pack_ctx);
    }

  cchainpack_pack_container_end(&shv_ctx->pack_ctx);
}

/****************************************************************************
 * Name: shv_unpack_skip
 *
 * Description:
 *   Skip data inside container
 *
 ****************************************************************************/

int shv_unpack_skip(shv_con_ctx_t * shv_ctx)
{
  struct ccpcp_unpack_context *ctx = &shv_ctx->unpack_ctx;
  int level = 1;

  do
    {
      cchainpack_unpack_next(ctx);
      if (ctx->err_no != CCPCP_RC_OK) return -1;

      if ((ctx->item.type == CCPCP_ITEM_META) ||
          (ctx->item.type == CCPCP_ITEM_LIST) ||
          (ctx->item.type == CCPCP_ITEM_MAP) ||
          (ctx->item.type == CCPCP_ITEM_IMAP))
        {
          level++;
        }
      else if (ctx->item.type == CCPCP_ITEM_CONTAINER_END)
        {
          level--;
        }
    }
  while (level);
}


/****************************************************************************
 * Name: shv_unpack_head
 *
 * Description:
 *  Unpacks the head of the message.
 *
 ****************************************************************************/

int shv_unpack_head(shv_con_ctx_t * shv_ctx, int * rid, char * method,
                    char * path)
{
  int l;
  int i;
  bool ok;

  method[0] = '\0';
  path[0] = '\0';

  struct ccpcp_unpack_context *ctx = &shv_ctx->unpack_ctx;

  /* Unpack length */

  l = cchainpack_unpack_uint_data(ctx, &ok);

  cchainpack_unpack_next(ctx);
  if (ctx->err_no != CCPCP_RC_OK) return -1;
  if (ctx->item.type != CCPCP_ITEM_UINT) return -1;

  /* Check if protocol is chain pack */

  if (ctx->item.as.UInt != 1)  return -1;

  /* Data meta header */

  cchainpack_unpack_next(ctx);
  if (ctx->err_no != CCPCP_RC_OK) return -1;
  if (ctx->item.type != CCPCP_ITEM_META) return -1;

  do
    {
      cchainpack_unpack_next(ctx);
      if (ctx->err_no != CCPCP_RC_OK) return -1;
      if (ctx->item.type == CCPCP_ITEM_INT)
        {
          i = ctx->item.as.Int;
        }
      else
        {
          i = 0;
        }
      if (ctx->item.type==CCPCP_ITEM_CONTAINER_END) return (ctx->current - ctx->start);

      cchainpack_unpack_next(ctx);
      if (ctx->err_no != CCPCP_RC_OK) return -1;

      if (ctx->item.type == CCPCP_ITEM_INT)
        {
          if (i == TAG_REQUEST_ID)
            {
              *rid = ctx->item.as.Int;
            }

          if (i == TAG_CALLER_IDS)
            {
              shv_ctx->cid_cnt = 1;

              /* Allocate memory for CIDs */

              int ret = cid_alloc(shv_ctx);
              if (ret < 0)
                {
                  printf("ERROR: Memory allocation for CID failed\n");
                  exit(-1);
                }

              shv_ctx->cid_ptr[0] = ctx->item.as.Int;
            }
        }
      if (ctx->item.type == CCPCP_ITEM_UINT)
        {
          if (i == TAG_CALLER_IDS)
            {
              shv_ctx->cid_cnt = 1;

              /* Allocate memory for CIDs */

              int ret = cid_alloc(shv_ctx);
              if (ret < 0)
                {
                  printf("ERROR: Memory allocation for CID failed\n");
                  exit(-1);
                }

              shv_ctx->cid_ptr[0] = ctx->item.as.UInt;
            }
        }
      if ((ctx->item.type == CCPCP_ITEM_MAP) ||
          (ctx->item.type == CCPCP_ITEM_IMAP))
        {
          if (shv_unpack_skip(shv_ctx) < 0)
             return -1;
        }
      else if (ctx->item.type == CCPCP_ITEM_LIST)
        {
          if (i != TAG_CALLER_IDS)
            {
              shv_ctx->cid_cnt = 0;
              do
                {
                  cchainpack_unpack_next(ctx);
                  if (ctx->err_no != CCPCP_RC_OK) return -1;
                  if ((ctx->item.type == CCPCP_ITEM_INT) ||
                      (ctx->item.type == CCPCP_ITEM_UINT))
                    {
                      int ret;
                      shv_ctx->cid_cnt += 1;

                      /* Allocate memory for CIDs */

                      ret = cid_alloc(shv_ctx);
                      if (ret < 0)
                        {
                          printf("ERROR: Memory allocation for CID failed\n");
                          exit(-1);
                        }

                      if (ctx->item.type == CCPCP_ITEM_INT)
                        {
                          shv_ctx->cid_ptr[shv_ctx->cid_cnt - 1] = ctx->item.as.Int;
                        }
                      else
                        {
                          shv_ctx->cid_ptr[shv_ctx->cid_cnt - 1] = ctx->item.as.UInt;
                        }
                    }
	        }
              while (ctx->item.type != CCPCP_ITEM_CONTAINER_END);
            }
          else
            {
              if (shv_unpack_skip(shv_ctx) < 0)
                return -1;
            }
        }
      else if (ctx->item.type == CCPCP_ITEM_STRING)
        {
          /* If Tag is PATH (9) */

          if (i == TAG_SHV_PATH)
            {
              /* Save path to path string */

              if (ctx->item.as.String.string_size < SHV_PATH_LEN)
                {
                  strncpy(path,ctx->item.as.String.chunk_start,
                          ctx->item.as.String.string_size);
                  path[ctx->item.as.String.string_size] = 0;
                }
              else
                {
                  char error_msg[80];
                  sprintf(error_msg, "Requested PATH is too long.");
                  shv_send_error(shv_ctx, *rid, error_msg);
                }
            }

          /* If Tag is METHOD (10) */

          if (i == TAG_METHOD)
            {
              /* Save method name to method string */

              if (ctx->item.as.String.string_size < SHV_MET_LEN)
                {
                  strncpy(method,ctx->item.as.String.chunk_start,
                          ctx->item.as.String.string_size);
                  method[ctx->item.as.String.string_size] = 0;
                }
              else
                {
                  char error_msg[80];
                  sprintf(error_msg, "Requested METHOD is too long.");
                  shv_send_error(shv_ctx, *rid, error_msg);
                }
            }
        }
    } while (ctx->err_no == CCPCP_RC_OK);

  return 1;
}

/****************************************************************************
 * Name: shv_unpack_data
 *
 * Description:
 *  Unpacks the data.
 *
 ****************************************************************************/

int shv_unpack_data(ccpcp_unpack_context * ctx, int * v, double * d)
{
  int l = 0;
  int k = 1;

  do
    {
      cchainpack_unpack_next(ctx);
      if (ctx->err_no != CCPCP_RC_OK) return -1;

      if (ctx->item.type == CCPCP_ITEM_CONTAINER_END)
        {
          l--;
          if (l == 0) return (ctx->current-ctx->start);
        }
      else if ((ctx->item.type == CCPCP_ITEM_LIST) ||
               (ctx->item.type == CCPCP_ITEM_META) ||
               (ctx->item.type == CCPCP_ITEM_MAP)  ||
               (ctx->item.type == CCPCP_ITEM_IMAP))
        {
          l++;
        }
      else
        {
          if (l == 1)
            {
              if (k == 1)
                {
                  if (ctx->item.type == CCPCP_ITEM_INT)
                    {
                      if (ctx->item.as.Int == 1) k = 2;
                      else k = 0;
                    }
                  else if (ctx->item.type == CCPCP_ITEM_UINT)
                    {
                      if (ctx->item.as.UInt == 1) k = 2;
                      else k = 0;
                    }
                }
              else if (k == 2)
                {
                  if (ctx->item.type == CCPCP_ITEM_INT)
                    {
                      if (d) *d = (double) ctx->item.as.Int;
                      if (v) *v = ctx->item.as.Int;
                    }
                  else if (ctx->item.type == CCPCP_ITEM_UINT)
                    {
                      if (d) *d = (double) ctx->item.as.UInt;
                      if (v) *v = (int) ctx->item.as.UInt;
                    }
                  else if (ctx->item.type == CCPCP_ITEM_DECIMAL)
                    {
                      if (d) *d = (ctx->item.as.Decimal.mantisa * pow(10, ctx->item.as.Decimal.exponent));
                    }
                  else if (ctx->item.type == CCPCP_ITEM_DOUBLE)
                    {
                      if (d) *d = ctx->item.as.Double;
                    }
                }
            }
        }
    } while (ctx->err_no == CCPCP_RC_OK);
  return 1;
}

/****************************************************************************
 * Name: shv_process_input
 *
 * Description:
 *   Read the message from the broker.
 *
 ****************************************************************************/

int shv_process_input(shv_con_ctx_t * shv_ctx)
{
  int i;
  int j;
  char met[SHV_MET_LEN];
  char path[SHV_PATH_LEN];

  struct ccpcp_unpack_context *ctx = &shv_ctx->unpack_ctx;

  i = read(shv_ctx->stream_fd, shv_ctx->shv_rd_data,
          sizeof(shv_ctx->shv_rd_data));
  if (i > 0)
    {
      ccpcp_unpack_context_init(ctx, shv_ctx->shv_rd_data, i,
                                shv_underrflow_handler, 0);

      while (ctx->current < ctx->end)
        {
          /* Get method and path */

          shv_unpack_head(shv_ctx, &j, met, path);

          if (met[0] != '\0')
            {
              shv_node_process(shv_ctx, j, met, path);
            }
          else
            {
              /* We received message without specified method -> reply from
               * server.
               */

              shv_unpack_data(ctx, 0, 0);
            }
        }
    }

  return i;
}

/****************************************************************************
 * Name: shv_process
 *
 * Description:
 *   A separate thread that processes the received messages.
 *
 ****************************************************************************/

static void *shv_process(void * p)
{
  int num_events;
  int ret;
  shv_con_ctx_t *shv_ctx = (shv_con_ctx_t *)p;

  struct pollfd pfds[1];
  pfds[0].fd = shv_ctx->stream_fd;
  pfds[0].events = POLLIN;

  while (1)
    {
      /* Set timemout to one half of shv_ctx->timeout (in ms) */

      num_events = poll(pfds, 1, (shv_ctx->timeout * 1000) / 2);

      if (num_events == 0)
        {
          /* Poll timeout, send ping */

          shv_send_ping(shv_ctx);
        }
      else if (pfds[0].revents & POLLIN)
        {
          /* Event happened on our socket, process TCP input */

          ret = shv_process_input(shv_ctx);
        }
    }

  return NULL;
}

/****************************************************************************
 * Name: shv_send_ping
 *
 * Description:
 *   Send integer to the broker.
 *
 ****************************************************************************/

void shv_send_ping(shv_con_ctx_t *shv_ctx)
{
  ccpcp_pack_context_init(&shv_ctx->pack_ctx, shv_ctx->shv_data, SHV_BUF_LEN,
                          shv_overflow_handler);

  for (shv_ctx->shv_send = 0; shv_ctx->shv_send < 2; shv_ctx->shv_send++)
    {
      if (shv_ctx->shv_send)
        {
          cchainpack_pack_uint_data(&shv_ctx->pack_ctx, shv_ctx->shv_len);
        }

      shv_ctx->shv_len = 0;
      cchainpack_pack_uint_data(&shv_ctx->pack_ctx, 1);

      shv_pack_head_request(shv_ctx, "ping", ".broker/app");

      shv_ctx->rid += 1;

      cchainpack_pack_imap_begin(&shv_ctx->pack_ctx);
      cchainpack_pack_container_end(&shv_ctx->pack_ctx);
      shv_overflow_handler(&shv_ctx->pack_ctx, 0);
    }
}

/****************************************************************************
 * Name: shv_send_int
 *
 * Description:
 *   Send integer to the broker.
 *
 ****************************************************************************/

void shv_send_int(shv_con_ctx_t *shv_ctx, int rid, int num)
{
  ccpcp_pack_context_init(&shv_ctx->pack_ctx, shv_ctx->shv_data, SHV_BUF_LEN,
                          shv_overflow_handler);

  for (shv_ctx->shv_send = 0; shv_ctx->shv_send < 2; shv_ctx->shv_send++)
    {
      if (shv_ctx->shv_send)
        {
          cchainpack_pack_uint_data(&shv_ctx->pack_ctx, shv_ctx->shv_len);
        }

      shv_ctx->shv_len = 0;
      cchainpack_pack_uint_data(&shv_ctx->pack_ctx, 1);

      shv_pack_head_reply(shv_ctx, rid);

      cchainpack_pack_imap_begin(&shv_ctx->pack_ctx);
      cchainpack_pack_int(&shv_ctx->pack_ctx, 2);
      cchainpack_pack_int(&shv_ctx->pack_ctx, num);
      cchainpack_pack_container_end(&shv_ctx->pack_ctx);
      shv_overflow_handler(&shv_ctx->pack_ctx, 0);
    }
}

/****************************************************************************
 * Name: shv_send_double
 *
 * Description:
 *   Send double to the broker.
 *
 ****************************************************************************/

void shv_send_double(shv_con_ctx_t *shv_ctx, int rid, double num)
{
  ccpcp_pack_context_init(&shv_ctx->pack_ctx,shv_ctx->shv_data,
                          SHV_BUF_LEN,shv_overflow_handler);

  for (shv_ctx->shv_send = 0; shv_ctx->shv_send < 2; shv_ctx->shv_send++)
    {
      if (shv_ctx->shv_send)
        {
          cchainpack_pack_uint_data(&shv_ctx->pack_ctx, shv_ctx->shv_len);
        }

      shv_ctx->shv_len = 0;
      cchainpack_pack_uint_data(&shv_ctx->pack_ctx, 1);

      shv_pack_head_reply(shv_ctx, rid);

      cchainpack_pack_imap_begin(&shv_ctx->pack_ctx);
      cchainpack_pack_int(&shv_ctx->pack_ctx, 2);
      cchainpack_pack_double(&shv_ctx->pack_ctx, num);
      cchainpack_pack_container_end(&shv_ctx->pack_ctx);
      shv_overflow_handler(&shv_ctx->pack_ctx, 0);
    }
}

/****************************************************************************
 * Name: shv_send_str
 *
 * Description:
 *   Send string to the broker.
 *
 ****************************************************************************/

void shv_send_str(shv_con_ctx_t *shv_ctx, int rid, const char *str)
{
  ccpcp_pack_context_init(&shv_ctx->pack_ctx,shv_ctx->shv_data, SHV_BUF_LEN,
                          shv_overflow_handler);

  for (shv_ctx->shv_send = 0; shv_ctx->shv_send < 2; shv_ctx->shv_send++)
    {
      if (shv_ctx->shv_send)
        {
          cchainpack_pack_uint_data(&shv_ctx->pack_ctx, shv_ctx->shv_len);
        }

      shv_ctx->shv_len = 0;
      cchainpack_pack_uint_data(&shv_ctx->pack_ctx, 1);

      shv_pack_head_reply(shv_ctx, rid);

      cchainpack_pack_imap_begin(&shv_ctx->pack_ctx);
      cchainpack_pack_int(&shv_ctx->pack_ctx, 2);
      cchainpack_pack_string(&shv_ctx->pack_ctx, str, strlen(str));
      cchainpack_pack_container_end(&shv_ctx->pack_ctx);
      shv_overflow_handler(&shv_ctx->pack_ctx, 0);
    }
}


/****************************************************************************
 * Name: shv_send_str_list
 *
 * Description:
 *   Send list of strings to the broker.
 *
 ****************************************************************************/

void shv_send_str_list(shv_con_ctx_t *shv_ctx, int rid, int num_str,
                       const char **str)
{
  ccpcp_pack_context_init(&shv_ctx->pack_ctx,shv_ctx->shv_data, SHV_BUF_LEN,
                          shv_overflow_handler);

  for (shv_ctx->shv_send = 0; shv_ctx->shv_send < 2; shv_ctx->shv_send++)
    {
      if (shv_ctx->shv_send)
        {
          cchainpack_pack_uint_data(&shv_ctx->pack_ctx, shv_ctx->shv_len);
        }

      shv_ctx->shv_len = 0;
      cchainpack_pack_uint_data(&shv_ctx->pack_ctx, 1);

      shv_pack_head_reply(shv_ctx, rid);

      cchainpack_pack_imap_begin(&shv_ctx->pack_ctx);
      cchainpack_pack_int(&shv_ctx->pack_ctx, 2);
      cchainpack_pack_list_begin(&shv_ctx->pack_ctx);
      for (int i = 0; i < num_str; i++)
        {
          cchainpack_pack_string(&shv_ctx->pack_ctx,str[i], strlen(str[i]));
        }

      cchainpack_pack_container_end(&shv_ctx->pack_ctx);
      cchainpack_pack_container_end(&shv_ctx->pack_ctx);
      shv_overflow_handler(&shv_ctx->pack_ctx, 0);
    }
}

/****************************************************************************
 * Name: shv_send_str_list_it
 *
 * Description:
 *   Send list of strings to the broker.
 *   This variant uses iterator to access strigs provided by other source.
 *
 ****************************************************************************/

void shv_send_str_list_it(shv_con_ctx_t *shv_ctx, int rid, int num_str,
                          shv_str_list_it_t *str_it)
{
  ccpcp_pack_context_init(&shv_ctx->pack_ctx,shv_ctx->shv_data, SHV_BUF_LEN,
                          shv_overflow_handler);

  for (shv_ctx->shv_send = 0; shv_ctx->shv_send < 2; shv_ctx->shv_send++)
    {
      int first_next_over;

      if (shv_ctx->shv_send)
        {
          cchainpack_pack_uint_data(&shv_ctx->pack_ctx, shv_ctx->shv_len);
        }

      shv_ctx->shv_len = 0;
      cchainpack_pack_uint_data(&shv_ctx->pack_ctx, 1);

      shv_pack_head_reply(shv_ctx, rid);

      cchainpack_pack_imap_begin(&shv_ctx->pack_ctx);
      cchainpack_pack_int(&shv_ctx->pack_ctx, 2);
      cchainpack_pack_list_begin(&shv_ctx->pack_ctx);

      first_next_over = 1;
      for (int i = 0; i < num_str; i++)
        {
	  const char *str = NULL;

	  if (first_next_over >= 0)
	    {
	      str = str_it->get_next_entry(str_it, first_next_over);
	      if (str != NULL)
	        {
                  first_next_over = 0;
		}
	      else
	        {
                  first_next_over = -1;
		}
	    }
	  if (str == NULL)
	    {
	      str = "";
	    }
          cchainpack_pack_string(&shv_ctx->pack_ctx, str, strlen(str));
        }

      cchainpack_pack_container_end(&shv_ctx->pack_ctx);
      cchainpack_pack_container_end(&shv_ctx->pack_ctx);
      shv_overflow_handler(&shv_ctx->pack_ctx, 0);
    }
}

/****************************************************************************
 * Name: shv_send_error
 *
 * Description:
 *   Send error to the broker.
 *
 ****************************************************************************/

void shv_send_error(shv_con_ctx_t *shv_ctx, int rid, const char *msg)
{
  ccpcp_pack_context_init(&shv_ctx->pack_ctx,shv_ctx->shv_data,
                          SHV_BUF_LEN, shv_overflow_handler);

  for (shv_ctx->shv_send = 0; shv_ctx->shv_send < 2; shv_ctx->shv_send++)
    {
      if (shv_ctx->shv_send)
        {
          cchainpack_pack_uint_data(&shv_ctx->pack_ctx, shv_ctx->shv_len);
        }

      shv_ctx->shv_len = 0;
      cchainpack_pack_uint_data(&shv_ctx->pack_ctx, 1);

      shv_pack_head_reply(shv_ctx, rid);

      cchainpack_pack_imap_begin(&shv_ctx->pack_ctx);
      cchainpack_pack_int(&shv_ctx->pack_ctx, 3);
      cchainpack_pack_imap_begin(&shv_ctx->pack_ctx);
      cchainpack_pack_int(&shv_ctx->pack_ctx, 1);
      cchainpack_pack_int(&shv_ctx->pack_ctx, TAG_ERROR);
      cchainpack_pack_int(&shv_ctx->pack_ctx, 2);
      cchainpack_pack_string(&shv_ctx->pack_ctx, msg, strlen(msg));
      cchainpack_pack_container_end(&shv_ctx->pack_ctx);
      cchainpack_pack_container_end(&shv_ctx->pack_ctx);
      shv_overflow_handler(&shv_ctx->pack_ctx, 0);
    }
}

/****************************************************************************
 * Name: shv_login
 *
 * Description:
 *   Login to SHV broker.
 *
 ****************************************************************************/

int shv_login(shv_con_ctx_t *shv_ctx)
{
  int i;
  int id = 0;
  char* shv_broker_user;
  char* shv_broker_passw;
  char* shv_broker_devid;
  char* shv_broker_mount;

  shv_write_err = 0;

  shv_broker_user = getenv("SHV_BROKER_USER");
  if (!shv_broker_user)
    {
      printf("Unable to get SHV_BROKER_USER env variable.");
      return -1;
    }

  shv_broker_passw = getenv("SHV_BROKER_PASSWORD");
  if (!shv_broker_passw)
    {
      printf("Unable to get SHV_BROKER_PASSWORD env variable.");
      return -1;
    }

  shv_broker_devid = getenv("SHV_BROKER_DEV_ID");
  if (!shv_broker_devid)
    {
      shv_broker_devid = "pysim";
    }

  shv_broker_mount = getenv("SHV_BROKER_MOUNT");
  if (!shv_broker_mount)
    {
      shv_broker_mount = "test/pysim";
    }

  ccpcp_pack_context_init(&shv_ctx->pack_ctx, shv_ctx->shv_data,
                          SHV_BUF_LEN, shv_overflow_handler);

  for (shv_ctx->shv_send = 0; shv_ctx->shv_send < 2; shv_ctx->shv_send++)
    {
      if (shv_ctx->shv_send)
        {
          cchainpack_pack_uint_data(&shv_ctx->pack_ctx, shv_ctx->shv_len);
        }

      shv_ctx->shv_len = 0;

      /* Chainpack */

      cchainpack_pack_uint_data(&shv_ctx->pack_ctx, 1);

      /* Start meta data header */

      cchainpack_pack_meta_begin(&shv_ctx->pack_ctx);

      /* Chainpack RPC */

      cchainpack_pack_int(&shv_ctx->pack_ctx, 1);
      cchainpack_pack_int(&shv_ctx->pack_ctx, 1);

      /* Tag 8:2 */

      cchainpack_pack_int(&shv_ctx->pack_ctx, TAG_REQUEST_ID);
      cchainpack_pack_int(&shv_ctx->pack_ctx, 1);

      /* Method: "hello" */

      cchainpack_pack_int(&shv_ctx->pack_ctx, TAG_METHOD);
      cchainpack_pack_string(&shv_ctx->pack_ctx, "hello", 5);

      cchainpack_pack_container_end(&shv_ctx->pack_ctx);

      /* Data: none */

      cchainpack_pack_imap_begin(&shv_ctx->pack_ctx);
      cchainpack_pack_container_end(&shv_ctx->pack_ctx);
      shv_overflow_handler(&shv_ctx->pack_ctx, 0);
    }

  /* Now init message <1:1,8:1,10:"hello">i{} was sent,
   * wait for reply from server
   */

  i = read(shv_ctx->stream_fd, shv_ctx->shv_data,
           sizeof(shv_ctx->shv_data));
  if (i <= 0)
    {
      return i;
    }

  /* Start method "login" */

  ccpcp_pack_context_init(&shv_ctx->pack_ctx, shv_ctx->shv_data,
                          SHV_BUF_LEN, shv_overflow_handler);

  for (shv_ctx->shv_send = 0; shv_ctx->shv_send < 2; shv_ctx->shv_send++)
    {
      if (shv_ctx->shv_send)
        {
          cchainpack_pack_uint_data(&shv_ctx->pack_ctx, shv_ctx->shv_len);
        }

      shv_ctx->shv_len = 0;

      /* Chainpack: 1 */

      cchainpack_pack_uint_data(&shv_ctx->pack_ctx, 1);

      /* Begin data meta header */

      cchainpack_pack_meta_begin(&shv_ctx->pack_ctx);

      /* Chainpack RPC 1:1 */

      cchainpack_pack_int(&shv_ctx->pack_ctx, 1);
      cchainpack_pack_int(&shv_ctx->pack_ctx, 1);

      /* TAG 8:2 */

      cchainpack_pack_int(&shv_ctx->pack_ctx, TAG_REQUEST_ID);
      cchainpack_pack_int(&shv_ctx->pack_ctx, 2);

      /* Method: "login" */

      cchainpack_pack_int(&shv_ctx->pack_ctx, TAG_METHOD);
      cchainpack_pack_string(&shv_ctx->pack_ctx, "login", 5);

      cchainpack_pack_container_end(&shv_ctx->pack_ctx);

      /* Begin data */

      cchainpack_pack_imap_begin(&shv_ctx->pack_ctx);

      cchainpack_pack_int(&shv_ctx->pack_ctx, 1);

      cchainpack_pack_map_begin(&shv_ctx->pack_ctx);

      cchainpack_pack_string(&shv_ctx->pack_ctx, "login", 5);
      cchainpack_pack_map_begin(&shv_ctx->pack_ctx);

      cchainpack_pack_string(&shv_ctx->pack_ctx, "password", 8);
      cchainpack_pack_string(&shv_ctx->pack_ctx, shv_broker_passw,
                             strlen(shv_broker_passw));

      cchainpack_pack_string(&shv_ctx->pack_ctx, "type", 4);
      cchainpack_pack_string(&shv_ctx->pack_ctx, "PLAIN", 5);

      cchainpack_pack_string(&shv_ctx->pack_ctx, "user", 4);
      cchainpack_pack_string(&shv_ctx->pack_ctx, shv_broker_user,
                             strlen(shv_broker_user));

      cchainpack_pack_container_end(&shv_ctx->pack_ctx);

      cchainpack_pack_string(&shv_ctx->pack_ctx, "options", 7);
      cchainpack_pack_map_begin(&shv_ctx->pack_ctx);

      cchainpack_pack_string(&shv_ctx->pack_ctx, "device", 6);
      cchainpack_pack_map_begin(&shv_ctx->pack_ctx);

      cchainpack_pack_string(&shv_ctx->pack_ctx, "deviceId", 8);
	    cchainpack_pack_string(&shv_ctx->pack_ctx, shv_broker_devid,
                                                 strlen(shv_broker_devid));

      cchainpack_pack_string(&shv_ctx->pack_ctx, "mountPoint", 10);
      cchainpack_pack_string(&shv_ctx->pack_ctx, shv_broker_mount,
                                                 strlen(shv_broker_mount));

      cchainpack_pack_container_end(&shv_ctx->pack_ctx);

      cchainpack_pack_string(&shv_ctx->pack_ctx, "idleWatchDogTimeOut", 19);
      cchainpack_pack_int(&shv_ctx->pack_ctx, shv_ctx->timeout);

      cchainpack_pack_container_end(&shv_ctx->pack_ctx);
      cchainpack_pack_container_end(&shv_ctx->pack_ctx);
      cchainpack_pack_container_end(&shv_ctx->pack_ctx);
      shv_overflow_handler(&shv_ctx->pack_ctx, 0);
  }

  i = read(shv_ctx->stream_fd, shv_ctx->shv_data, sizeof(shv_ctx->shv_data));
  if (i <= 0)
    {
      return i;
    }

  return id;
}

/****************************************************************************
 * Name: shv_com_end
 *
 * Description:
 *   Terminates TCP connection.
 *
 ****************************************************************************/

void shv_com_end(shv_con_ctx_t *ctx)
{
  int fd = ctx->stream_fd;

  tcp_terminate(fd);
}

/****************************************************************************
 * Name: shv_con_ctx_init
 *
 * Description:
 *   Fill the shv_con_ctx_t with variables.
 *
 ****************************************************************************/

void shv_con_ctx_init(shv_con_ctx_t *shv_ctx, struct shv_node *root)
{
  memset(shv_ctx, 0, sizeof(shv_con_ctx_t));

  shv_ctx->root = root;
  shv_ctx->stream_fd = tcp_init();
  shv_ctx->timeout = 360;
  shv_ctx->rid = 3;
}

/****************************************************************************
 * Name: shv_com_init
 *
 * Description:
 *   The functions connects TCP to the server and calls shv_login() that
 *   performs login to the broker.
 *
 ****************************************************************************/

shv_con_ctx_t *shv_com_init(struct shv_node *root)
{
  int ret;
  pthread_t thrd;
  pthread_attr_t attr;
  struct sched_param schparam;
  int priority_com;

  if (shv_init_done)
    {
      return NULL;
    }

  shv_con_ctx_t *shv_ctx = (shv_con_ctx_t *)malloc(sizeof(shv_con_ctx_t));
  if (shv_ctx == NULL)
    {
      printf("ERROR: Failed to allocate memory for shv_ctx\n");
      return NULL;
    }

  shv_con_ctx_init(shv_ctx, root);

  ret = shv_login(shv_ctx);
  if (ret < 0)
    {
      printf("ERROR: shv_login() failed, ret = %d\n", ret);
      return NULL;
    }

  /* Create pthread that receives messages from the server and performs
   * SHV operations.
   */

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
      pthread_create(&thrd, &attr, shv_process, shv_ctx);

      pthread_attr_destroy(&attr);
    }
  else
    {
      /* Run thread with main application priority */

      pthread_create(&thrd, NULL, shv_process, shv_ctx);
    }

  /* Already initialized */

  shv_init_done = 1;

  return shv_ctx;
}
