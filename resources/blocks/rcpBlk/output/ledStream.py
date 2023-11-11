from supsisim.RCPblk import RCPblk

def ledStream(pin, decim=1):
    """Create an interactive LED."""
   
    decim = int(decim)

    blk = RCPblk("led", pin, [], [0,0], 1, [], [decim, 0])
    return blk

