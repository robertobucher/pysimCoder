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
from typing import Callable, Optional, Any
from supsisim.shv.client import ShvFwUpdateClient, ShvCallError
from supsisim.shv.SHVInstance import SHVInstance

import asyncio
import subprocess
import shlex
import zlib
import io

class ImageUpdateMethod(ABC):
    def __init__(self, name: str, path_to_img: str):
        self.name: str = name
        self.path_to_img = path_to_img

    @abstractmethod
    async def upload(self, queue: asyncio.Queue) -> None:
        """
        Perform the upload.
        Each time an event happens (a succesful step or failure)
        it is reported using the supplied callback.
        """
        pass

    @abstractmethod
    async def stop_upload(self) -> None:
        """
        Stop the upload.
        """
        pass

class OpenocdUpdateMethod(ImageUpdateMethod):
    def __init__(self, params: str, path_to_img: str):
        super().__init__("openocd", path_to_img)
        self.params = params

    async def s1(self, params) -> [bool, str, str]:
        completed = subprocess.run(params, capture_output=True, text=True, timeout=20)
        if completed.returncode == 0:
            return [True, completed.stdout, completed.stderr]
        return [False, completed.stdout, completed.stderr]

    async def upload(self, queue: asyncio.Queue) -> None:
        """
        Flash using openocd.
        """
        if "@PYSIM_IMG@" not in self.params:
            queue.put_nowait("No @PYSIM_IMG@ in params found.")
            queue.put_nowait(False)
            return
        self.params = self.params.replace("@PYSIM_IMG@", self.path_to_img)

        params = ["openocd"] + shlex.split(self.params)
        ret = await self.s1(params)
        if not ret[0]:
            queue.put_nowait("Launching openocd failed. Output:")
            if ret[1] != "":
                queue.put_nowait("STDOUT:\n" + ret[1])
            if ret[2] != "":
                queue.put_nowait("STDERR:\n" + ret[2])
            queue.put_nowait(False)
            return

        queue.put_nowait("Flashed succesfully. Output:")
        queue.put_nowait(ret[1])
        queue.put_nowait(True)

    async def stop_upload(self) -> None:
        print("STOP")

class SHVUpdateMethod(ImageUpdateMethod):
    def __init__(self, path_to_img: str, shvclient: ShvFwUpdateClient, shvparams):
        super().__init__("SHV Update", path_to_img)
        self.shvclient = shvclient
        self.shvparams = shvparams
        self.maxwrite = 1024

    def _upload_existing_file_quit(self, queue: asyncio.Queue) -> None:
        queue.put_nowait(False)
        self.shvclient.disconnect()

    async def _upload_existing_file(self, queue: asyncio.Queue) -> None:
        stat_params: Optional
        self.shvclient.update_parameters_and_connect(
            self.shvparams.ip,
            self.shvparams.port,
            self.shvparams.user,
            self.shvparams.passw,
            self.shvparams.devid,
            self.shvparams.mount
        )
        if not self.shvclient.is_connected():
            queue.put_nowait("Error connecting to the SHV broker!")
            queue.put_nowait(False)
            return

        queue.put_nowait("Connected to the SHV broker!")
        stat_params = self.shvclient.stat_file()
        if isinstance(stat_params, ShvCallError):
            queue.put_nowait("Cannot call stat on the device!")
            self._upload_existing_file_quit(queue)
            return
        elif not isinstance(stat_params, dict):
            queue.put_nowait("Excepted stat result to be dict, got:" + str(type(stat_params)))
            self._upload_existing_file_quit(queue)
            return

        queue.put_nowait(f"Stopping the control loop")
        self.shvclient.pause_ctrl()

        # Now wait for the loop to stop (do polling)
        poll_retries = 10
        while poll_retries > 0:
            ctrlstate = self.shvclient.get_ctrlstate()
            if isinstance(ctrlstate, int) and ctrlstate == 0:
                break
            await asyncio.sleep(1)
            poll_retries -= 1

        if poll_retries == 0:
            queue.put_nowait(f"The uploader could not stop the control loop.")
            queue.put_nowait(f"But let's try anyway.")

        queue.put_nowait(f"Maximum file size {stat_params[1]}.")
        # Check maximum file writes
        if 5 in stat_params:
            queue.put_nowait(f"Maximum enabled write size {stat_params[5]}.")
            self.maxwrite = int(stat_params[5])
        else:
            queue.put_nowait("Maximum enabled write size not specified. Defaulting to 1024 bytes.")

        self.f.seek(0, io.SEEK_END)
        self.fsize = self.f.tell()
        self.f.seek(0, io.SEEK_SET)

        totalchunks = self.fsize // self.maxwrite + 1
        queue.put_nowait(f"File's size is {self.fsize}.")
        queue.put_nowait(f"New image shall be uploaded in {totalchunks} chunks.")
        await asyncio.sleep(0)

        chunk = 0
        crc = 0
        while data := self.f.read(self.maxwrite):
            crc = zlib.crc32(data, crc)
            offset = chunk * self.maxwrite
            res = self.shvclient.write_chunk(data, offset)
            if (chunk % 5 == 0) or chunk == totalchunks-1:
                queue.put_nowait(f"Uploaded chunk [{chunk+1}/{totalchunks}]")
                await asyncio.sleep(0)
            chunk += 1

        queue.put_nowait("Waiting for CRC calculation.")
        await asyncio.sleep(0)
        reccrc = self.shvclient.get_crc(0, self.fsize)
        if isinstance(reccrc, ShvCallError):
            queue.put_nowait(f"Failure calling crc on the device!")
            self._upload_existing_file_quit(queue)
            return

        queue.put_nowait(f"Computed image's checksum {hex(crc)}, received {hex(reccrc)}.")
        if crc != reccrc:
            queue.put_nowait("Checksum mismatch! Repeat the upload!")
            self._upload_existing_file_quit(queue)
            return

        queue.put_nowait("CRC check OK, performing reset on the remote device.")
        queue.put_nowait("The remote device now performs fw update. This will take some while.")
        queue.put_nowait("After the update, you need to check firmware is running. And confirm it.")
        await asyncio.sleep(0)
        self.shvclient.reset_device()

        # All OK, signal successful upload.
        queue.put_nowait(True)

    async def upload(self, queue: asyncio.Queue) -> None:
        # Before any connections, do the sanity check of existing file ...
        try:
            self.f = open(self.path_to_img, mode="rb")
            await self._upload_existing_file(queue)
            self.f.close()
        except FileNotFoundError:
            queue.put_nowait(f"Cannot find. Generate code first!")
            queue.put_nowait(False)
        
    async def stop_upload(self) -> None:
        pass

