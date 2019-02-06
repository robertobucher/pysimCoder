from supsisim.RCPblk import RCPblk
from scipy import size

def comediDABlk(pin, dev, ch, cr):
    """

    Call:   comediDABlk(pin, dev, ch, cr)

    Parameters
    ----------
       pin: connected input port(s)
       dev : Device
       ch : Channel
       cr : Range

    Returns
    -------
        blk  : RCPblk

    """
    
    if size(pin) != 1:
        raise ValueError("Block should have 1 input port; received %i !" % size(pin))

    blk = RCPblk('comedi_analog_output',pin,[],[0,0],1,[],[ch, cr],dev)
    return blk

