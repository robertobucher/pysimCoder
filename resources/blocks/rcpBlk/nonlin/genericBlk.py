from supsisim.RCPblk import RCPblk
from scipy import size

def genericBlk(pin, pout, nx, uy, rP, iP, strP, fname):
    """

    Call: genericBlk()

    Parameters
    ----------
        pin     : connected input ports
        pout    : connected output ports
        nx      : states [cont, disc]
        uy      : Feedforw input->output
        rP      : real parameters
        iP:     : integer parameters
        strP:   : Block string
        fname   : filename (implementation file .c)

    Returns
    -------
        blk  : RCPblk

    """
    
    blk = RCPblk(fname,pin,pout,nx,uy,rP,iP, strP)
    return blk

