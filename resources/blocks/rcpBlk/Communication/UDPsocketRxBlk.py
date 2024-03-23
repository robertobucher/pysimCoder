from supsisim.RCPblk import RCPblk
from numpy import size

def UDPsocketRxBlk(pout, IP, port):
    """

    Call:   UDPsocketRxBlk(pout, IP, port)

    Parameters
    ----------
       pout: connected output port(s)
       IP : IP Addr
       port :  Port

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('UDPsocketRx', [], pout, [0,0], 0, [], [port, 0], IP)
    return blk

