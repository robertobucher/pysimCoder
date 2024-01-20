#ifndef SHV_METHODS_H
#define SHV_METHODS_H

#include <ulut/ul_gavlcust.h>
#include <ulut/ul_gsacust.h>
#include <shv/chainpack/cchainpack.h>
#include <string.h>

#include "shv_com.h"
#include "shv_tree.h"

extern const shv_method_des_t shv_dmap_item_ls;
extern const shv_method_des_t shv_dmap_item_dir;

extern const shv_dmap_t shv_double_dmap;
extern const shv_dmap_t shv_double_read_only_dmap;
extern const shv_dmap_t shv_dir_ls_dmap;
extern const shv_dmap_t shv_root_dmap;

int shv_ls(shv_con_ctx_t * shv_ctx, shv_node_t* item, int rid);
int shv_dir(shv_con_ctx_t * shv_ctx, shv_node_t* item, int rid);
int shv_type(shv_con_ctx_t * shv_ctx, shv_node_t* item, int rid);
int shv_double_get(shv_con_ctx_t * shv_ctx, shv_node_t* item, int rid);
int shv_double_set(shv_con_ctx_t * shv_ctx, shv_node_t* item, int rid);

#endif /* SHV_METHODS_H */
