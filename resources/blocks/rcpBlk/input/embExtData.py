from supsisim.RCPblk import RCPblk, RcpParam
import numpy as np

def embExtDataBlk(pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   embExtDataBlk(pout, params)

    Parameters
    ----------
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    fname = params[0].value

    data = np.loadtxt(fname)
    
    N = data.shape[0]
    ch = data.shape[1]

    if np.size(pout) != ch:
        raise ValueError(
            "Block should have %i output port; received %i."
            % (ch, size(pout))
        )

    params[0] = RcpParam("len", N, RcpParam.Type.INT,0, True)
    params.append(RcpParam('ind', 0, RcpParam.Type.INT, 0, True))
    
    for i in range(0, N):
        for j in range(0, ch):
            st = 's' + str(j)
            params.append(RcpParam('st', data[i,j], RcpParam.Type.DOUBLE, 0, True))
 
    return RCPblk("embExtData", [], pout, [0, 0], 0, params)
