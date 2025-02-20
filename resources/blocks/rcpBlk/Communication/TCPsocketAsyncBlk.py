
from supsisim.RCPblk import RCPblk

def TCPsocketAsyncBlk(*args):
    if len(args) == 6:
        pin, pout, IP, port, buffer, downsampling = args
    elif len(args) == 5:
        pout = []
        pin, IP, port, buffer, downsampling = args

    """

    Call:   TCPsocketAsyncBlk(*args)

    Parameters
    ----------
       pin          : connected input port(s)
       pout         : connected output port(s)
       IP           : IP Addr
       port         : Port
       buffer       : size of send buffer
       downsampling : data is sent/received only per xth sample

    Returns
    -------
       blk: RCPblk

    """

    if downsampling < 1:
        raise ValueError("The downsampling ratio must be greater than zero!")

    # the current sample value must be included with the downsampling value
    blk = RCPblk('TCPsocketAsync', pin, pout, [0,0], 1, [], [port, buffer, 0, downsampling, 0], IP)
    return blk


