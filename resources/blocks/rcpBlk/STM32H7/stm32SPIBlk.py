
from supsisim.RCPblk import RCPblk
from scipy import size

def stm32SPIBlk(pin, pout, port, css):
    """

    Call:   stm32SPIBlk(pin, pout, port, pin, rxtx, index)

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
        
        if pn<0 or pn > 6:
            raise ValueError("Port should be between PORTA and PORTG not %s", (port))
    
        if css<0 or css > 15:
            raise ValueError("CSS should be between 0 and 15 not %i", (pin))

    print('pn: ', pn)
    print('css: ', css)

    blk = RCPblk('stm32SPI', pin, pout, [0,0], 1, [], [pn, css])
    return blk
