
from supsisim.RCPblk import RCPblk
from scipy import size

def FH_3XXX_INIT_Blk(candev, id, Kp_pos, Kd_pos, Kp_v, Ki_v, encres):
    """

    Call:   FH_3XXX_INIT_Blk(id, Kp_pos, Kd_pos, Kp_v, Ki_v, encres)

    Parameters
    ----------
        : Device ID
       Kp_pos : Pos. Prop. gain
       Kd_pos : Pos. Deriv. gain
       Kp_v : Prop. gain
       Ki_v : Integ. gain
       encres :  Encoder reset (1->Yes, 0->No)

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('FH_3XXX_INIT', [], [], [0,0], 0, [Kp_pos, Kd_pos, Kp_v, Ki_v], [id, encres], candev)
    return blk
