from supsisim.RCPblk import RCPblk
from scipy import size

def epos_MotXBlk(pin, ID):
    """

    Call:   epos_MotXBlk(pin, ID)

    Parameters
    ----------
       pin: connected input port(s)
       ID : Device ID
       propGain : Prop. gain
       intGain : Integ. gain
       derGain : Deriv. gain
       Vff : Vel. FeedForw
       Aff : Acc. Feed Forw.

    Returns
    -------
        blk  : RCPblk

    """
    
    if(size(pin) != 1):
        raise ValueError("Block should have 1 input port; received %i." % size(pout))

    blk = RCPblk('epos_canopen_motX',pin,[],[0,0],1,[],[ID])
    return blk

