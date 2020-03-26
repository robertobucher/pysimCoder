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
from scipy import mat, size, array, zeros
from numpy import  nonzero, ones
from os import environ
import sys
from supsisim.RCPblk import RCPblk

def genCode(model, Tsamp, blocks, rkstep = 10):
    """Generate C-Code

    Call: genCode(model, Tsamp, Blocks, rkstep)

    Parameters
    ----------
    model     : Model name
    Tsamp     : Sampling Time
    Blocks    : Block list
    rkstep    : step division pro sample time for fixed step solver

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
    
    fn = model + '.c'
    f=open(fn,'w')
    strLn = "#include <pyblock.h>\n#include <stdio.h>\n\n"
    f.write(strLn)
    N = size(Blocks)

    totContBlk = 0
    for blk in Blocks:
        totContBlk += blk.nx[0]

    f.write("/* Function prototypes */\n\n")

    for blk in Blocks:
        strLn = "void " + blk.fcn + "(int Flag, python_block *block);\n"
        f.write(strLn)

    f.write("\n")

    strLn = "double " + model + "_get_tsamp()\n"
    strLn += "{\n"
    strLn += "  return (" + str(Tsamp) + ");\n"
    strLn += "}\n\n"
    f.write(strLn)

    strLn = "python_block block_" + model + "[" + str(N) + "];\n\n"
    f.write(strLn);

    for n in range(0,N):
        blk = Blocks[n]
        if (size(blk.realPar) != 0):
            strLn = "static double realPar_" + str(n) +"[] = {"
            strLn += str(mat(blk.realPar).tolist())[2:-2] + "};\n"
            f.write(strLn)
        if (size(blk.intPar) != 0):
            strLn = "static int intPar_" + str(n) +"[] = {"
            strLn += str(mat(blk.intPar).tolist())[2:-2] + "};\n"
            f.write(strLn)
        strLn = "static int nx_" + str(n) +"[] = {"
        strLn += str(mat(blk.nx).tolist())[2:-2] + "};\n"
        f.write(strLn)
    f.write("\n")

    f.write("/* Nodes */\n")
    for n in range(1,maxNode+1):
        strLn = "static double Node_" + str(n) + "[] = {0.0};\n"
        f.write(strLn)

    f.write("\n")

    f.write("/* Input and outputs */\n")
    for n in range(0,N):
        blk = Blocks[n]
        nin = size(blk.pin)
        nout = size(blk.pout)
        if (nin!=0):
            strLn = "static void *inptr_" + str(n) + "[]  = {"
            for m in range(0,nin):
                strLn += "0,"
            strLn = strLn[0:-1] + "};\n"
            f.write(strLn)
        if (nout!=0):
            strLn = "static void *outptr_" + str(n) + "[] = {"
            for m in range(0,nout):
                strLn += "0,"
            strLn = strLn[0:-1] + "};\n"
            f.write(strLn)

    f.write("\n\n")

    f.write("/* Initialization function */\n\n")
    strLn = "void " + model + "_init()\n"
    strLn += "{\n"
    f.write(strLn)
    for n in range(0,N):
        blk = Blocks[n]
        nin = size(blk.pin)
        nout = size(blk.pout)

        if (nin!=0):
            for m in range(0,nin):
                strLn = "  inptr_" + str(n) + "[" + str(m) + "]  = (void *) Node_" + str(blk.pin[m]) + ";\n"
                f.write(strLn)
        if (nout!=0):
            for m in range(0,nout):
                strLn = "  outptr_" + str(n) + "[" + str(m) + "] = (void *) Node_" + str(blk.pout[m]) + ";\n"
                f.write(strLn)
    f.write("\n")

    f.write("/* Block definition */\n\n")
    for n in range(0,N):
        blk = Blocks[n]
        nin = size(blk.pin)
        nout = size(blk.pout)

        strLn =  "  block_" + model + "[" + str(n) + "].nin  = " + str(nin) + ";\n"
        strLn += "  block_" + model + "[" + str(n) + "].nout = " + str(nout) + ";\n"

        port = "nx_" + str(n)
        strLn += "  block_" + model + "[" + str(n) + "].nx   = " + port + ";\n"

        if (nin == 0):
            port = "NULL"
        else:
            port = "inptr_" + str(n)
        strLn += "  block_" + model + "[" + str(n) + "].u    = " + port + ";\n"
        if (nout == 0):
            port = "NULL"
        else:
            port = "outptr_" + str(n)
        strLn += "  block_" + model + "[" + str(n) + "].y    = " + port + ";\n"
        if (size(blk.realPar) != 0):
            par = "realPar_" + str(n)
        else:
            par = "NULL"
        strLn += "  block_" + model + "[" + str(n) + "].realPar = " + par + ";\n"
        if (size(blk.intPar) != 0):
            par = "intPar_" + str(n)
        else:
            par = "NULL"
        strLn += "  block_" + model + "[" + str(n) + "].intPar = " + par + ";\n"
        strLn += "  block_" + model + "[" + str(n) + "].str = " +'"' + blk.str + '"' + ";\n"
        strLn += "  block_" + model + "[" + str(n) + "].ptrPar = NULL;\n"
        f.write(strLn)
        f.write("\n")
    f.write("\n")

    f.write("/* Set initial outputs */\n\n")

    for n in range(0,N):
        blk = Blocks[n]
        strLn = "  " + blk.fcn + "(CG_INIT, &block_" + model + "[" + str(n) + "]);\n"
        f.write(strLn)
    f.write("}\n")

    f.write("/* ISR function */\n\n")
    strLn = "void " + model + "_isr(double t)\n"
    strLn += "{\n"
    f.write(strLn)

    if (totContBlk != 0):
        f.write("int i, j;\n")
        f.write("double h;\n\n")

    for n in range(0,N):
        blk = Blocks[n]
        strLn = "  " + blk.fcn + "(CG_OUT, &block_" + model + "[" + str(n) + "]);\n"
        f.write(strLn)
    f.write("\n")

    for n in range(0,N):
        blk = Blocks[n]
        if (blk.nx[1] != 0):
            strLn = "  " + blk.fcn + "(CG_STUPD, &block_" + model + "[" + str(n) + "]);\n"
            f.write(strLn)
    f.write("\n")

    if (totContBlk != 0):
        strLn = "  h = " + model + "_get_tsamp()/" + str(rkstep) + ";\n\n"
        f.write(strLn)

        for n in range(0,N):
            blk = Blocks[n]
            if (blk.nx[0] != 0):
                strLn = "  block_" + model + "[" + str(n) + "].realPar[0] = h;\n"
                f.write(strLn)
            
        strLn = "  for(i=0;i<" + str(rkstep) + ";i++){\n"
        f.write(strLn)
        for n in range(0,N):
            blk = Blocks[n]
            if (blk.nx[0] != 0):
                strLn = "    " + blk.fcn + "(CG_OUT, &block_" + model + "[" + str(n) + "]);\n"
                f.write(strLn)

        for n in range(0,N):
            blk = Blocks[n]
            if (blk.nx[0] != 0):
                strLn = "    " + blk.fcn + "(CG_STUPD, &block_" + model + "[" + str(n) + "]);\n"
                f.write(strLn)

        f.write("  }\n")

    f.write("}\n")

    f.write("/* Termination function */\n\n")
    strLn = "void " + model + "_end()\n"
    strLn += "{\n"
    f.write(strLn)
    for n in range(0,N):
        blk = Blocks[n]
        strLn = "  " + blk.fcn + "(CG_END, &block_" + model + "[" + str(n) + "]);\n"
        f.write(strLn)
    f.write("}\n\n")
    f.close()

def genMake(model, template, addObj = ''):
    """Generate the Makefile

    Call: genMake(model, template)

    Parameters
    ----------
    model     : Model name
    template  : Template makefile
    addObj    : Additional object files

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
        raise ValueError("Algeabric loop!")
    
    return blks
