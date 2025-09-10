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

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <pyblock.h>

#include "ulut/ul_utdefs.h"
#include "shv/tree/shv_tree.h"
#include "shv/tree/shv_methods.h"
#include "shv_pysim.h"

const shv_method_des_t * const shv_blk_dmap_items[] = {
  &shv_dmap_item_dir,
  &shv_dmap_item_ls,
};

extern int get_priority_for_com(void);

const shv_dmap_t shv_blk_dmap = {.methods = {.items = (void **)shv_blk_dmap_items,
                                             .count = sizeof(shv_blk_dmap_items)/sizeof(shv_blk_dmap_items[0]),
                                             .alloc_count = 0,
                                            }};

/****************************************************************************
 * Name: shv_add_block
 *
 * Description:
 *  Adds block's inputs, outputs and parameters to SHV tree.
 *
 ****************************************************************************/

void shv_add_block(python_block_name_map *block_map, int index,
                   shv_node_t *item_blk, int mode)
{
  shv_node_t *item_blk_par = shv_tree_node_new("parameters", &shv_dir_ls_dmap, mode);
  if (item_blk_par == NULL)
    {
      printf("ERROR: Failed to allocate memory for SHV tree block's parameters.");
      return;
    }

  shv_tree_add_child(item_blk, item_blk_par);

  /* For each parameter */

  for (int j = 0; j < block_map->block_structure[index].realParNum; j++)
    {
      const char *par_name = block_map->block_structure[index].realParNames[j];
      shv_node_typed_val_t *item_par = shv_tree_node_typed_val_new(par_name,
                                                            &shv_double_dmap, mode);

      item_par->val_ptr = &block_map->block_structure[index].realPar[j];
      item_par->type_name = "double";

      shv_tree_add_child(item_blk_par, &item_par->shv_node);
    }

  shv_node_t *item_blk_ins = shv_tree_node_new("inputs", &shv_dir_ls_dmap, mode);
  if (item_blk_ins == NULL)
    {
      printf("ERROR: Failed to allocate memory for SHV tree block's inputs.");
      return;
    }

  shv_tree_add_child(item_blk, item_blk_ins);

  for (int j = 0; j < block_map->block_structure[index].nin; j++)
    {
      char *input_name = malloc(sizeof(char) * 8);
      if (input_name == NULL)
        {
          printf("ERROR: malloc() failed to allocate space for input_name.\n");
          continue;
        }

      sprintf(input_name, "input%d", j);
      shv_node_typed_val_t *item_val = shv_tree_node_typed_val_new(input_name,
                                                           &shv_double_read_only_dmap,
                                                           mode);

      double *u = block_map->block_structure[index].u[j];
      item_val->val_ptr = &u[0];
      item_val->type_name = "double";

      shv_tree_add_child(item_blk_ins, &item_val->shv_node);
    }

  shv_node_t *item_blk_outs = shv_tree_node_new("outputs", &shv_dir_ls_dmap, mode);
  if (item_blk_outs == NULL)
    {
      printf("ERROR: Failed to allocate memory for SHV tree block's inputs.");
      return;
    }

  shv_tree_add_child(item_blk, item_blk_outs);

  for (int j = 0; j < block_map->block_structure[index].nout; j++)
    {
      char *output_name = malloc(sizeof(char) * 8);
      if (output_name == NULL)
        {
          printf("ERROR: malloc() failed to allocate space for input_name.\n");
          continue;
        }

      sprintf(output_name, "output%d", j);
      shv_node_typed_val_t *item_val = shv_tree_node_typed_val_new(output_name,
                                                           &shv_double_read_only_dmap,
                                                           mode);

      double *u = block_map->block_structure[index].y[j];
      item_val->val_ptr = &u[0];
      item_val->type_name = "double";

      shv_tree_add_child(item_blk_outs, &item_val->shv_node);
    }
}

/****************************************************************************
 * Name: shv_add_system_in
 *
 * Description:
 *  Adds system input to SHV tree.
 *
 ****************************************************************************/

