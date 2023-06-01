from supsisim.RCPblk import RCPblk
from scipy import size

def FH_3XXX_ADBlk(pout, candev, ID):
    """

    Call:   FH_3XXX_ADBlk(pout, ID)

    Parameters
    ----------
       pout: connected output port(s)
       ID : Device ID
 
    Returns
    -------
        blk  : RCPblk

    """
    
    if(size(pout) != 1):
        raise ValueError("Block should have 1 output port; received %i." % size(pout))

    blk = RCPblk('FH_3XXX_AD',[],pout,[0,0],0,[],[ID], candev)
    return blk

