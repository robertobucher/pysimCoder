from supsisim.RCPblk import RCPblk
from scipy import size

def can_gen_recvBlk(pout, ID, retID, index, subindex, K):
    """

    Call:   can_gen_recvBlk(pout, ID, retID, index, subindex, K)

    Parameters
    ----------
       pout: connected output port(s)
       ID : Device ID
       retID : ReturnID
       index : Index
       subindex : SubIndex
       K : Conversion factor

    Returns
    -------
       blk  : RCPblk

    """
    
    blk = RCPblk('can_gen_recv',[],pout,[0,0],0,[K],[ID, index, subindex, retID])
    return blk

