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

#include "shv_methods.h"
#include "ulut/ul_utdefs.h"

/* Method descriptors - general methods "ls" and "dir" */

const shv_method_des_t shv_dmap_item_ls = {.name = "ls", .method = shv_ls};
const shv_method_des_t shv_dmap_item_dir = {.name = "dir", .method = shv_dir};

/* Method descriptors - methods for parameters */

const shv_method_des_t shv_dmap_item_type = {.name = "typeName", .method = shv_type};

/* Method descriptors - methods for double values: params and inputs/outputs */

const shv_method_des_t shv_double_dmap_item_get = {.name = "get", .method = shv_double_get};
const shv_method_des_t shv_double_dmap_item_set = {.name = "set", .method = shv_double_set};

const shv_method_des_t * const shv_double_dmap_items[] = {
  &shv_dmap_item_dir,
  &shv_double_dmap_item_get,
  &shv_dmap_item_ls,
  &shv_double_dmap_item_set,
  &shv_dmap_item_type,
};

const shv_method_des_t * const shv_double_read_only_dmap_items[] = {
  &shv_dmap_item_dir,
  &shv_double_dmap_item_get,
  &shv_dmap_item_ls,
  &shv_dmap_item_type,
};

const shv_method_des_t * const shv_dir_ls_dmap_items[] = {
  &shv_dmap_item_dir,
  &shv_dmap_item_ls,
};

const shv_method_des_t * const shv_root_dmap_items[] = {
  &shv_dmap_item_dir,
  &shv_dmap_item_ls,
};

const shv_dmap_t shv_double_dmap = {.methods = {.items = (void **)shv_double_dmap_items,
                                                .count = sizeof(shv_double_dmap_items)/sizeof(shv_double_dmap_items[0]),
                                                .alloc_count = 0,
                                               }};
const shv_dmap_t shv_double_read_only_dmap = {.methods = {.items = (void **)shv_double_read_only_dmap_items,
                                              .count = sizeof(shv_double_read_only_dmap_items)/sizeof(shv_double_read_only_dmap_items[0]),
                                              .alloc_count = 0,
                                              }};
const shv_dmap_t shv_dir_ls_dmap = {.methods = {.items = (void **)shv_dir_ls_dmap_items,
                                                .count = sizeof(shv_dir_ls_dmap_items)/sizeof(shv_dir_ls_dmap_items[0]),
                                                .alloc_count = 0,
                                               }};

const shv_dmap_t shv_root_dmap = {.methods = {.items = (void **)shv_root_dmap_items,
                                              .count = sizeof(shv_root_dmap_items)/sizeof(shv_root_dmap_items[0]),
                                              .alloc_count = 0,
                                             }};

/****************************************************************************
 * Name: shv_ls
 *
 * Description:
 *   Method "ls". This methods returns the names of the node's children.
 *
 ****************************************************************************/

int shv_ls(shv_con_ctx_t * shv_ctx, shv_node_t* item, int rid)
{
  int count;
  shv_node_list_names_it_t names_it;

  shv_unpack_data(&shv_ctx->unpack_ctx, 0, 0);

  /* Get item's children count */

  count = shv_node_list_count(&item->children);

  /* Find each child */

  shv_node_list_names_it_init(&item->children, &names_it);

  /* And send it */

  shv_send_str_list_it(shv_ctx, rid, count, &names_it.str_it);

  return 0;
}

/****************************************************************************
 * Name: shv_dir
 *
 * Description:
 *   Method "dir". This method returns the methods supported by the node.
 *
 ****************************************************************************/

int shv_dir(shv_con_ctx_t * shv_ctx, shv_node_t* item, int rid)
{
  int met_num;
  int i;

  shv_unpack_data(&shv_ctx->unpack_ctx, 0, 0);

  /* Is the node a parameter? */

  met_num = item->dir->methods.count;

    {
      const char * str[met_num];

      for (i = 0; i < met_num; i++)
        {
          str[i] = shv_dmap_at(item->dir, i)->name;
        }

      shv_send_str_list(shv_ctx, rid, i, str);
    }

  return 0;
}

/****************************************************************************
 * Name: shv_type
 *
 * Description:
 *   Returns type name of the parameter (double, int, etc...)
 *
 ****************************************************************************/

int shv_type(shv_con_ctx_t * shv_ctx, shv_node_t* item, int rid)
{
  const char *str;

  shv_unpack_data(&shv_ctx->unpack_ctx, 0, 0);

  shv_node_typed_val_t *item_node = UL_CONTAINEROF(item, shv_node_typed_val_t,
                                                   shv_node);

  str = item_node->type_name;

  shv_send_str(shv_ctx, rid, str);

  return 0;
}

/****************************************************************************
 * Name: shv_set
 *
 * Description:
 *   Method "set".
 *
 ****************************************************************************/

int shv_double_set(shv_con_ctx_t * shv_ctx, shv_node_t* item, int rid)
{
  double shv_received;

  shv_unpack_data(&shv_ctx->unpack_ctx, 0, &shv_received);

  shv_node_typed_val_t *item_node = UL_CONTAINEROF(item, shv_node_typed_val_t,
                                                   shv_node);

  *(double *)item_node->val_ptr = shv_received;

  shv_send_double(shv_ctx, rid, shv_received);

  return 0;
}


/****************************************************************************
 * Name: shv_get
 *
 * Description:
 *   Method "get".
 *
 ****************************************************************************/

int shv_double_get(shv_con_ctx_t * shv_ctx, shv_node_t* item, int rid)
{
  double shv_send;

  shv_unpack_data(&shv_ctx->unpack_ctx, 0, 0);

  shv_node_typed_val_t *item_node = UL_CONTAINEROF(item, shv_node_typed_val_t,
                                                   shv_node);

  shv_send = *(double *)item_node->val_ptr;

  shv_send_double(shv_ctx, rid, shv_send);

  return 0;
}
