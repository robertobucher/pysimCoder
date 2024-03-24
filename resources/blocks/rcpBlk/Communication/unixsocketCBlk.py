from supsisim.RCPblk import RCPblk
from numpy import size

def unixsocketCBlk(pin, sockname):
    """

    Call:   unixsocketCBlk(pin, sockname)

    Parameters
    ----------
       pin: connected input port(s)
       sockname : Socket

    Returns
    -------
        blk  : RCPblk

    """
    
    blk = RCPblk('unixsockC',pin,[],[0,0],1,[],[0],'/tmp/'+sockname)
    return blk

