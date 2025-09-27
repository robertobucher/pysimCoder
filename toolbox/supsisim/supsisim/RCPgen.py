"""
This is a procedural interface to the RCPblk library

roberto.bucher@supsi.ch

The following class is provided:

  RCPblk      - class with info for Rapid Controller Prototyping

The following commands are provided:

  genCode        - Create  C code from BlockDiagram
  genMake        - Generate the Makefile for the C code
  detBlkSeq      - Get the right block sequence for simulation and RT
  sch2blks       - Generate block list fron schematic
  
"""
from numpy import  nonzero, ones, asmatrix, size, array, zeros
from os import environ
import copy
import sys
from supsisim.RCPblk import RCPblk, RcpParam
from .shv import ShvTreeGenerator

def genCode(model, Tsamp, blocks, rkMethod='standard_RK4', epsAbs = 1e-6, epsRel = 1e-6, rkstep = 10):
    """Generate C-Code

    Call: genCode(model, Tsamp, Blocks, rkstep)

    Parameters
    ----------
    model     : Model name
    Tsamp     : Sampling Time
    Blocks    : Block list
    rkMethod  : Numerical integration algoritm
    rkstep    : step division pro sample time for fixed step solverM

    Returns
    -------
    -
"""
    maxNode = 0
    for blk in blocks:
        for n in range(0,size(blk.pin)):
            if blk.pin[n] > maxNode:
                maxNode = blk.pin[n]
        for n in range(0,size(blk.pout)):
            if blk.pout[n] > maxNode:
                maxNode = blk.pout[n]

    # Check outputs not connected together!
    outnodes = zeros(maxNode+1)
    for blk in blocks:
        for n in range(0,size(blk.pout)):
            if outnodes[blk.pout[n]] == 0:
                outnodes[blk.pout[n]] = 1
            else:
                raise ValueError('Problem in diagram: outputs connected together!')

    Blocks = detBlkSeq(maxNode, blocks)
    if size(Blocks) == 0:
        raise ValueError('No possible to determine the block sequence')

    gslFlag = (rkMethod != 'standard RK4')
    fn = model + '.c'
    f=open(fn,'w')
    strLn = '#include <pyblock.h>\n#include <stdio.h>\n#include <stdlib.h>\n'
    f.write(strLn)
    if gslFlag:
        f.write('#include <string.h>\n#include <gsl/gsl_odeiv2.h>\n#include <matop.h>\n\n')
    else:
        f.write('\n')


    N = size(Blocks)

    shv_generator = ShvTreeGenerator(f, model, Blocks)
    shv_generator.generate_header()

    # Generate the model's context
    mctx = "/* Model's context */\n"
    mctx += 'struct pysim_model_ctx ' + model + '_ctx;\n'
    mctx += 'extern struct pysim_platform_model_ctx ' + model + '_pt_ctx;\n\n'
    f.write(mctx)

    # Generate the model's function prototypes
    f.write('/* Function prototypes */\n\n')
    prototypes  = "void " + model + "_init(void);\n"
    prototypes += "void " + model + "_isr(double);\n"
    prototypes += "void " + model + "_end(void);\n"
    prototypes += "double " + model + "_get_tsamp(void);\n"
    prototypes += "#ifdef CONF_SHV_USED\n"
    prototypes += "int " + model + "_com_init(shv_attention_signaller at_signlr);\n"
    prototypes += "void " + model + "_com_end(void);\n"
    prototypes += "#endif\n"
    prototypes += "double " + model + "_runtime(struct pysim_platform_model_ctx *pt_arg);\n"
    prototypes += "int " + model + "_comprio(struct pysim_platform_model_ctx *pt_arg);\n"
    prototypes += "void " + model + "_pausectrl(struct pysim_platform_model_ctx *pt_arg);\n"
    prototypes += "void " + model + "_resumectrl(struct pysim_platform_model_ctx *pt_arg);\n"
    prototypes += "int " + model + "_getctrlstate(struct pysim_platform_model_ctx *pt_arg);\n\n"
    f.write(prototypes)

    prototypes = []

    for blk in Blocks:
        prototypes.append('void ' + blk.fcn + '(int Flag, python_block *block);\n')
    setProto = set(prototypes)
    for el in setProto:
        f.write(el)

    f.write('\n')

    strLn = 'double ' + model + '_get_tsamp(void)\n'
    strLn += '{\n'
    strLn += '  return (' + str(Tsamp) + ');\n'
    strLn += '}\n\n'
    f.write(strLn)

    strLn = 'python_block block_' + model + '[' + str(N) + '];\n\n'
    f.write(strLn)

    for n in range(N):
        blk: RCPblk = Blocks[n]
        if sum(param.type == RcpParam.Type.DOUBLE for param in blk.params_list) != 0:
            values: str = ""
            names: str = ""
            values_num: int = 0
            names_num: int = 0
            for param in blk.params_list:
                if param.type == RcpParam.Type.DOUBLE:
                    if param.is_list:
                        values += str(asmatrix(param.value).tolist())[2:-2] + ", "
                        values_num += size(asmatrix(param.value).tolist())
                    else:
                        values += str(param.value) + ", "
                        values_num += 1
                    names += f'"{param.name}", '
                    names_num += 1

            if values_num != names_num:
                names = ""
                for i in range(values_num):
                    names += f'"double{i}", '
            strLn = "static double realPar_" + str(n) +"[] = {"
            strLn += values + "};\n"
            strLn += "static char *realParNames_" + str(n) + "[] = {"
            strLn += names + "};\n"
            f.write(strLn)
        if sum(param.type == RcpParam.Type.INT for param in blk.params_list) != 0:
            values: str = ""
            names: str = ""
            values_num: int = 0
            names_num: int = 0
            for param in blk.params_list:
                if param.type == RcpParam.Type.INT:
                    if param.is_list:
                        values += str(asmatrix(param.value).tolist())[2:-2] + ", "
                        values_num += size(asmatrix(param.value).tolist())
                    else:
                        values += str(param.value) + ", "
                        values_num += 1
                    names += f'"{param.name}", '
            if values_num != names_num:
                names = ""
                for i in range(values_num):
                    names += f'"int{i}", '
            strLn = "static int intPar_" + str(n) +"[] = {"
            strLn += values + "};\n"
            strLn += "static char *intParNames_" + str(n) + "[] = {"
            strLn += names + "};\n"
            f.write(strLn)
        strLn = 'static int nx_' + str(n) +'[] = {'
        strLn += str(asmatrix(blk.nx).tolist())[2:-2] + '};\n'
        f.write(strLn)
    f.write('\n')

    f.write('/* Nodes */\n')
    for n in range(1,maxNode+1):
        strLn = 'static double Node_' + str(n) + '[] = {0.0};\n'
        f.write(strLn)

    f.write('\n')

    f.write('/* Input and outputs */\n')
    for n in range(0,N):
        blk = Blocks[n]
        nin = size(blk.pin)
        nout = size(blk.pout)
        if (nin!=0):
            strLn = 'static void *inptr_' + str(n) + '[]  = {'
            for m in range(0,nin):
                strLn += '&Node_' + str(blk.pin[m]) + ','
            strLn = strLn[0:-1] + '};\n'
            f.write(strLn)
        if (nout!=0):
            strLn = 'static void *outptr_' + str(n) + '[] = {'
            for m in range(0,nout):
                strLn += '&Node_' + str(blk.pout[m]) + ','
            strLn = strLn[0:-1] + '};\n'
            f.write(strLn)

    f.write('\n\n')

    if (environ['SHV_TREE_TYPE'] == 'GSA_STATIC') and (environ['SHV_USED'] == 'True'):
        shv_generator.generate_tree()

    if (environ['SHV_USED'] == 'True'):
        shv_generator.generate_init()
        shv_generator.generate_end()

    f.write('/* Initialization function */\n\n')
    strLn = 'void ' + model + '_init(void)\n'
    strLn += '{\n'
    f.write(strLn)

    f.write('/* Block definition */\n\n')
    for n in range(0,N):
        blk = Blocks[n]
        nin = size(blk.pin)
        nout = size(blk.pout)
        num = 0

        strLn =  '  block_' + model + '[' + str(n) + '].nin  = ' + str(nin) + ';\n'
        strLn += '  block_' + model + '[' + str(n) + '].nout = ' + str(nout) + ';\n'

        port = 'nx_' + str(n)
        strLn += '  block_' + model + '[' + str(n) + '].nx   = ' + port + ';\n'

        if (nin == 0):
            port = 'NULL'
        else:
            port = 'inptr_' + str(n)
        strLn += '  block_' + model + '[' + str(n) + '].u    = ' + port + ';\n'
        if (nout == 0):
            port = 'NULL'
        else:
            port = 'outptr_' + str(n)
        strLn += '  block_' + model + '[' + str(n) + '].y    = ' + port + ';\n'
        if sum(param.type == RcpParam.Type.DOUBLE for param in blk.params_list) != 0:
            par = 'realPar_' + str(n)
            parNames = 'realParNames_' + str(n)
            num = sum(param.type == RcpParam.Type.DOUBLE for param in blk.params_list)
        else:
            par = 'NULL'
            parNames = 'NULL'
            num = 0
        strLn += '  block_' + model + '[' + str(n) + '].realPar = ' + par + ';\n'
        strLn += '  block_' + model + '[' + str(n) + '].realParNum = ' + str(num) + ';\n'
        strLn += '  block_' + model + '[' + str(n) + '].realParNames = ' + parNames + ';\n'
        if sum(param.type == RcpParam.Type.INT for param in blk.params_list) != 0:
            par = 'intPar_' + str(n)
            parNames = 'intParNames_' + str(n)
            num = sum(param.type == RcpParam.Type.INT for param in blk.params_list)
        else:
            par = 'NULL'
            parNames = 'NULL'
            num = 0
        strLn += '  block_' + model + '[' + str(n) + '].intPar = ' + par + ';\n'
        strLn += '  block_' + model + '[' + str(n) + '].intParNum = ' + str(num) + ';\n'
        strLn += '  block_' + model + '[' + str(n) + '].intParNames = ' + parNames + ';\n'
        str_param: str = ""
        for param in blk.params_list:
            if param.type == RcpParam.Type.STR:
                str_param = param.value
                break
        strLn += '  block_' + model + '[' + str(n) + '].str = ' + '"' + str_param + '"' + ';\n'
        strLn += '  block_' + model + '[' + str(n) + '].ptrPar = NULL;\n'
        f.write(strLn)
        f.write('\n')
    f.write('\n')

    if environ['SHV_USED'] == 'True':
        shv_generator.generate_code()

    f.write('/* Set initial outputs */\n\n')

    for n in range(0,N):
        blk = Blocks[n]
        strLn = '  ' + blk.fcn + '(CG_INIT, &block_' + model + '[' + str(n) + ']);\n'
        f.write(strLn)
    f.write('}\n\n')

    f.write('/* ISR function */\n\n')
    strLn = 'void ' + model + '_isr(double t)\n'
    strLn += '{\n'
    f.write(strLn)

    contIntg = False
    for n in range(0,N):
        blk = Blocks[n]
        if blk.fcn in ['css', 'integral']:
            contIntg = True

    if contIntg:
        f.write('int i;\n')
        f.write('double h;\n')

    if gslFlag:
        f.write('double t0;\n')
        f.write('gsl_odeiv2_driver *driver;\n')
        f.write('int status;\n')

    f.write('\n')

    for n in range(0,N):
        blk = Blocks[n]
        strLn = '  ' + blk.fcn + '(CG_OUT, &block_' + model + '[' + str(n) + ']);\n'
        f.write(strLn)
    f.write('\n')

    if contIntg:
        strLn = '  h = ' + model + '_get_tsamp()/' + str(rkstep) + ';\n\n'
        f.write(strLn)

        for n in range(0,N):
            blk = Blocks[n]
            if blk.fcn in ['css', 'integral']:
                if gslFlag:
                    nStates = blk.nx[0]
                    strLn = '  gsl_odeiv2_system sys' + str(n) + ' = {' + blk.fcn +'Func, NULL, ' + \
                    str(nStates) + ', &block_' + model + '[' + str(n) + ']};\n'
                    strLn += '  driver = gsl_odeiv2_driver_alloc_y_new(&sys' + str(n) + ', ' + rkMethod + \
                    ', ' + str(epsAbs) + ', ' + str(epsRel) + ', 0.0);\n'
                    strLn += '  block_' + model + '[' + str(n) + '].ptrPar = (void *) driver;\n'
                    f.write(strLn)

                    try:
                        nrp = len(blk.realPar[0])
                        pos = nrp - nStates
                        strLn = '  double y_' + str(n) + '[' + str(nStates) + '];\n'
                        f.write(strLn)

                    except:
                        nrp = len(blk.realPar)
                        pos = nrp - nStates
                        strLn = '  double y_' + str(n) + '[' + str(nStates) + '];\n'
                        f.write(strLn)

                    strLn = '  memcpy(y_' + str(n) + ', &(block_' + model + '[' + str(n) + '].realPar[' +\
                    str(pos) + ']),' + str(nStates) + '*sizeof(double));\n'
                    f.write(strLn)
                else:
                    strLn = '  block_' + model + '[' + str(n) + '].realPar[0] = h;\n'
                    f.write(strLn)

        strLn = '  for(i=0;i<' + str(rkstep) + ';i++){\n'
        f.write(strLn)
        for n in range(0,N):
            blk = Blocks[n]
            if blk.fcn in ['css', 'integral'] or (len(blk.pout) != 0 and blk.uy == 1):
                strLn = '    ' + blk.fcn + '(CG_OUT, &block_' + model + '[' + str(n) + ']);\n'
                f.write(strLn)

        for n in range(0,N):
            blk = Blocks[n]
            if blk.fcn in ['css', 'integral']:
                if gslFlag:
                    nStates = blk.nx[0]
                    strLn = '    t0 = 0.0;\n'
                    strLn += '    driver = (gsl_odeiv2_driver *) block_' + model + '[' + str(n) + '].ptrPar;\n'
                    strLn += '    status = gsl_odeiv2_driver_apply(driver, &t0, t0+h, y_' + str(n) + ');\n'
                    f.write(strLn)
                    try:
                        nrp = len(blk.realPar[0])
                    except:
                        nrp = len(blk.realPar)
                    pos = nrp-nStates
                    strLn = '    memcpy(&(block_' + model + '[' + str(n) + '].realPar[' +\
                    str(pos) + ']), y_' + str(n) + ', ' + str(nStates) + '*sizeof(double));\n'
                    f.write(strLn)
                else:
                    strLn = '    ' + blk.fcn + '(CG_STUPD, &block_' + model + '[' + str(n) + ']);\n'
                    f.write(strLn)

        strLn = '  }\n'
        f.write(strLn)

    for n in range(0,N):
        blk = Blocks[n]
        if blk.nx[1] != 0:
            strLn = '  ' + blk.fcn + '(CG_STUPD, &block_' + model + '[' + str(n) + ']);\n'
            f.write(strLn)


    f.write('}\n\n')

    f.write('/* Termination function */\n\n')

    strLn = 'void ' + model + '_end(void)\n'
    strLn += '{\n'
    f.write(strLn)

    if gslFlag:
        f.write('gsl_odeiv2_driver *driver;\n')

    for n in range(0,N):
        blk = Blocks[n]
        if blk.fcn in ['css', 'integral']:
            if gslFlag:
                strLn = '  driver = (gsl_odeiv2_driver *) block_' + model + '[' + str(n) + '].ptrPar;\n'
                strLn += '  gsl_odeiv2_driver_free(driver);\n'
            else:
                strLn = '  ' + blk.fcn + '(CG_END, &block_' + model + '[' + str(n) + ']);\n'
            f.write(strLn)
        else:
            strLn = '  ' + blk.fcn + '(CG_END, &block_' + model + '[' + str(n) + ']);\n'
            f.write(strLn)

    f.write('}\n\n')
    f.close()

