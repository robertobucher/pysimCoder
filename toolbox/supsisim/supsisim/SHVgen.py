"""
This is a procedural interface to generate silicon-heaven tree.

michallenc@seznam.cz

The following commands are provided:

  genSHVheader   - Create includes and defines
  genSHVtree     - Create static SHV tree in generated C code
  genSHVcode     - Create C code providing tree init
  genSHVend      - Provide call for tree destruction
"""

from scipy import size
from os import environ
import copy

def genSHVheader(f, model, N):
    if environ["SHV_USED"] == "True":
        strLn = "#define CONF_SHV_USED 1\n\n"
    else:
        strLn = "#undef CONF_SHV_USED\n\n"
    f.write(strLn)

    strLn  = "#ifdef CONF_SHV_USED\n"
    strLn += "#include <shv_tree.h>\n"
    strLn += "#include <shv_pysim.h>\n"
    strLn += "#include <shv_methods.h>\n"
    strLn += "#include <shv_com.h>\n"
    strLn += "#include <ulut/ul_utdefs.h>\n\n"
    f.write(strLn)

    if environ["SHV_TREE_TYPE"] == "GSA":
        strLn =  "#define CONF_SHV_TREE_TYPE 1\n"
        strLn += "#undef CONF_SHV_TREE_STATIC\n\n"
    elif environ["SHV_TREE_TYPE"] == "GSA_STATIC":
        strLn =  "#define CONF_SHV_TREE_TYPE 3\n"
        strLn += "#define CONF_SHV_TREE_STATIC 1\n\n"
    else:
        strLn = "#define CONF_SHV_TREE_TYPE 0\n"
        strLn += "#undef CONF_SHV_TREE_STATIC\n"

    strLn += "#endif /* CONF_SHV_USED */\n\n"
    f.write(strLn)

    if environ["SHV_USED"] == "True":
        f.write("/* SHV related function and structres */\n\n")
        f.write("#ifdef CONF_SHV_USED\n")
        f.write("shv_con_ctx_t *shv_tree_init(python_block_name_map * block_map, const shv_node_t *static_root, int mode);\n")
        f.write("void shv_tree_end(shv_con_ctx_t *ctx, int mode);\n\n")

        strLn =  "python_block_name_map block_name_map_" + model + ";\n"
        strLn += "python_block_name_entry block_name_entry_" + model + "[" + str(N) + "];\n"
        strLn += "static shv_con_ctx_t *" + model + "_ctx;\n"
        strLn += "#endif /* CONF_SHV_USED */\n\n"
        f.write(strLn)

