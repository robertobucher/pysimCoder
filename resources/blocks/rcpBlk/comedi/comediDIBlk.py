from supsisim.RCPblk import RCPblk
from scipy import size

def comediDIBlk(pout, dev, ch):
    """

    Call:   comediDIBlk(pout, dev, ch)

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

    blk = RCPblk('comedi_digital_input', [], pout, [0, 0], 0, [], [ch], dev)
    return blk

