from supsisim.RCPblk import RCPblk
from scipy import size

def loggerBlk(pin, log_file):
    """Wrtie pin to log_file."""
    blk = RCPblk("logger", pin, [], [0,0], 1, [], [], log_file)
    return blk

