from supsisim.RCPblk import RCPblk, RcpParam
import numpy as np
from os import system


def getXMLindex(file, ref):
    import xml.etree.ElementTree as ET

    ind = -1
    tree = ET.parse(file + "/modelDescription.xml")
    root = tree.getroot()
    cs = root.findall("./ModelVariables/ScalarVariable")
    for el in cs:
        if el.attrib.get("name") == ref:
            ind = int(el.attrib.get("valueReference"))
            return ind

    return ind


def FmuBlk(*args):
    if len(args) == 8:
        pin, pout, params = args
    elif len(args) == 7:
        pout, params = args
        pin = []
    else:
        raise ValueError("Not enough arguments received!")

    if np.size(pin) != np.size(params[0].value):
        raise ValueError(
            "Input references not correct; received %i." % np.size(params[0].value)
        )

    if np.size(pout) != np.size(params[1].value):
        raise ValueError(
            "Input references not correct; received %i." % np.size(params[1].value)
        )

    ft = params[-1].value
    params.pop(-1)
    params.append(RcpParam("Inputs Size", np.size(pin), RcpParam.Type.INT))
    params.append(RcpParam("Outputs Size", np.size(pin), RcpParam.Type.INT))

    spl_name = params[2].value.split(".")
    name = "/tmp/" + spl_name[0]

    cmd = "unzip -o -d " + name + " " + params[2].value + " >/dev/null"
    system(cmd)

    for ref in params[0].value:
        ind = getXMLindex(name, ref)
        if ind == -1:
            raise ValueError("Reference value not found!")
        else:
            params.append(RcpParam(ref, ind, RcpParam.Type.INT))

    for ref in params[1].value:
        ind = getXMLindex(name, ref)
        if ind == -1:
            raise ValueError("Reference value not found!")
        else:
            params.append(RcpParam(ref, ind, RcpParam.Type.INT))

    system("rm -fr " + name)

    params.pop(0)
    params.pop(1)
    return RCPblk("FMUinterface", pin, pout, [0, 0], ft, params)