void shv_add_system_in(python_block_name_map *block_map, int index,
                       shv_node_t *item_blk, int mode)
{
  /* For each block input */

  for (int j = 0; j < block_map->block_structure[index].nout; j++)
    {
      char *input_name = malloc(sizeof(char) * 8);
      if (input_name == NULL)
        {
          printf("ERROR: malloc() failed to allocate space for input_name.\n");
          continue;
        }

      sprintf(input_name, "input%d", j);
      shv_node_typed_val_t *item_val = shv_tree_node_typed_val_new(input_name,
                                                            &shv_double_dmap,
                                                            mode);

      double *y = block_map->block_structure[index].y[j];
      item_val->val_ptr = &y[0];
      item_val->type_name = "double";

      shv_tree_add_child(item_blk, &item_val->shv_node);
    }
}

/****************************************************************************
 * Name: shv_add_system_out
 *
 * Description:
 *  Adds system output to SHV tree.
 *
 ****************************************************************************/

void shv_add_system_out(python_block_name_map *block_map, int index,
                       shv_node_t *item_blk, int mode)
{
  /* For each block input */

  for (int j = 0; j < block_map->block_structure[index].nin; j++)
    {
      char *output_name = malloc(sizeof(char) * 8);
      if (output_name == NULL)
        {
          printf("ERROR: malloc() failed to allocate space for output_name.\n");
          continue;
        }

      sprintf(output_name, "output%d", j);
      shv_node_typed_val_t *item_val = shv_tree_node_typed_val_new(output_name,
                                                            &shv_double_read_only_dmap,
                                                            mode);

      double *u = block_map->block_structure[index].u[j];
      item_val->val_ptr = &u[0];
      item_val->type_name = "double";

      shv_tree_add_child(item_blk, &item_val->shv_node);
    }
}

/****************************************************************************
 * Name: shv_tree_create
 *
 * Description:
 *  Initialize and fill the SHV tree with blocks and parameters.
 *
 ****************************************************************************/

void shv_tree_create(python_block_name_map * block_map,
                     shv_node_t *shv_tree_root,
                     int mode)
{
  int n_blocks = block_map->blocks_count;
  shv_node_t *item_in;
  shv_node_t *item_out;
  shv_node_t *item_blocks;

  /* Initialization of tree root */

  if (mode & SHV_NLIST_MODE_GSA)
    {
      shv_node_list_gsa_init_array_field(&shv_tree_root->children);
      shv_tree_root->children.mode = mode;
    }
  else
    {
      shv_node_list_gavl_init_root_field(&shv_tree_root->children);
      shv_tree_root->children.mode = mode;
    }

  /* Create children for SHV input and output blocks */

  item_in = shv_tree_node_new("inputs", &shv_dir_ls_dmap, mode);
  if (item_in == NULL)
    {
      printf("ERROR: Failed to allocate memory for SHV tree block \"inputs\"!");
      return;
    }

  shv_tree_add_child(shv_tree_root, item_in);

  item_out = shv_tree_node_new("outputs", &shv_dir_ls_dmap, mode);
  if (item_out == NULL)
    {
      printf("ERROR: Failed to allocate memory for SHV tree block \"outputs\"!");
      return;
    }

  shv_tree_add_child(shv_tree_root, item_out);

  /* Create a child for common blocks */

  item_blocks = shv_tree_node_new("blocks", &shv_dir_ls_dmap, mode);
  if (item_blocks == NULL)
    {
      printf("ERROR: Failed to allocate memory for SHV tree block \"blocks\"!");
      return;
    }

  shv_tree_add_child(shv_tree_root, item_blocks);

  /* For each block */

  for (int i = 0; i < n_blocks; i++)
    {
      int index = block_map->blocks[i].block_idx;
      const char *blk_name = block_map->blocks[i].block_name;

      /* If block has a real parameter */

      if ((block_map->blocks[i].system_outputs == 0) && (block_map->blocks[i].system_inputs == 0))
        {
          shv_node_t *item_blk = shv_tree_node_new(blk_name, &shv_blk_dmap, mode);
          if (item_blk == NULL)
            {
              printf("ERROR: Failed to allocate memory for SHV tree block \"%s\"!", blk_name);
              continue;
            }

          shv_tree_add_child(item_blocks, item_blk);

          shv_add_block(block_map, index, item_blk, mode);
        }

      /* If block has editable outputs
       *  - there is kind of mind twist: editable outputs = user can change system inputs
       */

      if (block_map->blocks[i].system_inputs == 1)
        {
          shv_node_t *item_blk = shv_tree_node_new(blk_name, &shv_blk_dmap, mode);
          if (item_blk == NULL)
            {
              printf("ERROR: Failed to allocate memory for SHV tree block \"%s\"!", blk_name);
              return;
            }

          shv_tree_add_child(item_in, item_blk);

          shv_add_system_in(block_map, index, item_blk, mode);
        }

      /* If block has editable inputs */

      if (block_map->blocks[i].system_outputs == 1)
        {
          shv_node_t *item_blk = shv_tree_node_new(blk_name, &shv_blk_dmap, mode);
          if (item_blk == NULL)
            {
              printf("ERROR: Failed to allocate memory for SHV tree block \"%s\"!", blk_name);
              return;
            }

          shv_tree_add_child(item_out, item_blk);

          shv_add_system_out(block_map, index, item_blk, mode);
        }
    }
}

