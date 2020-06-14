from supsisim.RCPblk import RCPblk
from scipy import size

def scopeStream(pin, decim=1):
    """Create an interactive scope."""

    decim = int(decim)
    
    blk = RCPblk("scope", pin, [], [0,0], 1, [], [decim, 0])
    return blk

