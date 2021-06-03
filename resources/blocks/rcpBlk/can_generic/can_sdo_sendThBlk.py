from supsisim.RCPblk import RCPblk
from scipy import size

def can_sdo_sendThBlk(pin, candev, ID, index, subindex, data, useInp):
    """

    Call:   can_sdo_sendThBlk(pin, pout, ID, index, subindex, data, useInp)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       ID : Device ID
       index : Index
       subindex : SubIndex
       data :  Data
       useInp :  Use Input [0/1 no/yes]

    Returns
    -------
        blk: RCPblk

    """
    
    blk = RCPblk('can_sdo_send', pin, [], [0,0], 1, [], [ID, index, subindex, data, useInp], candev)
    return blk

