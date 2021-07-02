
from supsisim.RCPblk import RCPblk
from scipy import size

def TCPsocketTxBlk(pin, IP, port):
    """

    Call:   TCPsocketTxBlk(pin, IP, port)

    Parameters
    ----------
       pin: connected input port(s)
       IP : IP Addr
       port :  Port

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('TCPsocketTx', pin, [], [0,0], 1, [], [port, 0], IP)
    return blk
