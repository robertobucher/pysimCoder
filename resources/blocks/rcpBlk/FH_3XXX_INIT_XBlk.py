from supsisim.RCPblk import RCPblk
from scipy import size

def FH_3XXX_INIT_XBlk(ID, Kp, Kd, rescnt):
    """

    Call:   FH_3XXX_INIT_XBlk(ID, Kp, Kd, rescnt)

    Parameters
    ----------
       ID : Device ID
       Kp : Prop. gain
       Kd : Deriv. gain
       rescnt: reset counter

    Returns
    -------
        blk  : RCPblk

    """    
    blk = RCPblk('FH_3XXX_INIT_X',[],[],[0,0],0,[Kp, Kd],[ID, rescnt])
    return blk