def genSHVtree(f, Blks, BlksOrdered):
    f.write("#ifdef CONF_SHV_TREE_STATIC\n")

    #f.write("extern const shv_dmap_t shv_double_dmap;\nextern const shv_dmap_t shv_blk_dmap;\nextern const shv_dmap_t shv_root_dmap;\n\n")

    N = size(Blks)

    hasSystemInputs = 0
    hasSystemOutputs = 0
    hasBlocks = 0

    BlksNames = []
    for i in range(0, N):
        BlksNames.append(Blks[i].name)

    for n in range(0, N):
        hasRealPars = 0
        hasInputs = 0
        hasOutpus = 0
        index = BlksNames.index(BlksOrdered[n])

        strLn = ""
        if ((Blks[index].fcn != "shv_input") and (Blks[index].fcn != "shv_output")):
            hasBlocks = 1
            if (size(Blks[index].realPar) != 0):
                hasRealPars = 1

                realParNames= []

                if (size(Blks[index].realPar) == size(Blks[index].realParNames)):
                    for i in range(0,size(Blks[index].realPar)):
                        realParNames.append(Blks[index].realParNames[i])
                else:
                    for i in range(0,size(Blks[index].realPar)):
                        realParNames.append("double" + str(i))

                realParNamesOrdered = copy.deepcopy(realParNames)
                realParNamesOrdered.sort()

                for i in range(0, size(Blks[index].realPar)):
                    indexPar =  realParNames.index(realParNamesOrdered[i])
                    strLn += "const shv_node_typed_val_t shv_node_typed_val_blk" + str(n) + "_par" + str(i) + " = {\n" + \
                                "   .shv_node = {.name = \"" + str(realParNames[indexPar]) + "\",\n" + \
                                "            .dir  = UL_CAST_UNQ1(shv_dmap_t *, &shv_double_dmap),\n" + \
                                "           },\n" + \
                                "   .val_ptr = &realPar_" + str(index) + "[" + str(indexPar) + "],\n" + \
                                "   .type_name = \"double\",\n};\n\n"
                f.write(strLn)
                strLn = "const shv_node_typed_val_t *const shv_node_typed_val_blk" + str(n) + "_pars" + "[] = {\n"
                for i in range(0, size(Blks[index].realPar) ):
                    strLn += "  &shv_node_typed_val_blk" + str(n) + "_par" + str(i) + ",\n"

                strLn += "};\n\n"
                f.write(strLn)

            strLn = "const shv_node_t shv_node_blk" + str(n) + "_par = {\n" + \
                        "   .name = \"parameters\",\n" + \
                        "   .dir  = UL_CAST_UNQ1(shv_dmap_t *, &shv_blk_dmap),\n" + \
                        "   .children = {.mode = CONF_SHV_TREE_TYPE,\n"
            if (hasRealPars):
                strLn+= "                .list = {.gsa = {.root = {\n" + \
                        "                      .items = (void **)shv_node_typed_val_blk" + str(n) + "_pars,\n" + \
                        "                      .count = sizeof(shv_node_typed_val_blk" + str(n) + "_pars)/sizeof(shv_node_typed_val_blk" + str(n) + "_pars[0]),\n" + \
                        "                      .alloc_count = 0,}\n" + \
                        "}}"
            strLn += "}};\n\n"
            f.write(strLn)

            if (size(Blks[index].pin) != 0):
                hasInputs = 1
                strLn = ""
                for i in range(0, size(Blks[index].pin)):
                    strLn += "const shv_node_typed_val_t shv_node_typed_val_blk" + str(n) + "_in" + str(i) + " = {\n" + \
                                "   .shv_node = {.name = \"input" + str(i) + "\",\n" + \
                                "            .dir  = UL_CAST_UNQ1(shv_dmap_t *, &shv_double_read_only_dmap),\n" + \
                                "           },\n" + \
                                "   .val_ptr = Node_" + str(Blks[index].pin[i]) + ",\n" + \
                                "   .type_name = \"double\",\n};\n\n"
                f.write(strLn)
                strLn = "const shv_node_typed_val_t *const shv_node_typed_val_blk" + str(n) + "_ins" + "[] = {\n"
                for i in range(0, size(Blks[index].pin) ):
                    strLn += "  &shv_node_typed_val_blk" + str(n) + "_in" + str(i) + ",\n"
                strLn += "};\n\n"
                f.write(strLn)

            strLn = "const shv_node_t shv_node_blk" + str(n) + "_in = {\n" + \
                        "   .name = \"inputs\",\n" + \
                        "   .dir  = UL_CAST_UNQ1(shv_dmap_t *, &shv_blk_dmap),\n" + \
                        "   .children = {.mode = CONF_SHV_TREE_TYPE,\n"
            if (hasInputs):
                strLn+= "                .list = {.gsa = {.root = {\n" + \
                        "                      .items = (void **)shv_node_typed_val_blk" + str(n) + "_ins,\n" + \
                        "                      .count = sizeof(shv_node_typed_val_blk" + str(n) + "_ins)/sizeof(shv_node_typed_val_blk" + str(n) + "_ins[0]),\n" + \
                        "                      .alloc_count = 0,}\n" + \
                        "}}"
            strLn += "}};\n\n"
            f.write(strLn)

            if (size(Blks[index].pout) != 0):
                hasOutpus = 1
                strLn = ""
                for i in range(0, size(Blks[index].pout)):
                    strLn += "const shv_node_typed_val_t shv_node_typed_val_blk" + str(n) + "_out" + str(i) + " = {\n" + \
                                "   .shv_node = {.name = \"output" + str(i) + "\",\n" + \
                                "            .dir  = UL_CAST_UNQ1(shv_dmap_t *, &shv_double_read_only_dmap),\n" + \
                                "           },\n" + \
                                "   .val_ptr = Node_" + str(Blks[index].pout[i]) + ",\n" + \
                                "   .type_name = \"double\",\n};\n\n"
                f.write(strLn)
                strLn = "const shv_node_typed_val_t *const shv_node_typed_val_blk" + str(n) + "_outs" + "[] = {\n"
                for i in range(0, size(Blks[index].pout) ):
                    strLn += "  &shv_node_typed_val_blk" + str(n) + "_out" + str(i) + ",\n"
                strLn += "};\n\n"
                f.write(strLn)

            strLn = "const shv_node_t shv_node_blk" + str(n) + "_out = {\n" + \
                        "   .name = \"outputs\",\n" + \
                        "   .dir  = UL_CAST_UNQ1(shv_dmap_t *, &shv_blk_dmap),\n" + \
                        "   .children = {.mode = CONF_SHV_TREE_TYPE,\n"
            if (hasOutpus):
                strLn+= "                .list = {.gsa = {.root = {\n" + \
                        "                      .items = (void **)shv_node_typed_val_blk" + str(n) + "_outs,\n" + \
                        "                      .count = sizeof(shv_node_typed_val_blk" + str(n) + "_outs)/sizeof(shv_node_typed_val_blk" +  str(n) + "_outs[0]),\n" + \
                        "                      .alloc_count = 0,}\n" + \
                        "}}"
            strLn += "}};\n\n"
            f.write(strLn)

            strLn = "const shv_node_t *const shv_node_blk" + str(n) + "_items" + "[] = {\n"
            strLn += "  &shv_node_blk" + str(n) + "_in,\n"
            strLn += "  &shv_node_blk" + str(n) + "_out,\n"
            strLn += "  &shv_node_blk" + str(n) + "_par\n"
            strLn += "};\n\n"
            f.write(strLn)

            strLn = "const shv_node_t shv_node_blk" + str(n) + " = {\n" + \
                        "   .name = \"" + str(Blks[index].name) + "\",\n" + \
                        "   .dir  = UL_CAST_UNQ1(shv_dmap_t *, &shv_blk_dmap),\n" + \
                        "   .children = {.mode = CONF_SHV_TREE_TYPE,\n" + \
                        "                .list = {.gsa = {.root = {\n" + \
                        "                      .items = (void **)shv_node_blk" + str(n) + "_items,\n" + \
                        "                      .count = sizeof(shv_node_blk" + str(n) + "_items)/sizeof(shv_node_blk" + str(n) + "_items[0]),\n" + \
                        "                      .alloc_count = 0,}\n" + \
                        "}}}};\n\n"
            f.write(strLn)

        strLn = ""
        if (Blks[index].fcn == "shv_input"):
            hasSystemInputs = 1
            # we have editable outputs (SHV input block)
            for i in range(0, size(Blks[index].pout)):
                strLn += "const shv_node_typed_val_t shv_node_typed_val_blk" + str(n) + "_sysIn" + str(i) + " = {\n" + \
                            "   .shv_node = {.name = \"input" + str(i) + "\",\n" + \
                            "            .dir  = UL_CAST_UNQ1(shv_dmap_t *, &shv_double_dmap),\n" + \
                            "           },\n" + \
                            "   .val_ptr = Node_" + str(Blks[index].pout[i]) + ",\n" + \
                            "   .type_name = \"double\",\n};\n\n"
            f.write(strLn)
            strLn = "const shv_node_typed_val_t *const shv_node_typed_val_blk" + str(n) + "_sysIns" + "[] = {\n"
            for i in range(0, size(Blks[index].pout) ):
                strLn += "  &shv_node_typed_val_blk" + str(n) + "_sysIn" + str(i) + ",\n"

            strLn += "};\n\n"
            f.write(strLn)

            strLn = "const shv_node_t shv_node_blk" + str(n) + "_sysIns = {\n" + \
                        "   .name = \"" + str(Blks[index].name) + "\",\n" + \
                        "   .dir  = UL_CAST_UNQ1(shv_dmap_t *, &shv_blk_dmap),\n" + \
                        "   .children = {.mode = CONF_SHV_TREE_TYPE,\n" + \
                        "                .list = {.gsa = {.root = {\n" + \
                        "                      .items = (void **)shv_node_typed_val_blk" + str(n) + "_sysIns,\n" + \
                        "                      .count = sizeof(shv_node_typed_val_blk" + str(n) + "_sysIns)/sizeof(shv_node_typed_val_blk" + str(n) + "_sysIns[0]),\n" + \
                        "                      .alloc_count = 0,}\n" + \
                        "}}}};\n\n"
            f.write(strLn)

        if (Blks[index].fcn == "shv_output"):
            hasSystemOutputs = 1
            # we have editable inputs (SHV output block)
            for i in range(0, size(Blks[index].pin)):
                strLn += "const shv_node_typed_val_t shv_node_typed_val_blk" + str(n) + "_sysOut" + str(i) + " = {\n" + \
                            "   .shv_node = {.name = \"output" + str(i) + "\",\n" + \
                            "            .dir  = UL_CAST_UNQ1(shv_dmap_t *, &shv_double_read_only_dmap),\n" + \
                            "           },\n" + \
                            "   .val_ptr = Node_" + str(Blks[index].pin[i]) + ",\n" + \
                            "   .type_name = \"double\",\n};\n\n"
            f.write(strLn)
            strLn = "const shv_node_typed_val_t *const shv_node_typed_val_blk" + str(n) + "_sysOuts" + "[] = {\n"
            for i in range(0, size(Blks[index].pin) ):
                strLn += "  &shv_node_typed_val_blk" + str(n) + "_sysOut" + str(i) + ",\n"

            strLn += "};\n\n"
            f.write(strLn)

            strLn = "const shv_node_t shv_node_blk" + str(n) + "_sysOuts = {\n" + \
                        "   .name = \"" + str(Blks[index].name) + "\",\n" + \
                        "   .dir  = UL_CAST_UNQ1(shv_dmap_t *, &shv_blk_dmap),\n" + \
                        "   .children = {.mode = CONF_SHV_TREE_TYPE,\n" + \
                        "                .list = {.gsa = {.root = {\n" + \
                        "                      .items = (void **)shv_node_typed_val_blk" + str(n) + "_sysOuts,\n" + \
                        "                      .count = sizeof(shv_node_typed_val_blk" + str(n) + "_sysOuts)/sizeof(shv_node_typed_val_blk" + str(n) + "_sysOuts[0]),\n" + \
                        "                      .alloc_count = 0,}\n" + \
                        "}}}};\n\n"
            f.write(strLn)

    if (hasBlocks):
        strLn = "const shv_node_t *const shv_node_blks_list[] = {\n"
        for n in range(0, N):
            index = BlksNames.index(BlksOrdered[n])
            if ((Blks[index].fcn != "shv_input") and (Blks[index].fcn != "shv_output")):
                strLn += "  &shv_node_blk" + str(n) + ",\n"
        strLn += "};\n\n"
        f.write(strLn)

    strLn = "const shv_node_t shv_node_blks = {\n" + \
                "   .name = \"blocks\",\n" + \
                "   .dir = UL_CAST_UNQ1(shv_dmap_t *, &shv_root_dmap),\n" + \
                "   .children = {.mode = CONF_SHV_TREE_TYPE,\n"
    if (hasBlocks):
        strLn += "                .list = {.gsa = {.root = {\n" + \
                 "                      .items = (void **)shv_node_blks_list,\n" + \
                 "                      .count = sizeof(shv_node_blks_list)/sizeof(shv_node_blks_list[0]),\n" + \
                 "                      .alloc_count = 0,}\n" + \
                 "}}"
    strLn += "}};\n\n"
    f.write(strLn)

    if (hasSystemInputs):
        strLn = "const shv_node_t *const shv_node_blks_inputs[] = {\n"
        for n in range(0, N):
            index = BlksNames.index(BlksOrdered[n])
            if ((size(Blks[index].pout) != 0) and (Blks[index].fcn == "shv_input")):
                strLn += "  &shv_node_blk" + str(n) + "_sysIns,\n"
        strLn += "};\n\n"
        f.write(strLn)

    strLn = "const shv_node_t shv_node_inputs = {\n" + \
                "   .name = \"inputs\",\n" + \
                "   .dir = UL_CAST_UNQ1(shv_dmap_t *, &shv_root_dmap),\n" + \
                "   .children = {.mode = CONF_SHV_TREE_TYPE,\n"
    if (hasSystemInputs):
        strLn += "                .list = {.gsa = {.root = {\n" + \
                 "                      .items = (void **)shv_node_blks_inputs,\n" + \
                 "                      .count = sizeof(shv_node_blks_inputs)/sizeof(shv_node_blks_inputs[0]),\n" + \
                 "                      .alloc_count = 0,}\n" + \
                 "}}"
    strLn += "}};\n\n"
    f.write(strLn)

    if (hasSystemOutputs):
        strLn = "const shv_node_t *const shv_node_blks_outputs[] = {\n"
        for n in range(0, N):
            index = BlksNames.index(BlksOrdered[n])
            if ((size(Blks[index].pin) != 0) and (Blks[index].fcn == "shv_output")):
                strLn += "  &shv_node_blk" + str(n) + "_sysOuts,\n"
        strLn += "};\n\n"
        f.write(strLn)

    strLn = "const shv_node_t shv_node_outputs = {\n" + \
                "   .name = \"outputs\",\n" + \
                "   .dir = UL_CAST_UNQ1(shv_dmap_t *, &shv_root_dmap),\n" + \
                "   .children = {.mode = CONF_SHV_TREE_TYPE,\n"
    if (hasSystemOutputs):
        strLn += "                .list = {.gsa = {.root = {\n" + \
                 "                      .items = (void **)shv_node_blks_outputs,\n" + \
                 "                      .count = sizeof(shv_node_blks_outputs)/sizeof(shv_node_blks_outputs[0]),\n" + \
                 "                      .alloc_count = 0,}\n" + \
                 "}}"
    strLn += "}};\n\n"
    f.write(strLn)

    strLn = "const shv_node_t *const shv_tree_root_items[] = {\n" + \
            "  &shv_node_blks,\n" + \
            "  &shv_node_inputs,\n" + \
            "  &shv_node_outputs,\n};\n\n"
    f.write(strLn)

    strLn = "const shv_node_t shv_tree_root = {\n" + \
                "   .dir = UL_CAST_UNQ1(shv_dmap_t *, &shv_root_dmap),\n" + \
                "   .children = {.mode = CONF_SHV_TREE_TYPE,\n" + \
                "                .list = {.gsa = {.root = {\n" + \
                "                      .items = (void **)shv_tree_root_items,\n" + \
                "                      .count = sizeof(shv_tree_root_items)/sizeof(shv_tree_root_items[0]),\n" + \
                "                      .alloc_count = 0,}\n" + \
                "}}}};\n\n"
    f.write(strLn)

    f.write("#endif /* CONF_SHV_TREE_STATIC */")

