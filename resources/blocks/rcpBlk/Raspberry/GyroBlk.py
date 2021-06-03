from supsisim.RCPblk import RCPblk

def GyroBlk(pout, Cx, Cy, Cz):
    """

    Call:   AccelBlk(pout, Cx, Cy, Cz)

    Parameters
    ----------
       pout: connected output port(s)
       Cx:    Correction factor X
       Cy:    Correction factor Y
       Cz:    Correction factor Z

    Returns
    -------
        blk  : RCPblk

    """
    
    blk = RCPblk('ImuGyro',[],pout,[0,0],0,[Cx, Cy, Cz],[0])
    return blk

