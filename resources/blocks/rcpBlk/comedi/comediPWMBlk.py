from supsisim.RCPblk import RCPblk
from scipy import size

def comediPWMBlk(pin, devicename, channel, prescaler, pfiout):
    """Create Comedi_PWM block 

    Comedi_PWM
    
    Call:  ComediPWMBlk(pin, devicename, channel, prescaler, pfiout)

    Parameters
    ----------
    pin : connected input port
    devicename: Comedi device name /path
    channel : Channel
    prescaler: Clock Prescaler
    pfiout: Output pin

    Returns
    -------
    blk  : RCPblk
    """
    if size(pin)!=1:
        raise ValueError("Block should have only 1 input port, receivesd %i" % size(pin))
    
    blk = RCPblk("comedi_pwm_generator", pin, [], [0, 0], 1, [], [channel, prescaler, pfiout], devicename)
    return blk
    

