
from supsisim.RCPblk import RCPblk, RcpParam
from numpy import size

def shmemInBlk(pout: list[int], params: RcpParam) -> RCPblk:
    """

    Call:   shmemInBlk(pout, name)

    Parameters
    ----------
       pout: connected output port(s)
       name : SHM name

    Returns
    -------
       blk: RCPblk

    """

    params.append(RcpParam("File descriptor", 0, RcpParam.Type.INT))
    blk = RCPblk('shmemIn', [], pout, [0,0], 0, params)
    return blk
