from supsisim.RCPblk import RCPblk
from scipy import size

def unixsocketSBlk(pout, sockname, defvals):
    """

    Call:   unixsocketSBlk(pout, sockname, defvals)

    Parameters
    ----------
       pout: connected output port(s)
       sockname : Socket
       defvals : Default outputs

    Returns
    -------
        blk  : RCPblk

    """
    
    outputs = len(pout)
    vals = zeros(outputs,float)
    if len(defvals) > outputs:
        N=outputs
    else:
        N = len(defvals)

    for n in range(N):
        vals[n]=defvals[n]
        
    blk = RCPblk('unixsockS',[],pout,[0,0],0,vals,[0,0],'/tmp/'+sockname)
    return blk

