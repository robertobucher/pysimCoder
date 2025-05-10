from supsisim.RCPblk import RCPblk, RcpParam


def brikiAIBlk(pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   brikiAIBlk(pout, params)

    Parameters
    ----------
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    ADlist = [26, 27, 28, 29, 30, 31, 20]
    if params[0].value not in ADlist:
        raise ValueError(
            "You pass a channel which is not in the ABC Board PIN list: %i"
            % (params[0].value)
        )

    return RCPblk("brikiAI", [], pout, [0, 0], 0, params)