/****************************************************************************
 Functions to keep intermediate version working with new shv-libs4c
 ****************************************************************************/

static void shv_my_at_signlr(struct shv_con_ctx *ctx, enum shv_attention_reason r)
{
}

/****************************************************************************
 * Name: shv_tree_init
 *
 * Description:
 *  Entry point for SHV related operations. Calls shv_tree_create to create
 *  a SHV tree and then initialize SHV connection.
 *
 ****************************************************************************/

shv_con_ctx_t *shv_tree_init(python_block_name_map * block_map,
                             const shv_node_t *static_root, int mode)
{
  int ret;
  const shv_node_t *root;
  shv_attention_signaller at_signlr = shv_my_at_signlr;
  static struct shv_connection shv_conn;
  uint16_t port;
  char *shv_broker_ip;
  char *shv_broker_port;

  shv_broker_ip = getenv("SHV_BROKER_IP");
  if (!shv_broker_ip)
    {
      printf("Unable to get SHV_BROKER_IP env variable.");
      return NULL;
    }

  shv_broker_port = getenv("SHV_BROKER_PORT");
  if (!shv_broker_port)
    {
      printf("Unable to get SHV_BROKER_PORT env variable.");
      return NULL;
    }
  else
    {
      port = atoi(shv_broker_port);
    }

  if ((mode & SHV_NLIST_MODE_STATIC) == 0)
    {
      /* Create tree only if it should be allocated dynamically */

      root = shv_tree_node_new(NULL, &shv_root_dmap, mode);
      if (root == NULL)
        {
          printf("ERROR: malloc() failed\n");
          return NULL;
        }
      shv_tree_create(block_map, (shv_node_t *)root, mode);
    }
  else
    {
      root = static_root;
    }

  /* Initialize SHV connection */
  shv_connection_init(&shv_conn, SHV_TLAYER_TCPIP);
  shv_conn.broker_user = getenv("SHV_BROKER_USER");
  shv_conn.broker_password = getenv("SHV_BROKER_PASSWORD");
  shv_conn.broker_mount = getenv("SHV_BROKER_MOUNT");
  shv_conn.device_id = getenv("SHV_BROKER_DEV_ID");
  shv_conn.reconnect_period = 10;
  shv_conn.reconnect_retries = 0;
  shv_connection_tcpip_init(&shv_conn, shv_broker_ip, port);

  struct shv_con_ctx *ctx = shv_com_init((struct shv_node *)root, &shv_conn, at_signlr);
  if (ctx == NULL)
    {
      printf("ERROR: shv_init() failed.\n");
    }

  ret = shv_create_process_thread(get_priority_for_com(), ctx);
  if (ret < 0)
    {
      printf("ERROR: %s\n", shv_errno_str(ctx));
    }

  return ctx;
}

/****************************************************************************
 * Name: shv_tree_end
 *
 * Description:
 *  End function for SHV tree. This should be called before blocks's end
 *  functions are called. This destroys the SHV tree, free memory and
 *  terminates TCP communication.
 *
 ****************************************************************************/

void shv_tree_end(shv_con_ctx_t *ctx, int mode)
{
  struct shv_node *root;

  if (ctx == NULL)
    {
      return;
    }

  root = ctx->root;

  /* Terminate the SHV connection */

  shv_com_destroy(ctx);

  /* And destroy SHV tree */
  if ((root->children.mode & SHV_NLIST_MODE_STATIC) == 0)
    {
      shv_tree_destroy(root);
    }
}
