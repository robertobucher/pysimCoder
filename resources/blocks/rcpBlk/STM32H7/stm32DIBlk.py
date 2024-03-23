
from supsisim.RCPblk import RCPblk
from numpy import size

def stm32DIBlk(pout, port, ch):
    """

    Call:   stm32DIBlk(pout, port, ch)

    Parameters
    ----------
       pout: connected output port(s)
       port : Port
       ch : Channel

    Returns
    -------
       blk: RCPblk

    """

    p = port[-1]
    p = p.upper()

    pn = ord(p) - ord('A')

    if pn<0 or pn > 6:
        raise ValueError("Port should be between PORTA and PORTG not %s", (port))
    
    if ch<0 or ch > 15:
        raise ValueError("Channel should be between 0 and 15 not %i", (ch))
    
    blk = RCPblk('stm32DI', [], pout, [0,0], 0, [], [pn, ch])
    
    return blk
