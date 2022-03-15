from supsisim.RCPblk import RCPblk
from numpy import size, hstack

def stm32SPIBlk(pin, pout, port, css,  outs):
    """

    Call:   stm32SPIBlk(pin, pout, port, pin, outs)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       port : CS Port
       pin : CS PIN
 
    Returns
    -------
       blk: RCPblk

    """
    if port == 'NSS':
        pn = -1
        css = 0
    else:
        p = port[-1]
        p = p.upper()
        pn = ord(p) - ord('A')

    if size(pout) != size(outs):
        raise ValueError("Number of outputs should fit the len of Outputs")
        
    if pn<-1 or pn > 6:
        raise ValueError("Port should be between PORTA and PORTG not %s", (port))
    
    if css<0 or css > 15:
        raise ValueError("CSS should be between 0 and 15 not %i", (pin))

    intP = hstack((pn, css, outs))
 
    blk = RCPblk('stm32SPI', pin, pout, [0,0], 1, [], intP)
    return blk
