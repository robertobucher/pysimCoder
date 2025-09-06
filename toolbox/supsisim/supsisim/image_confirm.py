#!/usr/bin/env python3
# Copyright (c) 2025 Stepan Pressl <pressl.stepan@gmail.com>
# Credits given to Michal Lenc's base script, serving as inspiration
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

from abc import ABC, abstractmethod
from supsisim.shv.client import ShvFwUpdateClient
from supsisim.shv.SHVInstance import SHVInstance

import asyncio

class ImageConfirmMethod(ABC):
    def __init__(self, name: str):
        self.name: str = name

    @abstractmethod
    async def confirm(self) -> None:
        """
        Perform the confirm.
        """
        pass

class SHVConfirmMethod(ImageConfirmMethod):
    def __init__(self, shvclient: ShvFwUpdateClient, shvparams):
        super().__init__("SHV Confirm")
        self.shvclient = shvclient
        self.shvparams = shvparams

    async def confirm(self, queue: asyncio.Queue) -> None:
        try:
            self.shvclient.update_parameters_and_connect(
                self.shvparams.ip,
                self.shvparams.port,
                self.shvparams.user,
                self.shvparams.passw,
                self.shvparams.devid,
                self.shvparams.mount
            )
        except:
            queue.put_nowait("Error connecting to the SHV broker!")
            queue.put_nowait(False)
            return

        queue.put_nowait("Confirming the image on fwStable/confirm.")
        await asyncio.sleep(0)
        ret = self.shvclient.confirm_image()

        # default fwStable/confirm response
        if ret is None:
            queue.put_nowait(True)
        elif isinstance(ret, str):
            queue.put_nowait(f"Response from device: {ret}")
            queue.put_nowait(False)

         