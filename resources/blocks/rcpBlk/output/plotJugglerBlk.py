from supsisim.RCPblk import RCPblk, RcpParam


def plotJugglerBlk(pin: list[int], params: RcpParam) -> RCPblk:
    """Create an interactive scope.
    Call:   plotJugglerBlk(pin, params)

    Parameters
    ----------
       pin: connected input port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    params.append(RcpParam("Internal", 0, RcpParam.Type.INT))
    return RCPblk("plotJuggler", pin, [], [0, 0], 1, params)
