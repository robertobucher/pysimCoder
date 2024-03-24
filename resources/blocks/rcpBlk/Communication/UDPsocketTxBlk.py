from supsisim.RCPblk import RCPblk
from numpy import size

def UDPsocketTxBlk(pin, IP, port):
    """

    Call:   UDPsocketTxBlk(pin, IP, port)

    Parameters
    ----------
       pin: connected input port(s)
       IP : IP Addr
       port :  Port

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('UDPsocketTx', pin, [], [0,0], 1, [], [port, 0], IP)
    return blk

