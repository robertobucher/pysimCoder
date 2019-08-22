from supsisim.RCPblk import RCPblk
import numpy as np
from os import system

def getXMLindex(file,ref):
    import xml.etree.ElementTree as ET

    ind = -1;
    tree = ET.parse(file + '/modelDescription.xml')
    root = tree.getroot()
    cs=root.findall('./ModelVariables/ScalarVariable')
    for el in cs:
        if el.attrib.get('name')==ref:
            ind = int(el.attrib.get('valueReference'))
            return ind

    return ind

def FmuBlk(*args):
    if len(args) == 8:
        pin, pout, IN_ref, OUT_ref, file, ts, dt, ft = args
    elif len(args) == 7:
        pout, IN_ref, OUT_ref, file, ts, dt, ft = args
        pin = []
    else:
         raise ValueError("Not enough arguments received!")
       
    if np.size(pin) != np.size(IN_ref):
        raise ValueError("Input references not correct; received %i." % np.size(IN_ref))

    if np.size(pout) != np.size(OUT_ref):
        raise ValueError("Input references not correct; received %i." % np.size(OUT_ref))

    intPar = np.zeros(2 + np.size(pin)+np.size(pout), dtype=np.int16)
    intPar[0] =  np.size(pin)
    intPar[1] =  np.size(pout)

    spl_name = file.split('.')
    name = '/tmp/'+spl_name[0]
    
    cmd = 'unzip -o -d ' + name + ' ' + file + ' >/dev/null'
    system(cmd)

    pos = 0
    for ref in IN_ref:
        ind = getXMLindex(name, ref)
        if ind == -1:
            raise ValueError('Reference value not found!')
        else:
            intPar[pos+2] = ind
            pos += 1

    for ref in OUT_ref:
        ind = getXMLindex(name, ref)
        if ind == -1:
            raise ValueError('Reference value not found!')
        else:
            intPar[pos+2] = ind
            pos += 1

    system('rm -fr ' + name)
            
    blk = RCPblk('FMUinterface',pin,pout,[0,0],ft,[ts, dt],intPar, file)
    return blk
