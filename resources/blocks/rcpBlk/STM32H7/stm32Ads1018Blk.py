
from supsisim.RCPblk import RCPblk
from scipy import size

def stm32Ads1018Blk(pout, port, pin, channel, rng, single):
    """

    Call:   stm32Ads1018Blk(pout, port, pin, channel)

    Parameters
    ----------
       pout: connected output port(s)
       port : CS Port
       pin : CS PIN
       channel : Channel

    Returns
    -------
       blk: RCPblk

    """
    if port == 'NSS':
        pn = -1
        pin = 0
    else:
        p = port[-1]
        p = p.upper()
        pn = ord(p) - ord('A')
        
        if pn<0 or pn > 6:
            raise ValueError("Port should be between PORTA and PORTG not %s", (port))
    
        if pin<0 or pin > 15:
            raise ValueError("PIN should be between 0 and 15 not %i", (pin))
       
    if channel<0 or channel > 3:
        raise ValueError("PIN should be between 0 and 3 not %i", (channel))
        
    if rng<2 or rng > 7:
        raise ValueError("Voltage range  should be between 2 and 7 not %i", (rng))

    if single>0:
        single = 1
    else:
        single = 0
        
    blk = RCPblk('stm32Ads1018', [], pout, [0,0], 0, [0], [pn, pin, channel, rng, single])
    return blk
