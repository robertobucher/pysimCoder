from supsisim.RCPblk import RCPblk, RcpParam
from numpy import reshape, shape, size, asmatrix


def matmultBlk(pin: list[int], pout: list[int], params: RcpParam) -> RCPblk:
    """
    Matrix multiplication of the input signals

    Call: matmultBlk(pin, pout, params)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
        Block's reprezentation RCPblk
    """

    if isinstance(params[0].value, list):
        params[0].value = asmatrix(params[0].value)
        n, m = shape(params[0].value)
        if size(pin) != m:
            raise ValueError(
                "Block should have %i input port; received %i." % (m, size(pin))
            )
        if size(pout) != n:
            raise ValueError(
                "Block should have %i output port; received %i." % (n, size(pout))
            )
        params[0].value = reshape(params[0].value, (1, size(params[0].value)), "C")
    return RCPblk("mxmult", pin, pout, [0, 0], 1, params)
