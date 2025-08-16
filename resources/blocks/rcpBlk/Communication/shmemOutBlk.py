
from supsisim.RCPblk import RCPblk, RcpParam
from numpy import size

def shmemOutBlk(pin: list[int], params: RcpParam) -> RCPblk:
    """

    Call:   shmemOutBlk(pin, name)

    Parameters
    ----------
       pin: connected input port(s)
       name : SHM name

    Returns
    -------
       blk: RCPblk

    """

    params.append(RcpParam("File descriptor", 0, RcpParam.Type.INT))
    blk = RCPblk('shmemOut', pin, [], [0,0], 1, params)
    return blk
