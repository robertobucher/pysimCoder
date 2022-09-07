from supsisim.RCPblk import RCPblk
from scipy import size

def comediENCBlk(pout, dev, ch):
    """

    Call:   comediENCBlk(pout, dev, ch)

    Parameters
    ----------
       pout: connected output port(s)
       dev : Device
       ch : Channel

    Returns
    -------
        blk  : RCPblk

    """
    
    if size(pout) != 1:
        raise ValueError("Block should have 1 input port; received %i !" % size(pout))

    if ch > 1:
        raise ValueError("Channel value can be 0 or 1 not %i !" % ch)

    blk = RCPblk('comedi_encoder', [], pout, [0,0], 0, [], [ch], dev)
    return blk

