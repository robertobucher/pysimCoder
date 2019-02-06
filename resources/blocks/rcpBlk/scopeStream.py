from supsisim.RCPblk import RCPblk
from scipy import size

def scopeStream(pin):
    """Create an interactive scope."""
    blk = RCPblk("scope", pin, [], [0,0], 1, [], [])
    return blk

