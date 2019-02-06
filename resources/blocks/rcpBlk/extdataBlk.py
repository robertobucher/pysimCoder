from supsisim.RCPblk import RCPblk
from scipy import size

def extdataBlk(pout, length, fname):
    """

    Call:   extdataBlk(pout, length, fname)

    Parameters
    ----------
       pout: connected output port(s)
       length : Data length
       fname : File name

    Returns
    -------
        blk  : RCPblk

    """
    
    if(size(pout) != 1):
        raise ValueError("Block should have 1 output port; received %i." % size(pout))

    blk = RCPblk('extdata',[],pout,[0,0],0,[],[length,0],fname)
    return blk

