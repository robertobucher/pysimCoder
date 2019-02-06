from supsisim.RCPblk import RCPblk
from scipy import size

def init_maxon_MotBlk(ID, propGain, intGain):
    """

    Call:   init_maxon_MotIBlk(ID, propGain, intGain)

    Parameters
    ----------
       ID : Device ID
       propGain : Prop. gain
       intGain : Integ. gain

    Returns
    -------
        blk  : RCPblk

    """
    
    blk = RCPblk('init_maxon_Mot',[],[],[0,0],0,[propGain, intGain],[ID])
    
    return blk

