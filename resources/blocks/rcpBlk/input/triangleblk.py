from supsisim.RCPblk import RCPblk
from numpy import size

def triangleBlk(pout, amp, period, bias, delay):
    """

    Call:   triangleBlk(pout, amp, period, bias, delay):

    Parameters
    ----------
       pout   : connected output port(s)
       amp    : Amplitude
       period : Period [s]
       bias   : Bias
       delay  : Delay

    Returns
    -------
        blk  : RCPblk
    
    """

    if(size(pout) != 1):
        raise ValueError("Block should have 1 output port; received %i." % size(pout))

    blk = RCPblk('triangle', [], pout, [0, 0], 0, [amp, period, bias, delay], [])
    return blk

