/*
  COPYRIGHT (C) 2025  Stepan Pressl (press.stepan@gmail.com)

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
 */

#include <pyblock.h>
#include <shv_pysim.h>
#include <shv_manager_node.h>

#include <shv/tree/shv_tree.h>
#include <shv/tree/shv_methods.h>
#include <shv/tree/shv_com.h>
#include <ulut/ul_utdefs.h>

static int shv_pausectrl(shv_con_ctx_t *shv_ctx, shv_node_t *item, int rid)
{
    shv_unpack_data(&shv_ctx->unpack_ctx, 0, 0);
    struct shv_node_model_ctx *item_node = UL_CONTAINEROF(item, struct shv_node_model_ctx,
                                                          shv_node);
    struct pysim_model_ctx *mctx = item_node->model_ctx;
    if (mctx->pt_ops.pausectrl) {
        mctx->pt_ops.pausectrl(mctx->pt_arg);
        shv_send_empty_response(shv_ctx, rid);
        return 0;
    }
    shv_send_error(shv_ctx, rid, SHV_RE_METHOD_CALL_EXCEPTION, "Platform pausectrl cb not defined!");
    return -1;
}

static int shv_resumectrl(shv_con_ctx_t *shv_ctx, shv_node_t *item, int rid)
{
    shv_unpack_data(&shv_ctx->unpack_ctx, 0, 0);
    struct shv_node_model_ctx *item_node = UL_CONTAINEROF(item, struct shv_node_model_ctx,
                                                          shv_node);
    struct pysim_model_ctx *mctx = item_node->model_ctx;
    if (mctx->pt_ops.resumectrl) {
        mctx->pt_ops.resumectrl(mctx->pt_arg);
        shv_send_empty_response(shv_ctx, rid);
        return 0;
    }
    shv_send_error(shv_ctx, rid, SHV_RE_METHOD_CALL_EXCEPTION, "Platform resumectrl cb not defined!");
    return -1;
}

static int shv_getstate(shv_con_ctx_t *shv_ctx, shv_node_t *item, int rid)
{
    shv_unpack_data(&shv_ctx->unpack_ctx, 0, 0);
    struct shv_node_model_ctx *item_node = UL_CONTAINEROF(item, struct shv_node_model_ctx,
                                                          shv_node);
    struct pysim_model_ctx *mctx = item_node->model_ctx;
    if (mctx->pt_ops.getctrlstate) {
        shv_send_int(shv_ctx, rid, mctx->pt_ops.getctrlstate(mctx->pt_arg));
        return 0;
    }
    shv_send_error(shv_ctx, rid, SHV_RE_METHOD_CALL_EXCEPTION, "Platform getstate cb not defined!");
    return -1;
}

static const shv_method_des_t shv_dmap_item_pausectrl =
{
  .name = "pause",
  .flags = 0,
  .result = "",
  .access = SHV_ACCESS_COMMAND,
  .method = shv_pausectrl
};

static const shv_method_des_t shv_dmap_item_resumectrl =
{
  .name = "resume",
  .flags = 0,
  .result = "",
  .access = SHV_ACCESS_COMMAND,
  .method = shv_resumectrl
};

static const shv_method_des_t shv_dmap_item_getctrlstate =
{
  .name = "getstate",
  .flags = SHV_METHOD_GETTER,
  .result = "i",
  .access = SHV_ACCESS_READ,
  .method = shv_getstate
};

static const shv_method_des_t * const shv_manager_dmap_items[] =
{
  &shv_dmap_item_dir,
  &shv_dmap_item_getctrlstate,
  &shv_dmap_item_ls,
  &shv_dmap_item_pausectrl,
  &shv_dmap_item_resumectrl
};

const shv_dmap_t shv_manager_dmap = 
{
  .methods =
  {
    .items = (void **)shv_manager_dmap_items,
    .count = sizeof(shv_manager_dmap_items)/sizeof(shv_manager_dmap_items[0]),
    .alloc_count = 0,
  }
};
