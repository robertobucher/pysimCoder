from supsisim.RCPblk import RCPblk
from scipy import size

def squareBlk(pout, Amp, Period, Width, Bias, Delay):
    """

    Call:   squareBlk(pout, Amp, Period, Width, Bias, Delay)

    Parameters
    ----------
       pout: connected output port(s)
       Amp : Amplitude
       Period : Period
       Width : Width
       Bias : Bias
       Delay : Delay

    Returns
    -------
        blk  : RCPblk

    """
    
    if(size(pout) != 1):
        raise ValueError("Block should have 1 output port; received %i." % size(pout))
    blk = RCPblk('squareSignal',[],pout,[0,0],0,[Amp, Period, Width, Bias, Delay],[])
    return blk

