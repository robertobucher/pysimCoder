from supsisim.RCPblk import RCPblk


def mz_apo_3pmdrv1(pin: list[int], pout: list[int]) -> RCPblk:
    """
    Call:   mz_apo_3pmdrv1(pin, pout)

    Parameters
    ----------
       pin : connected input ports
       pout: connected output ports

    Returns
    -------
      Block's reprezentation RCPblk
    """

    return RCPblk("zynq_3pmdrv1", pin, pout, [0, 0], 0)
