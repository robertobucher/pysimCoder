from supsisim.RCPblk import RCPblk, RcpParam


def ar2inoSetupBlk(params: RcpParam) -> RCPblk:
    """
    Call:   ar2inoSetup(params)

    Parameters
    ----------
      params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    return RCPblk("ar2inoSetup", [], [], [0, 0], 0, params)
