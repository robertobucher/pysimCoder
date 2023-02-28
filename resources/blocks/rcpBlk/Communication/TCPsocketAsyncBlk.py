
from supsisim.RCPblk import RCPblk

def TCPsocketAsyncBlk(*args):
    if len(args) == 5:
        pin, pout, IP, port, buffer = args
    elif len(args) == 4:
        pout = []
        pin, IP, port, buffer = args

    """

    Call:   TCPsocketAsyncBlk(*args)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       IP : IP Addr
       port :  Port
       buffer : size of send buffer

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('TCPsocketAsync', pin, pout, [0,0], 1, [], [port, buffer, 0], IP)
    return blk


