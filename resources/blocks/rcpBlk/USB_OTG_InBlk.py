
from supsisim.RCPblk import RCPblk
from numpy import size, zeros

def USB_OTG_InBlk(pout, defvals):
    """

    Call:   USB_OTG_InBlk(pout)

    Parameters
    ----------
       pout: connected output port(s)

    Returns
    -------
       blk: RCPblk

    """

    outputs = len(pout)
    vals = zeros(outputs, float)
    if len(defvals) > outputs:
        N=outputs
    else:
        N = len(defvals)

    for n in range(N):
        vals[n]=defvals[n]

    blk = RCPblk('USB_OTG_In', [], pout, [0,0], 0, vals, [])
    return blk
