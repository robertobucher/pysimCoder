"""
SHV C tree generation.
"""

from numpy import size
from os import environ
from supsisim.RCPblk import RcpParam
import copy
import typing

class ShvTreeGenerator:
    def __init__(self, f: typing.IO, model: str, blocks) -> None:
        self.f = f
        self.model: str = model
        self.blocks = blocks
        self.blocks_cnt = size(blocks)

        self.blocks_ordered = []
        for n in range(0, self.blocks_cnt):
            self.blocks_ordered.append(self.blocks[n].name)
        self.blocks_ordered.sort()

    def generate_header(self) -> None:
        text = "#ifdef CONF_SHV_USED\n"
        text += "#include <shv/tree/shv_connection.h>\n"
        text += "#include <shv/tree/shv_tree.h>\n"
        text += "#include <shv/tree/shv_methods.h>\n"
        text += "#include <shv/tree/shv_com.h>\n"
        text += "#include <shv/tree/shv_file_node.h>\n"
        text += "#include <shv/tree/shv_dotdevice_node.h>\n"
        text += "#include <ulut/ul_utdefs.h>\n\n"
        text += '#include "shv_pysim.h"\n'
        text += '#include "shv_manager_node.h"\n'
        text += '#include "shv_fwstable_node.h"\n'
        self.f.write(text)

        if environ["SHV_TREE_TYPE"] == "GSA":
            text = "#define CONF_SHV_TREE_TYPE 1\n"
            text += "#undef CONF_SHV_TREE_STATIC\n\n"
        elif environ["SHV_TREE_TYPE"] == "GSA_STATIC":
            text = "#define CONF_SHV_TREE_TYPE 3\n"
            text += "#define CONF_SHV_TREE_STATIC 1\n\n"
        else:
            text = "#define CONF_SHV_TREE_TYPE 0\n"
            text += "#undef CONF_SHV_TREE_STATIC\n"

        text += "#endif /* CONF_SHV_USED */\n\n"
        self.f.write(text)

        # It gets a bit complicated, due to the file update node.
        # As the main should provide the shv_init_fwupdate function due to platform differences,
        # we expect the fwupdate node to be dynamic, regardless of the whole tree type.
        # Thus in case of fwupdate and GSA being used, the whole tree is built
        # as const rodata, but the file node is always built and initialized dynamically.
        # So we need to modifier to determine whether the root is const or not.

        if environ["SHV_USED"] == "True":
            self.f.write("/* SHV related function and structures */\n\n")
            self.f.write("#ifdef CONF_SHV_USED\n")
            text = "#ifdef CONF_SHV_UPDATES_USED\n"
            text += "#define ROOT_NODE_CONST\n"
            text += "#else\n"
            text += "#define ROOT_NODE_CONST const\n"
            text += "#endif /* CONF_SHV_UPDATES_USED */\n\n"
            self.f.write(text)

            text = "/********** SHV Function Declarations **********/\n"
            text += "int shv_init_fwupdate(struct pysim_platform_model_ctx *ctx, struct shv_file_node *item);\n"
            text += "int shv_init_fwstable(struct pysim_platform_model_ctx *ctx, struct shv_fwstable_node *item);\n"
            text += "\n"
            self.f.write(text)

            text = "/********** SHV Private Data **********/\n"
            text += "python_block_name_map block_name_map_" + self.model + ";\n"
            text += (
                "python_block_name_entry block_name_entry_"
                + self.model
                + "["
                + str(self.blocks_cnt)
                + "];\n"
            )
            text += "static struct shv_con_ctx *" + self.model + "_shv_ctx;\n"
            text += "static struct shv_connection shv_conn;\n"
            text += "#endif /* CONF_SHV_USED */\n\n"
            self.f.write(text)

    def generate_tree(self) -> None:
        self.f.write("#ifdef CONF_SHV_TREE_STATIC\n")

        has_system_inputs = False
        has_system_outputs = False
        has_blocks = False

        blks_names = []
        for i in range(0, self.blocks_cnt):
            blks_names.append(self.blocks[i].name)

        for n in range(0, self.blocks_cnt):
            has_real_pars = False
            has_inputs = False
            has_outputs = False
            index = blks_names.index(self.blocks_ordered[n])

            text = ""
            if (self.blocks[index].fcn != "shv_input") and (
                self.blocks[index].fcn != "shv_output"
            ):
                has_blocks = True
                # Get all the real parameters
                real_params = [param for param in self.blocks[index].params_list \
                               if param.type == RcpParam.Type.DOUBLE]
                real_params_cnt = len(real_params)

                if real_params_cnt != 0:
                    has_real_pars = True
                    real_par_names = [param.name for param in real_params]

                    # All params have a unique name. So no new name creations are needed.
                    real_par_names_ordered = copy.deepcopy(real_par_names)
                    real_par_names_ordered.sort()

                    for i in range(0, real_params_cnt):
                        indexPar = real_par_names.index(real_par_names_ordered[i])
                        text += (
                            "const struct shv_node_typed_val shv_node_typed_val_blk"
                            + str(n)
                            + "_par"
                            + str(i)
                            + " = {\n"
                            + '   .shv_node = {.name = "'
                            + str(real_par_names[indexPar])
                            + '",\n'
                            + "            .dir  = UL_CAST_UNQ1(struct shv_dmap *, &shv_double_dmap),\n"
                            + "           },\n"
                            + "   .val_ptr = &realPar_"
                            + str(index)
                            + "["
                            + str(indexPar)
                            + "],\n"
                            + '   .type_name = "double",\n};\n\n'
                        )
                    self.f.write(text)
                    text = (
                        "const struct shv_node_typed_val *const shv_node_typed_val_blk"
                        + str(n)
                        + "_pars"
                        + "[] = {\n"
                    )
                    for i in range(0, real_params_cnt):
                        text += (
                            "  &shv_node_typed_val_blk"
                            + str(n)
                            + "_par"
                            + str(i)
                            + ",\n"
                        )

                    text += "};\n\n"
                    self.f.write(text)

                text = (
                    "const struct shv_node shv_node_blk"
                    + str(n)
                    + "_par = {\n"
                    + '   .name = "parameters",\n'
                    + "   .dir  = UL_CAST_UNQ1(struct shv_dmap *, &shv_blk_dmap),\n"
                    + "   .children = {.mode = CONF_SHV_TREE_TYPE,\n"
                )
                if has_real_pars:
                    text += (
                        "                .list = {.gsa = {.root = {\n"
                        + "                      .items = (void **)shv_node_typed_val_blk"
                        + str(n)
                        + "_pars,\n"
                        + "                      .count = sizeof(shv_node_typed_val_blk"
                        + str(n)
                        + "_pars)/sizeof(shv_node_typed_val_blk"
                        + str(n)
                        + "_pars[0]),\n"
                        + "                      .alloc_count = 0,}\n"
                        + "}}"
                    )
                text += "}};\n\n"
                self.f.write(text)

                if size(self.blocks[index].pin) != 0:
                    has_inputs = True
                    text = ""
                    for i in range(0, size(self.blocks[index].pin)):
                        text += (
                            "const struct shv_node_typed_val shv_node_typed_val_blk"
                            + str(n)
                            + "_in"
                            + str(i)
                            + " = {\n"
                            + '   .shv_node = {.name = "input'
                            + str(i)
                            + '",\n'
                            + "            .dir  = UL_CAST_UNQ1(struct shv_dmap *, &shv_double_read_only_dmap),\n"
                            + "           },\n"
                            + "   .val_ptr = Node_"
                            + str(self.blocks[index].pin[i])
                            + ",\n"
                            + '   .type_name = "double",\n};\n\n'
                        )
                    self.f.write(text)
                    text = (
                        "const struct shv_node_typed_val *const shv_node_typed_val_blk"
                        + str(n)
                        + "_ins"
                        + "[] = {\n"
                    )
                    for i in range(0, size(self.blocks[index].pin)):
                        text += (
                            "  &shv_node_typed_val_blk"
                            + str(n)
                            + "_in"
                            + str(i)
                            + ",\n"
                        )
                    text += "};\n\n"
                    self.f.write(text)

                text = (
                    "const struct shv_node shv_node_blk"
                    + str(n)
                    + "_in = {\n"
                    + '   .name = "inputs",\n'
                    + "   .dir  = UL_CAST_UNQ1(struct shv_dmap *, &shv_blk_dmap),\n"
                    + "   .children = {.mode = CONF_SHV_TREE_TYPE,\n"
                )
                if has_inputs:
                    text += (
                        "                .list = {.gsa = {.root = {\n"
                        + "                      .items = (void **)shv_node_typed_val_blk"
                        + str(n)
                        + "_ins,\n"
                        + "                      .count = sizeof(shv_node_typed_val_blk"
                        + str(n)
                        + "_ins)/sizeof(shv_node_typed_val_blk"
                        + str(n)
                        + "_ins[0]),\n"
                        + "                      .alloc_count = 0,}\n"
                        + "}}"
                    )
                text += "}};\n\n"
                self.f.write(text)

                if size(self.blocks[index].pout) != 0:
                    has_outputs = True
                    text = ""
                    for i in range(0, size(self.blocks[index].pout)):
                        text += (
                            "const struct shv_node_typed_val shv_node_typed_val_blk"
                            + str(n)
                            + "_out"
                            + str(i)
                            + " = {\n"
                            + '   .shv_node = {.name = "output'
                            + str(i)
                            + '",\n'
                            + "            .dir  = UL_CAST_UNQ1(struct shv_dmap *, &shv_double_read_only_dmap),\n"
                            + "           },\n"
                            + "   .val_ptr = Node_"
                            + str(self.blocks[index].pout[i])
                            + ",\n"
                            + '   .type_name = "double",\n};\n\n'
                        )
                    self.f.write(text)
                    text = (
                        "const struct shv_node_typed_val *const shv_node_typed_val_blk"
                        + str(n)
                        + "_outs"
                        + "[] = {\n"
                    )
                    for i in range(0, size(self.blocks[index].pout)):
                        text += (
                            "  &shv_node_typed_val_blk"
                            + str(n)
                            + "_out"
                            + str(i)
                            + ",\n"
                        )
                    text += "};\n\n"
                    self.f.write(text)

                text = (
                    "const struct shv_node shv_node_blk"
                    + str(n)
                    + "_out = {\n"
                    + '   .name = "outputs",\n'
                    + "   .dir  = UL_CAST_UNQ1(struct shv_dmap *, &shv_blk_dmap),\n"
                    + "   .children = {.mode = CONF_SHV_TREE_TYPE,\n"
                )
                if has_outputs:
                    text += (
                        "                .list = {.gsa = {.root = {\n"
                        + "                      .items = (void **)shv_node_typed_val_blk"
                        + str(n)
                        + "_outs,\n"
                        + "                      .count = sizeof(shv_node_typed_val_blk"
                        + str(n)
                        + "_outs)/sizeof(shv_node_typed_val_blk"
                        + str(n)
                        + "_outs[0]),\n"
                        + "                      .alloc_count = 0,}\n"
                        + "}}"
                    )
                text += "}};\n\n"
                self.f.write(text)

                text = (
                    "const struct shv_node *const shv_node_blk"
                    + str(n)
                    + "_items"
                    + "[] = {\n"
                )
                text += "  &shv_node_blk" + str(n) + "_in,\n"
                text += "  &shv_node_blk" + str(n) + "_out,\n"
                text += "  &shv_node_blk" + str(n) + "_par\n"
                text += "};\n\n"
                self.f.write(text)

                text = (
                    "const struct shv_node shv_node_blk"
                    + str(n)
                    + " = {\n"
                    + '   .name = "'
                    + str(self.blocks[index].name)
                    + '",\n'
                    + "   .dir  = UL_CAST_UNQ1(struct shv_dmap *, &shv_blk_dmap),\n"
                    + "   .children = {.mode = CONF_SHV_TREE_TYPE,\n"
                    + "                .list = {.gsa = {.root = {\n"
                    + "                      .items = (void **)shv_node_blk"
                    + str(n)
                    + "_items,\n"
                    + "                      .count = sizeof(shv_node_blk"
                    + str(n)
                    + "_items)/sizeof(shv_node_blk"
                    + str(n)
                    + "_items[0]),\n"
                    + "                      .alloc_count = 0,}\n"
                    + "}}}};\n\n"
                )
                self.f.write(text)

            text = ""
            if self.blocks[index].fcn == "shv_input":
                has_system_inputs = True
                # we have editable outputs (SHV input block)
                for i in range(0, size(self.blocks[index].pout)):
                    text += (
                        "const struct shv_node_typed_val shv_node_typed_val_blk"
                        + str(n)
                        + "_sysIn"
                        + str(i)
                        + " = {\n"
                        + '   .shv_node = {.name = "input'
                        + str(i)
                        + '",\n'
                        + "            .dir  = UL_CAST_UNQ1(struct shv_dmap *, &shv_double_dmap),\n"
                        + "           },\n"
                        + "   .val_ptr = Node_"
                        + str(self.blocks[index].pout[i])
                        + ",\n"
                        + '   .type_name = "double",\n};\n\n'
                    )
                self.f.write(text)
                text = (
                    "const struct shv_node_typed_val *const shv_node_typed_val_blk"
                    + str(n)
                    + "_sysIns"
                    + "[] = {\n"
                )
                for i in range(0, size(self.blocks[index].pout)):
                    text += (
                        "  &shv_node_typed_val_blk" + str(n) + "_sysIn" + str(i) + ",\n"
                    )

                text += "};\n\n"
                self.f.write(text)

                text = (
                    "const struct shv_node shv_node_blk"
                    + str(n)
                    + "_sysIns = {\n"
                    + '   .name = "'
                    + str(self.blocks[index].name)
                    + '",\n'
                    + "   .dir  = UL_CAST_UNQ1(struct shv_dmap *, &shv_blk_dmap),\n"
                    + "   .children = {.mode = CONF_SHV_TREE_TYPE,\n"
                    + "                .list = {.gsa = {.root = {\n"
                    + "                      .items = (void **)shv_node_typed_val_blk"
                    + str(n)
                    + "_sysIns,\n"
                    + "                      .count = sizeof(shv_node_typed_val_blk"
                    + str(n)
                    + "_sysIns)/sizeof(shv_node_typed_val_blk"
                    + str(n)
                    + "_sysIns[0]),\n"
                    + "                      .alloc_count = 0,}\n"
                    + "}}}};\n\n"
                )
                self.f.write(text)

            if self.blocks[index].fcn == "shv_output":
                has_system_outputs = True
                # we have editable inputs (SHV output block)
                for i in range(0, size(self.blocks[index].pin)):
                    text += (
                        "const struct shv_node_typed_val shv_node_typed_val_blk"
                        + str(n)
                        + "_sysOut"
                        + str(i)
                        + " = {\n"
                        + '   .shv_node = {.name = "output'
                        + str(i)
                        + '",\n'
                        + "            .dir  = UL_CAST_UNQ1(struct shv_dmap *, &shv_double_read_only_dmap),\n"
                        + "           },\n"
                        + "   .val_ptr = Node_"
                        + str(self.blocks[index].pin[i])
                        + ",\n"
                        + '   .type_name = "double",\n};\n\n'
                    )
                self.f.write(text)
                text = (
                    "const struct shv_node_typed_val *const shv_node_typed_val_blk"
                    + str(n)
                    + "_sysOuts"
                    + "[] = {\n"
                )
                for i in range(0, size(self.blocks[index].pin)):
                    text += (
                        "  &shv_node_typed_val_blk"
                        + str(n)
                        + "_sysOut"
                        + str(i)
                        + ",\n"
                    )

                text += "};\n\n"
                self.f.write(text)

                text = (
                    "const struct shv_node shv_node_blk"
                    + str(n)
                    + "_sysOuts = {\n"
                    + '   .name = "'
                    + str(self.blocks[index].name)
                    + '",\n'
                    + "   .dir  = UL_CAST_UNQ1(struct shv_dmap *, &shv_blk_dmap),\n"
                    + "   .children = {.mode = CONF_SHV_TREE_TYPE,\n"
                    + "                .list = {.gsa = {.root = {\n"
                    + "                      .items = (void **)shv_node_typed_val_blk"
                    + str(n)
                    + "_sysOuts,\n"
                    + "                      .count = sizeof(shv_node_typed_val_blk"
                    + str(n)
                    + "_sysOuts)/sizeof(shv_node_typed_val_blk"
                    + str(n)
                    + "_sysOuts[0]),\n"
                    + "                      .alloc_count = 0,}\n"
                    + "}}}};\n\n"
                )
                self.f.write(text)

        if has_blocks:
            text = "const struct shv_node *const shv_node_blks_list[] = {\n"
            for n in range(0, self.blocks_cnt):
                index = blks_names.index(self.blocks_ordered[n])
                if (self.blocks[index].fcn != "shv_input") and (
                    self.blocks[index].fcn != "shv_output"
                ):
                    text += "  &shv_node_blk" + str(n) + ",\n"
            text += "};\n\n"
            self.f.write(text)

        text = (
            "const struct shv_node shv_node_blks = {\n"
            + '   .name = "blocks",\n'
            + "   .dir = UL_CAST_UNQ1(struct shv_dmap *, &shv_root_dmap),\n"
            + "   .children = {.mode = CONF_SHV_TREE_TYPE,\n"
        )
        if has_blocks:
            text += (
                "                .list = {.gsa = {.root = {\n"
                + "                      .items = (void **)shv_node_blks_list,\n"
                + "                      .count = sizeof(shv_node_blks_list)/sizeof(shv_node_blks_list[0]),\n"
                + "                      .alloc_count = 0,}\n"
                + "}}"
            )
        text += "}};\n\n"
        self.f.write(text)

        if has_system_inputs:
            text = "const struct shv_node *const shv_node_blks_inputs[] = {\n"
            for n in range(0, self.blocks_cnt):
                index = blks_names.index(self.blocks_ordered[n])
                if (size(self.blocks[index].pout) != 0) and (
                    self.blocks[index].fcn == "shv_input"
                ):
                    text += "  &shv_node_blk" + str(n) + "_sysIns,\n"
            text += "};\n\n"
            self.f.write(text)

        text = (
            "const struct shv_node shv_node_inputs = {\n"
            + '   .name = "inputs",\n'
            + "   .dir = UL_CAST_UNQ1(struct shv_dmap *, &shv_root_dmap),\n"
            + "   .children = {.mode = CONF_SHV_TREE_TYPE,\n"
        )
        if has_system_inputs:
            text += (
                "                .list = {.gsa = {.root = {\n"
                + "                      .items = (void **)shv_node_blks_inputs,\n"
                + "                      .count = sizeof(shv_node_blks_inputs)/sizeof(shv_node_blks_inputs[0]),\n"
                + "                      .alloc_count = 0,}\n"
                + "}}"
            )
        text += "}};\n\n"
        self.f.write(text)

        if has_system_outputs:
            text = "const struct shv_node *const shv_node_blks_outputs[] = {\n"
            for n in range(0, self.blocks_cnt):
                index = blks_names.index(self.blocks_ordered[n])
                if (size(self.blocks[index].pin) != 0) and (
                    self.blocks[index].fcn == "shv_output"
                ):
                    text += "  &shv_node_blk" + str(n) + "_sysOuts,\n"
            text += "};\n\n"
            self.f.write(text)

        text = (
            "const struct shv_node shv_node_outputs = {\n"
            + '   .name = "outputs",\n'
            + "   .dir = UL_CAST_UNQ1(struct shv_dmap *, &shv_root_dmap),\n"
            + "   .children = {.mode = CONF_SHV_TREE_TYPE,\n"
        )
        if has_system_outputs:
            text += (
                "                .list = {.gsa = {.root = {\n"
                + "                      .items = (void **)shv_node_blks_outputs,\n"
                + "                      .count = sizeof(shv_node_blks_outputs)/sizeof(shv_node_blks_outputs[0]),\n"
                + "                      .alloc_count = 0,}\n"
                + "}}"
            )
        text += "}};\n\n"
        self.f.write(text)

        # Generate the manager node, in case of GSA_STATIC
        text = (
            "const struct shv_node_model_ctx shv_node_manager = {\n" +
            "    .shv_node = {\n" +
            '        .name = "manager",\n'
            "        .dir = UL_CAST_UNQ1(struct shv_dmap *, &shv_manager_dmap),\n" +
            "        .children = { .mode = CONF_SHV_TREE_TYPE }\n" +
            "    },\n" +
            "    .model_ctx = &" + self.model + "_ctx\n" +
            "};\n\n"
        )
        self.f.write(text)

        text = (
            "const struct shv_node *const shv_tree_root_items[] = {\n"
            + "  &shv_node_blks,\n"
            + "  &shv_node_inputs,\n"
            + "  &shv_node_manager.shv_node,\n"
            + "  &shv_node_outputs,\n};\n\n"
        )
        self.f.write(text)

        text = (
            "ROOT_NODE_CONST struct shv_node shv_tree_root = {\n"
            + "   .dir = UL_CAST_UNQ1(struct shv_dmap *, &shv_root_dmap),\n"
            + "   .children = {.mode = CONF_SHV_TREE_TYPE,\n"
            + "                .list = {.gsa = {.root = {\n"
            + "                      .items = (void **)shv_tree_root_items,\n"
            + "                      .count = sizeof(shv_tree_root_items)/sizeof(shv_tree_root_items[0]),\n"
            + "                      .alloc_count = 0,}\n"
            + "}}}};\n\n"
        )
        self.f.write(text)
        self.f.write("#endif /* CONF_SHV_TREE_STATIC */\n\n")

    def _generate_fwupdate_init(self) -> str:
        text  = "#ifdef CONF_SHV_UPDATES_USED\n"
        text += '  struct shv_file_node *shv_fwupdate_node = shv_tree_file_node_new("fwUpdate", &shv_file_node_dmap, CONF_SHV_TREE_TYPE);\n'
        text += "  if (shv_fwupdate_node == NULL)\n"
        text += "    return -1;\n"
        text += "  shv_init_fwupdate(&" + self.model + "_pt_ctx, shv_fwupdate_node);\n"
        text += "#else\n"
        text += "  struct shv_file_node *shv_fwupdate_node = NULL;\n"
        text += "#endif /* CONF_SHV_UPDATES_USED */\n"
        return text

    def _generate_dotdevice_init(self) -> str:
        text  = "  struct shv_dotdevice_node *shv_dotdevice_node = shv_tree_dotdevice_node_new(&shv_dotdevice_dmap, CONF_SHV_TREE_TYPE);\n"
        text += "  if (shv_dotdevice_node == NULL)\n"
        text += "    return -1;\n"
        text += "  /* Fill in the dotdevice parameters */\n"
        text += '  shv_dotdevice_node->name = "pysimCoder SHV compatible device";\n'
        text += '  shv_dotdevice_node->version = "0.1.0";\n'
        text += '  shv_dotdevice_node->serial_number = "0xDEADBEEF";\n'
        return text

    def _generate_fwstable_init(self) -> str:
        text  = "#ifdef CONF_SHV_UPDATES_USED\n"
        text += "  struct shv_fwstable_node *fwstable_node = shv_fwstable_node_new(&shv_fwstable_dmap, CONF_SHV_TREE_TYPE);\n"
        text += "  if (fwstable_node == NULL)\n"
        text += "    return -1;\n"
        text += "  /* Initialize the fwStable node */\n"
        text += "  shv_init_fwstable(&" + self.model + "_pt_ctx, fwstable_node);\n"
        text += "#else\n"
        text += "  struct shv_fwstable_node *fwstable_node = NULL;\n"
        text += "#endif /* CONF_SHV_UPDATES_USED */\n"
        return text

    def generate_init(self) -> None:
        text = "#ifdef CONF_SHV_USED\n"
        text += "int " + self.model + "_com_init(shv_attention_signaller at_signlr)\n"
        text += "{\n"
        text += "  /* Call shv_tree_init() to initialize SHV tree */\n"

        if environ["SHV_TREE_TYPE"] != "GSA_STATIC":
            text += "  const struct shv_node shv_tree_root = {};\n"

        text += '  shv_connection_init(&shv_conn, SHV_TLAYER_TCPIP);\n'
        text += '  shv_conn.broker_user = "' + environ["SHV_BROKER_USER"] + '";\n'
        text += '  shv_conn.broker_password = "' + environ["SHV_BROKER_PASSWORD"] + '";\n'
        text += '  shv_conn.broker_mount = "' + environ["SHV_BROKER_MOUNT"] + '";\n'
        text += '  shv_conn.device_id = "' + environ["SHV_BROKER_DEV_ID"] + '";\n'
        text += '  shv_conn.reconnect_period = 10;\n'
        text += '  shv_conn.reconnect_retries = 0;\n'
        text += '  shv_connection_tcpip_init(&shv_conn, "' + environ["SHV_BROKER_IP"] + \
                '", ' + environ["SHV_BROKER_PORT"] + ');\n\n'
        text += '  /* Fill in the pysim_model_ctx struct. REVISIT: do it somewhere else */\n'
        text += "  " + self.model + '_ctx.pt_arg = &' + self.model + '_pt_ctx;\n'
        text += (
            "  " + self.model + '_ctx.pt_ops.pausectrl = &' + self.model + '_pausectrl;\n' +
            "  " + self.model + '_ctx.pt_ops.resumectrl = &' + self.model + '_resumectrl;\n' +
            "  " + self.model + '_ctx.pt_ops.getctrlstate = &' + self.model + '_getctrlstate;\n'
            "  " + self.model + '_ctx.pt_ops.comprio = &' + self.model + '_comprio;\n'
        )
        text += "  block_name_map_" + self.model + ".model_ctx = &" + self.model + "_ctx;\n"
        self.f.write(text)
        # Generate the update node (file node) and .device node dynamically, regardless of the tree type.
        # The reason is that these nodes are platform dependant and require platform dependant init.
        self.f.write(self._generate_fwupdate_init())
        self.f.write(self._generate_dotdevice_init())
        self.f.write(self._generate_fwstable_init())
        # Now construct the whole tree, using all the dynamically instantiated nodes.
        text = (
            "  "
            + self.model
            + "_shv_ctx = shv_tree_init(&block_name_map_"
            + self.model
            + ", (const struct shv_node *) &shv_tree_root, CONF_SHV_TREE_TYPE, "
            + "&shv_conn, at_signlr, shv_fwupdate_node, shv_dotdevice_node, fwstable_node);\n"
        )
        text += "  if (" + self.model + "_shv_ctx == NULL)\n"
        text += "    return -1;\n"
        text += "  return 0;\n"
        text += "}\n"
        text += "#endif /* CONF_SHV_USED */\n\n"
        self.f.write(text)

    def generate_code(self) -> None:
        self.f.write("#ifdef CONF_SHV_USED\n")
        self.f.write("/* SHV structures definition */\n\n")

        blks_names = []
        for i in range(0, self.blocks_cnt):
            blks_names.append(self.blocks[i].name)

        text = ""
        for n in range(0, self.blocks_cnt):
            index = blks_names.index(self.blocks_ordered[n])
            text += (
                "  block_name_entry_"
                + self.model
                + "["
                + str(n)
                + '].block_name = "'
                + self.blocks_ordered[n]
                + '";\n'
            )
            text += (
                "  block_name_entry_"
                + self.model
                + "["
                + str(n)
                + "].block_idx = "
                + str(index)
                + ";\n"
            )
            if self.blocks[index].fcn == "shv_input":
                text += (
                    "  block_name_entry_"
                    + self.model
                    + "["
                    + str(n)
                    + "].system_inputs = "
                    + str(1)
                    + ";\n"
                )
            else:
                text += (
                    "  block_name_entry_"
                    + self.model
                    + "["
                    + str(n)
                    + "].system_inputs = "
                    + str(0)
                    + ";\n"
                )
            if self.blocks[index].fcn == "shv_output":
                text += (
                    "  block_name_entry_"
                    + self.model
                    + "["
                    + str(n)
                    + "].system_outputs = "
                    + str(1)
                    + ";\n"
                )
            else:
                text += (
                    "  block_name_entry_"
                    + self.model
                    + "["
                    + str(n)
                    + "].system_outputs = "
                    + str(0)
                    + ";\n"
                )

        text += "\n"
        self.f.write(text)

        text = (
            "  block_name_map_"
            + self.model
            + ".blocks_count = "
            + str(self.blocks_cnt)
            + ";\n"
        )
        text += (
            "  block_name_map_"
            + self.model
            + ".blocks = "
            + "block_name_entry_"
            + self.model
            + ";\n"
        )
        text += (
            "  block_name_map_"
            + self.model
            + ".block_structure = block_"
            + self.model
            + ";\n"
        )
        text += (
            "  block_name_map_"
            + self.model
            + ".model_ctx = &"
            + self.model
            + "_ctx;\n\n"
        )
        self.f.write(text)
        self.f.write("#endif /* CONF_SHV_USED */\n\n")

    def generate_end(self) -> None:
        text = "#ifdef CONF_SHV_USED\n"
        text += "void " + self.model + "_com_end(void)\n"
        text += "{\n"
        text += "  shv_tree_end(" + self.model + "_shv_ctx, CONF_SHV_TREE_TYPE);\n"
        text += "}\n"
        text += "#endif /* CONF_SHV_USED */\n\n"
        self.f.write(text)
