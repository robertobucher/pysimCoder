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
        if environ["SHV_USED"] == "True":
            text: str = "#define CONF_SHV_USED 1\n\n"
        else:
            text = "#undef CONF_SHV_USED\n\n"
        self.f.write(text)

        text = "#ifdef CONF_SHV_USED\n"
        text += "#include <shv_tree.h>\n"
        text += "#include <shv_pysim.h>\n"
        text += "#include <shv_methods.h>\n"
        text += "#include <shv_com.h>\n"
        text += "#include <ulut/ul_utdefs.h>\n\n"
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

        if environ["SHV_USED"] == "True":
            self.f.write("/* SHV related function and structres */\n\n")
            self.f.write("#ifdef CONF_SHV_USED\n")
            self.f.write(
                "shv_con_ctx_t *shv_tree_init(python_block_name_map * block_map, const shv_node_t *static_root, int mode);\n"
            )
            self.f.write("void shv_tree_end(shv_con_ctx_t *ctx, int mode);\n\n")

            text = "python_block_name_map block_name_map_" + self.model + ";\n"
            text += (
                "python_block_name_entry block_name_entry_"
                + self.model
                + "["
                + str(self.blocks_cnt)
                + "];\n"
            )
            text += "static shv_con_ctx_t *" + self.model + "_ctx;\n"
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
                            "const shv_node_typed_val_t shv_node_typed_val_blk"
                            + str(n)
                            + "_par"
                            + str(i)
                            + " = {\n"
                            + '   .shv_node = {.name = "'
                            + str(real_par_names[indexPar])
                            + '",\n'
                            + "            .dir  = UL_CAST_UNQ1(shv_dmap_t *, &shv_double_dmap),\n"
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
                        "const shv_node_typed_val_t *const shv_node_typed_val_blk"
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
                    "const shv_node_t shv_node_blk"
                    + str(n)
                    + "_par = {\n"
                    + '   .name = "parameters",\n'
                    + "   .dir  = UL_CAST_UNQ1(shv_dmap_t *, &shv_blk_dmap),\n"
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
                            "const shv_node_typed_val_t shv_node_typed_val_blk"
                            + str(n)
                            + "_in"
                            + str(i)
                            + " = {\n"
                            + '   .shv_node = {.name = "input'
                            + str(i)
                            + '",\n'
                            + "            .dir  = UL_CAST_UNQ1(shv_dmap_t *, &shv_double_read_only_dmap),\n"
                            + "           },\n"
                            + "   .val_ptr = Node_"
                            + str(self.blocks[index].pin[i])
                            + ",\n"
                            + '   .type_name = "double",\n};\n\n'
                        )
                    self.f.write(text)
                    text = (
                        "const shv_node_typed_val_t *const shv_node_typed_val_blk"
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
                    "const shv_node_t shv_node_blk"
                    + str(n)
                    + "_in = {\n"
                    + '   .name = "inputs",\n'
                    + "   .dir  = UL_CAST_UNQ1(shv_dmap_t *, &shv_blk_dmap),\n"
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
                            "const shv_node_typed_val_t shv_node_typed_val_blk"
                            + str(n)
                            + "_out"
                            + str(i)
                            + " = {\n"
                            + '   .shv_node = {.name = "output'
                            + str(i)
                            + '",\n'
                            + "            .dir  = UL_CAST_UNQ1(shv_dmap_t *, &shv_double_read_only_dmap),\n"
                            + "           },\n"
                            + "   .val_ptr = Node_"
                            + str(self.blocks[index].pout[i])
                            + ",\n"
                            + '   .type_name = "double",\n};\n\n'
                        )
                    self.f.write(text)
                    text = (
                        "const shv_node_typed_val_t *const shv_node_typed_val_blk"
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
                    "const shv_node_t shv_node_blk"
                    + str(n)
                    + "_out = {\n"
                    + '   .name = "outputs",\n'
                    + "   .dir  = UL_CAST_UNQ1(shv_dmap_t *, &shv_blk_dmap),\n"
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
                    "const shv_node_t *const shv_node_blk"
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
                    "const shv_node_t shv_node_blk"
                    + str(n)
                    + " = {\n"
                    + '   .name = "'
                    + str(self.blocks[index].name)
                    + '",\n'
                    + "   .dir  = UL_CAST_UNQ1(shv_dmap_t *, &shv_blk_dmap),\n"
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
                        "const shv_node_typed_val_t shv_node_typed_val_blk"
                        + str(n)
                        + "_sysIn"
                        + str(i)
                        + " = {\n"
                        + '   .shv_node = {.name = "input'
                        + str(i)
                        + '",\n'
                        + "            .dir  = UL_CAST_UNQ1(shv_dmap_t *, &shv_double_dmap),\n"
                        + "           },\n"
                        + "   .val_ptr = Node_"
                        + str(self.blocks[index].pout[i])
                        + ",\n"
                        + '   .type_name = "double",\n};\n\n'
                    )
                self.f.write(text)
                text = (
                    "const shv_node_typed_val_t *const shv_node_typed_val_blk"
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
                    "const shv_node_t shv_node_blk"
                    + str(n)
                    + "_sysIns = {\n"
                    + '   .name = "'
                    + str(self.blocks[index].name)
                    + '",\n'
                    + "   .dir  = UL_CAST_UNQ1(shv_dmap_t *, &shv_blk_dmap),\n"
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
                        "const shv_node_typed_val_t shv_node_typed_val_blk"
                        + str(n)
                        + "_sysOut"
                        + str(i)
                        + " = {\n"
                        + '   .shv_node = {.name = "output'
                        + str(i)
                        + '",\n'
                        + "            .dir  = UL_CAST_UNQ1(shv_dmap_t *, &shv_double_read_only_dmap),\n"
                        + "           },\n"
                        + "   .val_ptr = Node_"
                        + str(self.blocks[index].pin[i])
                        + ",\n"
                        + '   .type_name = "double",\n};\n\n'
                    )
                self.f.write(text)
                text = (
                    "const shv_node_typed_val_t *const shv_node_typed_val_blk"
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
                    "const shv_node_t shv_node_blk"
                    + str(n)
                    + "_sysOuts = {\n"
                    + '   .name = "'
                    + str(self.blocks[index].name)
                    + '",\n'
                    + "   .dir  = UL_CAST_UNQ1(shv_dmap_t *, &shv_blk_dmap),\n"
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
            text = "const shv_node_t *const shv_node_blks_list[] = {\n"
            for n in range(0, self.blocks_cnt):
                index = blks_names.index(self.blocks_ordered[n])
                if (self.blocks[index].fcn != "shv_input") and (
                    self.blocks[index].fcn != "shv_output"
                ):
                    text += "  &shv_node_blk" + str(n) + ",\n"
            text += "};\n\n"
            self.f.write(text)

        text = (
            "const shv_node_t shv_node_blks = {\n"
            + '   .name = "blocks",\n'
            + "   .dir = UL_CAST_UNQ1(shv_dmap_t *, &shv_root_dmap),\n"
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
            text = "const shv_node_t *const shv_node_blks_inputs[] = {\n"
            for n in range(0, self.blocks_cnt):
                index = blks_names.index(self.blocks_ordered[n])
                if (size(self.blocks[index].pout) != 0) and (
                    self.blocks[index].fcn == "shv_input"
                ):
                    text += "  &shv_node_blk" + str(n) + "_sysIns,\n"
            text += "};\n\n"
            self.f.write(text)

        text = (
            "const shv_node_t shv_node_inputs = {\n"
            + '   .name = "inputs",\n'
            + "   .dir = UL_CAST_UNQ1(shv_dmap_t *, &shv_root_dmap),\n"
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
            text = "const shv_node_t *const shv_node_blks_outputs[] = {\n"
            for n in range(0, self.blocks_cnt):
                index = blks_names.index(self.blocks_ordered[n])
                if (size(self.blocks[index].pin) != 0) and (
                    self.blocks[index].fcn == "shv_output"
                ):
                    text += "  &shv_node_blk" + str(n) + "_sysOuts,\n"
            text += "};\n\n"
            self.f.write(text)

        text = (
            "const shv_node_t shv_node_outputs = {\n"
            + '   .name = "outputs",\n'
            + "   .dir = UL_CAST_UNQ1(shv_dmap_t *, &shv_root_dmap),\n"
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

        text = (
            "const shv_node_t *const shv_tree_root_items[] = {\n"
            + "  &shv_node_blks,\n"
            + "  &shv_node_inputs,\n"
            + "  &shv_node_outputs,\n};\n\n"
        )
        self.f.write(text)

        text = (
            "const shv_node_t shv_tree_root = {\n"
            + "   .dir = UL_CAST_UNQ1(shv_dmap_t *, &shv_root_dmap),\n"
            + "   .children = {.mode = CONF_SHV_TREE_TYPE,\n"
            + "                .list = {.gsa = {.root = {\n"
            + "                      .items = (void **)shv_tree_root_items,\n"
            + "                      .count = sizeof(shv_tree_root_items)/sizeof(shv_tree_root_items[0]),\n"
            + "                      .alloc_count = 0,}\n"
            + "}}}};\n\n"
        )
        self.f.write(text)

        self.f.write("#endif /* CONF_SHV_TREE_STATIC */")

    def generate_code(self) -> None:
        text = "#ifdef CONF_SHV_USED\n"
        text += '  setenv("SHV_BROKER_IP", "' + environ["SHV_BROKER_IP"] + '", 0);\n'
        text += (
            '  setenv("SHV_BROKER_PORT", "' + environ["SHV_BROKER_PORT"] + '", 0);\n'
        )
        text += (
            '  setenv("SHV_BROKER_USER", "' + environ["SHV_BROKER_USER"] + '", 0);\n'
        )
        text += (
            '  setenv("SHV_BROKER_PASSWORD", "'
            + environ["SHV_BROKER_PASSWORD"]
            + '", 0);\n'
        )
        text += (
            '  setenv("SHV_BROKER_DEV_ID", "'
            + environ["SHV_BROKER_DEV_ID"]
            + '", 0);\n'
        )
        text += (
            '  setenv("SHV_BROKER_MOUNT", "'
            + environ["SHV_BROKER_MOUNT"]
            + '", 0);\n\n'
        )
        self.f.write(text)

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
            + ";\n\n"
        )
        self.f.write(text)

        self.f.write("/* Call shv_tree_init() to initialize SHV tree */\n\n")

        if environ["SHV_TREE_TYPE"] != "GSA_STATIC":
            text = "  const shv_node_t shv_tree_root = {};\n\n"
            self.f.write(text)

        text = (
            "  "
            + self.model
            + "_ctx = shv_tree_init(&block_name_map_"
            + self.model
            + ", &shv_tree_root, CONF_SHV_TREE_TYPE);\n\n"
        )
        self.f.write(text)

        self.f.write("#endif /* CONF_SHV_USED */\n\n")

    def generate_end(self) -> None:
        text = "#ifdef CONF_SHV_USED\n"
        text += "  shv_tree_end(" + self.model + "_ctx, CONF_SHV_TREE_TYPE);\n"
        text += "#endif /* CONF_SHV_USED */\n\n"
        self.f.write(text)
