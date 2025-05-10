from supsisim.RCPblk import RCPblk, RcpParam
from control import tf2ss, TransferFunction
from numpy import reshape, hstack, asmatrix, shape, size, zeros


def dssBlk(pin: list[int], pout: list[int], params: RcpParam) -> RCPblk:
    """
    Discrete state space block

    Call: dssBlk(pin,pout, sys,X0)

    Parameters
    ----------
       pin : connected input ports
       pout: connected output ports
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    sys = params[0].value
    if isinstance(sys, TransferFunction):
        sys = tf2ss(sys)

    nin = size(pin)
    ni = shape(sys.B)[1]
    if nin != ni:
        raise ValueError("Block have %i inputs: received %i input ports" % (nin, ni))

    no = shape(sys.C)[0]
    nout = size(pout)
    if no != nout:
        raise ValueError("Block have %i outputs: received %i output ports" % (nout, no))

    a = reshape(sys.A, (1, size(sys.A)), "C")
    b = reshape(sys.B, (1, size(sys.B)), "C")
    c = reshape(sys.C, (1, size(sys.C)), "C")
    d = reshape(sys.D, (1, size(sys.D)), "C")
    nx = shape(sys.A)[0]

    x0 = params[1].value
    if size(x0) == nx:
        x0 = reshape(x0, (1, size(x0)), "C")
    else:
        x0 = asmatrix(zeros((1, nx)))

    indA = 0
    indB = nx * nx
    indC = indB + nx * ni
    indD = indC + nx * no
    indX = indD + ni * no
    arrays = hstack((a, b, c, d, x0))

    params = [
        RcpParam("nx", nx, RcpParam.Type.INT),
        RcpParam("ni", ni, RcpParam.Type.INT),
        RcpParam("no", no, RcpParam.Type.INT),
        RcpParam("indA", indA, RcpParam.Type.INT),
        RcpParam("indB", indB, RcpParam.Type.INT),
        RcpParam("indC", indC, RcpParam.Type.INT),
        RcpParam("indD", indD, RcpParam.Type.INT),
        RcpParam("indX", indX, RcpParam.Type.INT),
    ]

    names = ["A", "B", "C", "D", "x0"]
    for array, i in zip(arrays, range(len(arrays))):
        params.append(RcpParam(names[i], array, RcpParam.Type.DOUBLE, 0, True))

    if d.any():
        uy = 1
    else:
        uy = 0

    blk = RCPblk("dss", pin, pout, [0, nx], uy, params)
    return blk