def genMake(model, template, addObj = '', addCDefs = ''):
    """Generate the Makefile

    Call: genMake(model, template)

    Parameters
    ----------
    model     : Model name
    template  : Template makefile
    addObj    : Additional object files
    addCDefs  : Additional C Defines

    Returns
    -------
    -
"""
    template_path = environ.get('PYSUPSICTRL')
    fname = template_path + '/CodeGen/templates/' + template
    f = open(fname,'r')
    mf = f.read()
    f.close()
    mf = mf.replace('$$MODEL$$',model)
    mf = mf.replace('$$ADD_FILES$$',addObj)

    # Define SHV parameters for the build
    if environ.get('SHV_USED') == 'True':
        addCDefs += ' \'-DCONF_SHV_USED\''
        if environ.get('SHV_UPDATES_USED') == 'True':
            addCDefs += ' \'-DCONF_SHV_UPDATES_USED\''

    mf = mf.replace('$$ADDITIONAL_DEFINES$$', addCDefs)
    f = open('Makefile','w')
    f.write(mf)
    f.close()

def detBlkSeq(Nodes, blocks):
    """Generate the Block sequence for simulation and RT

    Call: detBlkSeq(Nodes, Blocks)

    Parameters
    ----------
    Nodes     : Number of total nodes in diagram
    blocks    : List with the unordered blocks

    Returns
    -------
    Blocks    : List with the ordered blocks

"""
    class blkDep:
        def __init__(self, block, blkL, nodeL):
            self.block = block
            self.block_in = []

            if len(block.pin) != 0:
                for node in block.pin:
                    if nodeL[node].block_in[0].uy == 1:
                        self.block_in.append(nodeL[node].block_in[0])


        def __str__(self):
            txt  = 'Block: ' + self.block.fcn.__str__() + '\n'
            txt += 'Inputs\n'
            for item in self.block_in:
                txt += item.fcn + '\n'
            txt += '\n'
            return txt

    class nodeClass:
        def __init__(self, N):
            self.PN = N
            self.block_in = []
            self.block_out = []

        def __str__(self):
            txt  = 'Node: ' + self.PN.__str__() + '\n'
            txt += ' Blocks in:\n'
            for item in self.block_in:
                try:
                    txt += item.fcn + '\n'
                except:
                    txt += 'None\n'
            txt += ' Blocks out:\n'
            for item in self.block_out:
                try:
                    txt += item.fcn + '\n'
                except:
                    txt += 'None\n'
            return txt

    def fillNodeList(nN,blks):
        nL = []
        nL.append(nodeClass(0))
        for n in range(1, nN+1):
            node = nodeClass(n)
            nL.append(node)
        for blk in blks:
            for n in blk.pin:
                nL[n].block_out.append(blk)
            for n in blk.pout:
                nL[n].block_in.append(blk)
        return nL

    blks = []
    blks2order = []

    nodes = fillNodeList(Nodes, blocks)

    # First search block with no input and no output

    for blk in blocks:
        if blk.uy == 0:
            if len(blk.pin) == 0 and len(blk.pout) == 0:
                blks.insert(0, blk)
            else:
                blks.append(blk)
        else:
            block = blkDep(blk, blocks, nodes)
            blks2order.append(block)

    # Order the remaining blocks
    counter = 0
    while len(blks2order) != counter:
        blk = blks2order.pop(0)
        if len(blk.block_in) == 0:
            blks.append(blk.block)
            counter = 0

            try:
                for node in blk.block.pout:
                    for bk in nodes[node].block_out:
                        el=[el for el in blks2order if el.block == bk]
                        try:
                            el[0].block_in.remove(blk.block)
                        except:
                            pass
            except:
                pass
        else:
            blks2order.append(blk)
            counter += 1

    # Check if remain blocks in blks2order -> Algeabric loop!
    if len(blks2order) != 0:
        for item in blks2order:
            print(item.block)
        raise ValueError('Algeabric loop!')

    return blks
