from supsisim.RCPblk import RCPblk
from scipy import size

def sineBlk(pout, Amp, Freq, Phase, Bias, Delay):
    """

    Call:   sineBlk(pout, Amp, Freq, Phase, Bias, Delay)

    Parameters
    ----------
       pout: connected output port(s)
       Amp : Amplitude
       Freq : Freq
       Phase : Phase
       Bias : Bias
       Delay : Delay

    Returns
    -------
        blk  : RCPblk
    
    """

    if(size(pout) != 1):
        raise ValueError("Block should have 1 output port; received %i." % size(pout))
    blk = RCPblk('sinus', [],pout,[0,0],0,[Amp, Freq, Phase, Bias, Delay],[])
    return blk

