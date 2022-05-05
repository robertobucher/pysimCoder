
from supsisim.RCPblk import RCPblk
from numpy import size, hstack

def RLCBlk(pin, pout, port, outs):
    """

    Call:   heaterBlk(pin, pout)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
        : Port

    Returns
    -------
       blk: RCPblk

    """

    if size(pout)>3:
        raise ValueError("Number of outputs should not be greater than 3")
        
    if size(pin)>2:
        raise ValueError("Number of inputs should not be greater than 2")
        
    if size(pout) != size(outs):
        raise ValueError("Number of outputs should fit the len of Outputs")

    intP = hstack((0, outs))
    
    blk = RCPblk('RLC', pin, pout, [0,0], 1, [], outs, port)
    return blk
