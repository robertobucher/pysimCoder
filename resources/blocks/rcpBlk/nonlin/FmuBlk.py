from supsisim.RCPblk import RCPblk, RcpParam
import numpy as np
import zipfile


def getXMLindex(fmu, ref):
    import xml.etree.ElementTree as ET

    ind = -1
    # An FMU is a zip archive; read its model description in place
    with zipfile.ZipFile(fmu) as z:
        with z.open("modelDescription.xml") as f:
            tree = ET.parse(f)
    root = tree.getroot()
    cs = root.findall("./ModelVariables/ScalarVariable")
    for el in cs:
        if el.attrib.get("name") == ref:
            ind = int(el.attrib.get("valueReference"))
            return ind

    return ind


def FmuBlk(*args):
    if len(args) == 3:
        pin, pout, params = args
    elif len(args) == 2:
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

    fmu = params[2].value

    for ref in params[0].value:
        ind = getXMLindex(fmu, ref)
        if ind == -1:
            raise ValueError("Reference value not found!")
        else:
            params.append(RcpParam(ref, ind, RcpParam.Type.INT))

    for ref in params[1].value:
        ind = getXMLindex(fmu, ref)
        if ind == -1:
            raise ValueError("Reference value not found!")
        else:
            params.append(RcpParam(ref, ind, RcpParam.Type.INT))

    params.pop(0)
    params.pop(0)
    return RCPblk("FMUinterface", pin, pout, [0, 0], ft, params)
