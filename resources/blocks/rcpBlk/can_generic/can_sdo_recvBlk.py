from supsisim.RCPblk import RCPblk
from numpy import size

def can_sdo_recvBlk(pout, candev, ID, index, subindex, K):
    """

    Call:   can_sdo_recvBlk(pout, ID, index, subindex, K)

    Parameters
    ----------
       pout: connected output port(s)
       ID : Device ID
       index : Index
       subindex : SubIndex
       K : Conversion factor

    Returns
    -------
        blk  : RCPblk

    """
    
    retID = 0x580+ID
    blk = RCPblk('can_sdo_recv',[],pout,[0,0],0,[K],[ID, index, subindex, retID], candev)
    return blk

