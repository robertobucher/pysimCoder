
from supsisim.RCPblk import RCPblk
from numpy import size

def stm32DOBlk(pin, port, ch, Th):
    """

    Call:   stm32_OBlk(pin, port, ch, Th)

    Parameters
    ----------
       pin: connected input port(s)
       port : Port
       ch : Channel [0-15]
       Th : Threshold

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
    
    blk = RCPblk('stm32DO', pin, [], [0,0], 1, [Th], [pn, ch])
    return blk
