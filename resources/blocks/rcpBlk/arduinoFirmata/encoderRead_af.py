
from supsisim.RCPblk import RCPblk
from numpy import size

def encoderRead_af(pout, encno, pinA, pinB):
    """

    Call:   encoderRead_af(pout, encno, pinA, pinB)

    Parameters
    ----------
       pout: connected output port(s)
       encno : Encode no
       pinA : Pin A
       pinB : Pin B

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('encoderRead_af', [], pout, [0,0], 0, [], [encno, pinA, pinB])
    return blk
