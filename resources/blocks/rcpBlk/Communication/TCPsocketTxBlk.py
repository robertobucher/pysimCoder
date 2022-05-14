
from supsisim.RCPblk import RCPblk
from scipy import size

# def TCPsocketTxRxBlk(pin, pout, IP, port):
def TCPsocketTxRxBlk(*args):
    if len(args) == 4:
        pin, pout, IP, port = args
    elif len(args) == 3:
        pout = []
        pin, IP, port = args

    """

    Call:   TCPsocketTxRxBlk(pin, pout, IP, port)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       IP : IP Addr
       port :  Port

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('TCPsocketTxRx', pin, pout, [0,0], 1, [], [port, 0], IP)
    return blk


