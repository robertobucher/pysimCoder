/*
  COPYRIGHT (C) 2025  Stepan Pressl (pressl.stepan@gmail.com)

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
#include <shv_pysim.h>
#include <shv_fwstable_node.h>
#include <shv/tree/shv_methods.h>
#include <shv/tree/shv_tree.h>
#include <ulut/ul_utdefs.h>

#include <stdlib.h>

static int shv_confirm(struct shv_con_ctx *shv_ctx, struct shv_node *item, int rid)
{
    shv_unpack_data(&shv_ctx->unpack_ctx, 0, 0);
    struct shv_fwstable_node *item_node = UL_CONTAINEROF(item, struct shv_fwstable_node,
                                                         shv_node);
    if (item_node->ops.confirm) {
        item_node->ops.confirm();
        shv_send_empty_response(shv_ctx, rid);
        return 0;
    }
    shv_send_error(shv_ctx, rid, SHV_RE_METHOD_CALL_EXCEPTION, "Platform confirm cb not defined!");
    return -1;
}

static int shv_get(struct shv_con_ctx *shv_ctx, struct shv_node *item, int rid)
{
    shv_unpack_data(&shv_ctx->unpack_ctx, 0, 0);
    struct shv_fwstable_node *item_node = UL_CONTAINEROF(item, struct shv_fwstable_node,
                                                         shv_node);
    if (item_node->ops.get) {
        shv_send_int(shv_ctx, rid, item_node->ops.get());
        return 0;
    }
    shv_send_error(shv_ctx, rid, SHV_RE_METHOD_CALL_EXCEPTION, "Platform get cb not defined!");
    return -1;
}

static const struct shv_method_des shv_dmap_item_confirm =
{
    .name = "confirm",
    .flags = 0,
    .result = "",
    .access = SHV_ACCESS_COMMAND,
    .method = shv_confirm
};

static const struct shv_method_des shv_dmap_item_get =
{
   .name = "get",
   .flags = SHV_METHOD_GETTER,
   .result = "i",
   .access = SHV_ACCESS_READ,
   .method = shv_get
};

static const struct shv_method_des *const shv_fwstable_dmap_items[] =
{
    &shv_dmap_item_confirm,
    &shv_dmap_item_dir,
    &shv_dmap_item_get,
    &shv_dmap_item_ls
};

const struct shv_dmap shv_fwstable_dmap = SHV_CREATE_NODE_DMAP(fwstable, shv_fwstable_dmap_items);

static void _shv_fwstable_node_destructor(struct shv_node *this)
{
    struct shv_fwstable_node *item = UL_CONTAINEROF(this, struct shv_fwstable_node, shv_node);
    free(item);
}

struct shv_fwstable_node *shv_fwstable_node_new(const struct shv_dmap *dir, int mode)
{
    struct shv_fwstable_node *item = calloc(1, sizeof(struct shv_fwstable_node));
    if (item == NULL) {
        return NULL;
    }
    shv_tree_node_init(&item->shv_node, "fwStable", dir, mode);
    item->shv_node.vtable.destructor = _shv_fwstable_node_destructor;
    return item;
}
