from supsisim.RCPblk import RCPblk
from numpy import size

def comediDOBlk(pin, dev, ch, thr):
    """

    Call:   comediDOBlk(pin, dev, ch, thr)

    Parameters
    ----------
       pin: connected input port(s)
       dev : Device
       ch : Channel
       thr : Threshold

    Returns
    -------
        blk  : RCPblk

    """
    
    if size(pin) != 1:
        raise ValueError("Block should have 1 input port; received %i !" % size(pin))

    blk = RCPblk('comedi_digital_output', pin, [], [0,0], 1, [thr], [ch], dev)
    return blk

