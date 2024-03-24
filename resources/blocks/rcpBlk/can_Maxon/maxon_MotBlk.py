from supsisim.RCPblk import RCPblk
from numpy import size

def maxon_MotBlk(pin, candev, ID, propGain, intGain):
    """

    Maxon driver for torque control

    Call: maxon_MotBlk(pin, ID)

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

    blk = RCPblk('maxon_mot',pin,[],[0,0],1,[propGain, intGain],[ID], candev)
    return blk

