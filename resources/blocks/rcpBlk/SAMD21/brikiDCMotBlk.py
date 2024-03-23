
from supsisim.RCPblk import RCPblk
from numpy import size

def brikiDCMotBlk(pin, enCh, in1Ch, in2Ch):
    """

    Call:   brikiDCMotBlk(pin, enCh, in1Ch, in2Ch)

    Parameters
    ----------
       pin: connected input port(s)
       enCh : Enable Pin
       in1Ch : In1 Pin
       in2Ch : In2 Pin

    Returns
    -------
       blk: RCPblk

    """
    CHlist = [11, 13, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31]
    if enCh not in CHlist:
        raise ValueError("You pass a channel which is not in the ABC Board PIN list: %i" % (enCh))
    if in1Ch not in CHlist:
        raise ValueError("You pass a channel which is not in the ABC Board PIN list: %i" % (in1Ch))
    if in2Ch not in CHlist:
        raise ValueError("You pass a channel which is not in the ABC Board PIN list: %i" % (in2Ch))

    blk = RCPblk('brikiDCMot', pin, [], [0,0], 1, [], [enCh, in1Ch, in2Ch])
    return blk
