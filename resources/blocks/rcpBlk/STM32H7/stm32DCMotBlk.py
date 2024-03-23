
from supsisim.RCPblk import RCPblk
from numpy import size

def stm32DCMotBlk(pin, pwmCh, dir1Port, dir1Ch, dir2Port, dir2Ch, umin, umax):
    """

    Call:   stm32DCMotBlk(pin, pwmCh, dir1Port, dir1Ch, dir2Port, dir2Ch, umin, umax)

    Parameters
    ----------
       pin: connected input port(s)
       pwmCh : PWM CHannel
       dir1Port : Direction 1 Port
       dir1Ch : Direction 1 Channel
       dir2Port : Direction 2 Port
       dir2Ch : Direction 2 Channel
       umin : Umin 
       umax : Umax

    Returns
    -------
       blk: RCPblk

    """
    
    p1 = dir1Port[-1]
    p1 = p1.upper()
    pn1 = ord(p1) - ord('A')

    p2 = dir2Port[-1]
    p2 = p2.upper()
    pn2 = ord(p2) - ord('A')
    
    if pn1<0 or pn1 > 6:
        raise ValueError("Port should be between PORTA and PORTG not %s", (dir1Port))
    
    if dir1Ch<0 or dir1Ch > 15:
        raise ValueError("Channel should be between 0 and 15 not %i", (dir1Ch))
 
    if pn2<0 or pn2 > 6:
        raise ValueError("Port should be between PORTA and PORTG not %s", (dir2Port))
    
    if dir2Ch<0 or dir2Ch > 15:
        raise ValueError("Channel should be between 0 and 15 not %i", (dir2Ch))
 
    if pwmCh<1 or pwmCh>4:
        raise ValueError("Channel should have value 1 2 3  or 4 not %d" % pwmCh)
    
    blk = RCPblk('stm32DCMot', pin, [], [0,0], 1, [umin, umax], [pwmCh, pn1, dir1Ch, pn2, dir2Ch])
    
    return blk
