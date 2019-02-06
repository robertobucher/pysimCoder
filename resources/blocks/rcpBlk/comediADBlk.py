from supsisim.RCPblk import RCPblk
from scipy import size

def comediADBlk(pout, dev, ch, cr):
    """

    Call:   comediADBlk(pout, dev, ch, cr)

    Parameters
    ----------
       pout: connected output port(s)
       dev : Device
       ch : Channel
       cr : Range

    Returns
    -------
        blk  : RCPblk

    """
    
    if size(pout) != 1:
        raise ValueError("Block should have 1 input port; received %i !" % size(pout))

    blk = RCPblk('comedi_analog_input',[],pout,[0,0],0,[],[ch, cr],dev)
    return blk

