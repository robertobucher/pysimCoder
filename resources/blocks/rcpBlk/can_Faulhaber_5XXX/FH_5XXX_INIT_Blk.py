
from supsisim.RCPblk import RCPblk
from scipy import size

def FH_5XXX_INIT_Blk(candev, id, Kp_pos, Kd_pos, Kp_v, Ki_v, Kp_tq, Ki_tq, encres):
    """

    Call:   FH_5XXX_INIT_Blk(id, Kp_pos, Kd_pos, Kp_v, Ki_v, Kp_tq, Ki_tq, encres)

    Parameters
    ----------
       id : Device ID
       Kp_pos : Pos Prop. gain
       Kd_pos : Pos Deriv. gain
       Kp_v : V Prop. gain
       Ki_v : V Intg gain
       Kp_tq : TQ Prop. gain
       Ki_tq : TQ Intg gain
       encres :  Encoder reset (1->Yes, 0->No)

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('FH_5XXX_INIT', [], [], [0,0], 0, [Kp_pos, Kd_pos, Kp_v, Ki_v, Kp_tq, Ki_tq], [id, encres], candev)
    return blk