def genSHVcode(f, model, Blks, BlksOrdered):
    strLn = "#ifdef CONF_SHV_USED\n"
    strLn += "  setenv(\"SHV_BROKER_IP\", \"" + environ["SHV_BROKER_IP"] + "\", 0);\n"
    strLn += "  setenv(\"SHV_BROKER_PORT\", \"" + environ["SHV_BROKER_PORT"] + "\", 0);\n"
    strLn += "  setenv(\"SHV_BROKER_USER\", \"" + environ["SHV_BROKER_USER"] + "\", 0);\n"
    strLn += "  setenv(\"SHV_BROKER_PASSWORD\", \"" + environ["SHV_BROKER_PASSWORD"] + "\", 0);\n"
    strLn += "  setenv(\"SHV_BROKER_DEV_ID\", \"" + environ["SHV_BROKER_DEV_ID"] + "\", 0);\n"
    strLn += "  setenv(\"SHV_BROKER_MOUNT\", \"" + environ["SHV_BROKER_MOUNT"] + "\", 0);\n\n"
    f.write(strLn)

    f.write("/* SHV structures definition */\n\n")

    N = size(Blks)

    BlksNames = []
    for i in range(0, N):
        BlksNames.append(Blks[i].name)

    strLn = ""
    for n in range(0,N):
        index = BlksNames.index(BlksOrdered[n])
        strLn += "  block_name_entry_" + model + "[" + str(n) + "].block_name = \"" + BlksOrdered[n] + "\";\n"
        strLn += "  block_name_entry_"  + model + "[" + str(n) + "].block_idx = " + str(index) + ";\n"
        if (Blks[index].fcn == 'shv_input'):
            strLn += "  block_name_entry_"  + model + "[" + str(n) + "].system_inputs = " + str(1) + ";\n"
        else:
            strLn += "  block_name_entry_"  + model + "[" + str(n) + "].system_inputs = " + str(0) + ";\n"
        if (Blks[index].fcn == 'shv_output'):
            strLn += "  block_name_entry_"  + model + "[" + str(n) + "].system_outputs = " + str(1) + ";\n"
        else:
            strLn += "  block_name_entry_"  + model + "[" + str(n) + "].system_outputs = " + str(0) + ";\n"

    strLn += "\n"
    f.write(strLn)

    strLn =  "  block_name_map_" + model + ".blocks_count = " + str(N) + ";\n"
    strLn += "  block_name_map_" + model + ".blocks = " + "block_name_entry_" + model + ";\n"
    strLn += "  block_name_map_" + model + ".block_structure = block_" + model + ";\n\n"
    f.write(strLn)

    f.write("/* Call shv_tree_init() to initialize SHV tree */\n\n")

    if (environ["SHV_TREE_TYPE"] != "GSA_STATIC"):
        strLn = "  const shv_node_t shv_tree_root = {};\n\n"
        f.write(strLn)

    strLn = "  " + model + "_ctx = shv_tree_init(&block_name_map_" + model + ", &shv_tree_root, CONF_SHV_TREE_TYPE);\n\n"
    f.write(strLn)

    f.write("#endif /* CONF_SHV_USED */\n\n")

def genSHVend(f, model):
    strLn =  "#ifdef CONF_SHV_USED\n"
    strLn += "  shv_tree_end(" + model + "_ctx, CONF_SHV_TREE_TYPE);\n"
    strLn += "#endif /* CONF_SHV_USED */\n\n"
    f.write(strLn)