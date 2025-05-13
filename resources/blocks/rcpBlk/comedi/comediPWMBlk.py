from supsisim.RCPblk import RCPblk, RcpParam
from numpy import size


def comediPWMBlk(pin: list[int], params: RcpParam) -> RCPblk:
    """Create Comedi_PWM block

    Comedi_PWM

    Call:  ComediPWMBlk(pin, params)

    Parameters
    ----------
        pin : connected input port
        params: block's parameters

    Returns
    -------
        Block's reprezentation RCPblk
    """

    if size(pin) != 1:
        raise ValueError("Block should have only 1 input port, received %i" % size(pin))

    return RCPblk("comedi_pwm_generator", pin, [], [0, 0], 1, params)
