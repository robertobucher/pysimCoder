from supsisim.RCPblk import RCPblk
from scipy import size

def baumer_EncBlk(pout, ID, res, encres):
    """

    Call:   baumer_EncBlk(pout, ID, res, encres)

    Parameters
    ----------
       pout: connected output port(s)
       ID : Device ID
       res : Resolution
       encres : Encoder reset

    Returns
    -------
        blk  : RCPblk

    """
    
    if(size(pout) != 1):
        raise ValueError("Block should have 1 output port; received %i." % size(pout))

    blk = RCPblk('baumer_enc',[],pout,[0,0],0,[4*res],[ID, encres])
    return blk

