from supsisim.RCPblk import RCPblk
from numpy import size

def init_epos_MotIBlk(candev, ID, propGain, intGain, mode):
    """

    Call:   init_epos_MotIBlk(ID, propGain, intGain)

    Parameters
    ----------
       ID : Device ID
       propGain : Prop. gain
       intGain : Integ. gain

    Returns
    -------
        blk  : RCPblk

    """
    blk = RCPblk('init_epos_Mot',[],[],[0,0],0,[propGain, intGain],[ID, -1*mode], candev)
    
    return blk

