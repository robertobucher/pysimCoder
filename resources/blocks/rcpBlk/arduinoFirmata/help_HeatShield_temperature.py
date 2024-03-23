
from supsisim.RCPblk import RCPblk
from numpy import size

def help_HeatShield_temperature(pin, pout, ADCREF, AREF, REF_TEMP, NTC_RES, VD_RES, VD_REF, NTC_BETA, ABSZERO):
    """

    Call:   help_HeatShield_temperature(pin, pout, ADCREF, AREF, REF_TEMP, NTC_RES, VD_RES, VD_REF, NTC_BETA, ABSZERO)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       ADCREF : adcref
       AREF : aref
       REF_TEMP : ref_temp
       NTC_RES : ntc_res
       VD_RES : vd_res
       VD_REF : vd_ref
       NTC_BETA : ntc_beta
       ABSZERO : abszero

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('help_HeatShield_temperature', pin, pout, [0,0], 0, [ADCREF, AREF, REF_TEMP, NTC_RES, VD_RES, VD_REF, NTC_BETA, ABSZERO], [])
    return blk
