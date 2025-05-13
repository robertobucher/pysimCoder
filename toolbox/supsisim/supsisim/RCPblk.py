"""
This is a procedural interface to the RCPblk library

roberto.bucher@supsi.ch
michallenc@seznam.cz
"""

from numpy import array, ones
import enum
from typing_extensions import Self  # from typing import Self for 3.11+


class RcpParam:
    """The represenation of RCP block's parameter"""

    class Type(enum.IntEnum):
        """The possible types of block's parameters"""

        DOUBLE = 0
        INT = 1
        STR = 2
        BOOL = 3
        UNKNOWN = 99

    class ShvFlag(enum.Flag):
        """Parameter's behaviour in respect to SHV tree"""

        HIDDEN = 0
        """The parameter is hidden from SHV interface.

        It is not possible to see the current value of the parameter nor
        to modify it.
        """

        VISIBLE = 1  # we would want enum.auto(), but | is forbidden for it in 3.10
        """Visible on SHV tree.

        The parameter will be visible in SHV tree, but not neccessary
        editable. This automatically allows method get on the parameter.
        """
        EDITABLE = 2 | VISIBLE
        """Editable parameter.

        The parameter is editable in SHV tree, therefore method set must
        be provided. Editable parameters are automatically visible as well.
        """
        SIGNAL = 4 | VISIBLE
        """Parameter generates signal on change.

        The parameter generates a SHV signal if its value is changed. The
        signal is generated as chng on method get. The parameter has to
        be visible to generate the signal. This is currently not supported
        in C implementation of SHV tree in pysimCoder.
        """

    def __init__(
        self,
        name: str,
        value: float | str | bool | list[float | str | bool] = 0,
        type: Type = Type.UNKNOWN,
        shv_flags: ShvFlag = ShvFlag.HIDDEN,
        is_list: bool = False,
    ) -> None:
        self.name = name
        self.value = value
        self.type = type
        self.shv_flags = shv_flags
        self.is_list = is_list

    def __lt__(self, other: Self) -> bool:
        return self.type < other.type

    def __str__(self) -> str:
        """String representation of the parameter"""
        str = (
            f" Name:         {self.name}\n"
            f"  Value:        {self.value}\n"
            f"  Type:         {self.type.__str__()}\n"
            f"  List:         {self.is_list}\n"
            f"  SHV Flags:    {self.shv_flags.__str__()}\n"
        )
        return str


class RCPblk:
    """The represenation of one RCP block"""

    def __init__(
        self,
        fcn: str,
        pin: list[int],
        pout: list[int],
        nx: list[int],
        uy: list[int],
        params: list[RcpParam] = [],
    ) -> None:
        self.name = None
        self.fcn = fcn
        self.pin = array(pin)
        self.pout = array(pout)
        self.dimPin = ones(self.pin.shape)
        self.dimPout = ones(self.pout.shape)
        self.nx = array(nx)
        self.uy = array(uy)
        self.sysPath = ""
        self.no_fcn_call = False
        self.params_list = params

    def __str__(self):
        """String representation of the Block"""
        str = (
            f"Block Name:        {self.name}\n"
            f"Function:          {self.fcn}\n"
            f"System path:       {self.sysPath}\n"
            f"Input ports:       {self.pin}\n"
            f"Output ports:      {self.pout}\n"
            f"Input dimensions:  {self.dimPin}\n"
            f"Output dimensions: {self.dimPout}\n"
            f"Num of states:     {self.nx}\n"
            f"Relations u->y:    {self.uy}\n"
            f"Parameters:\n"
        )
        for param in self.params_list:
            str += f"{param}"
        return str
