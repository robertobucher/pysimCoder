from supsisim.RCPblk import RCPblk
from scipy import size

def FH_5XXX_INIT_TQBlk(ID, Kp, Ti, rescnt):
    """

    Call:   FH_5XXX_INIT_VBlk(ID, Kp, Ki, rescnt)

    Parameters
    ----------
       ID : Device ID
       Kp : Prop. gain
       Ti : Intgr. time
       rescnt: reset counter

    Returns
    -------
        blk  : RCPblk

    """    
    blk = RCPblk('FH_5XXX_INIT_TQ',[],[],[0,0],0,[Kp, Ti],[ID, rescnt])
    return blk

