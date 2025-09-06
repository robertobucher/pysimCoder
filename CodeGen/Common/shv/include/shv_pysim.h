#ifndef SHV_PYSIM_H
#define SHV_PYSIM_H

#include <ulut/ul_gavlcust.h>
#include <ulut/ul_gsacust.h>
#include <shv/chainpack/cchainpack.h>
#include <string.h>

#include <shv/tree/shv_tree.h>
#include <shv/tree/shv_connection.h>
#include <shv/tree/shv_com.h>
#include <shv/tree/shv_dotdevice_node.h>
#include <shv_fwstable_node.h>
#include <pyblock.h>

/* TBD in the future: The Communication tree structure is as follows:
 * (to allow communication with multiple models and its centralized management)
 *
 * SHV Tree Root
 * |- .device         // a node that is part of SHV specification
 * |- fwUpdate        // a file node used to do firmware updates
 * |- fwStable        // a node used to confirm a newly updated image
 * |- models          // the node tree of models
 *    |- model1
 *       |- manager   // a model's manager used to control its execution
 *       |- blocks    // a tree of model's blocks
 *       |- inputs    // a tree of model's inputs
 *       |- outputs   // a tree of model's outputs
 *    |- model2
 *       |- manager
 *       |- blocks
 *       |- inputs
 *       |- outputs
 *    ...
 *    |- modelN
 *       ...
 */

/* A SHV node used by the model's manager */
struct shv_node_model_ctx {
  struct shv_node shv_node;          /* Node instance */
  struct pysim_model_ctx *model_ctx; /* A pointer to the model's context, needed for interaction */
};

struct shv_node_model_ctx *shv_node_model_ctx_new(const char *child_name,
                                                  const struct shv_dmap *dir,
                                                  int mode);

struct shv_con_ctx *shv_tree_init(python_block_name_map * block_map,
                                  const struct shv_node *static_root, int mode,
                                  struct shv_connection *conn,
                                  shv_attention_signaller at_signlr,
                                  struct shv_file_node *fwupdate_node,
                                  struct shv_dotdevice_node *dotdevice_node,
                                  struct shv_fwstable_node *fwstable_node);

void shv_tree_end(struct shv_con_ctx *ctx, int mode);

extern const struct shv_dmap shv_blk_dmap;

#endif /* SHV_PYSIM_H */
