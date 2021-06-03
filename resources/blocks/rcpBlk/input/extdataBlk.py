from supsisim.RCPblk import RCPblk
from scipy import size

def extdataBlk(pout, ch, datasize, fname):
    """

    Call:   extdataBlk(pout, ch, len, fname)

    Parameters
    ----------
       pout: connected output port(s)
       ch : Channels
       len : Data length
       fname : File name

    Returns
    -------
       blk: RCPblk

    """

    if(size(pout) != ch):
        raise ValueError("Block should have %i output port; received %i." % (ch,size(pout)))
    
    blk = RCPblk('extdata', [], pout, [0,0], 0, [], [ch, datasize, 0], fname)
    return blk
