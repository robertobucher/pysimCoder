from supsisim.RCPblk import RCPblk, RcpParam


def init_epos_MotIBlk(params: RcpParam) -> RCPblk:
    """
    Call:   init_epos_MotIBlk(params)

    Parameters
    ----------
       params: block's parameters

    Returns
    -------
        Block's reprezentation RCPblk
    """

    params[4].value = -1 * params[4].value
    return RCPblk("init_epos_Mot", [], [], [0, 0], 0, params)
