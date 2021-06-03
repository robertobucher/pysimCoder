from supsisim.RCPblk import RCPblk
from scipy import size

def epos_MotIBlk(pin, candev, ID):
    """

    Call:   epos_MotIBlk(pin, ID)

    Parameters
    ----------
       pin: connected input port(s)
       ID : Device ID
       propGain : Prop. gain
       intGain : Integ. gain

    Returns
    -------
        blk  : RCPblk

    """
    if(size(pin) != 1):
        raise ValueError("Block should have 1 input port; received %i." % size(pout))

    blk = RCPblk('epos_canopen_motI',pin,[],[0,0],1,[],[ID], candev)
    return blk

