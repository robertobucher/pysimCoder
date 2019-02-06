from supsisim.RCPblk import RCPblk
from scipy import size

def FH_5XXX_INIT_VBlk(ID, Kp, Ki, rescnt):
    """

    Call:   FH_5XXX_INIT_VBlk(ID, Kp, Ki, rescnt)

    Parameters
    ----------
       ID : Device ID
       Kp : Prop. gain
       Ki : Intgr. gain
       rescnt: reset counter

    Returns
    -------
        blk  : RCPblk

    """    
    blk = RCPblk('FH_5XXX_INIT_V',[],[],[0,0],0,[Kp, Ki],[ID, rescnt])
    return blk

