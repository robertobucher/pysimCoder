from supsisim.RCPblk import RCPblk

def plotJugglerBlk(pin, IP, port):
    """Create an interactive scope.
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

    blk = RCPblk("plotJuggler", pin, [], [0,0], 1, [], [port, 0], IP)
    return blk

