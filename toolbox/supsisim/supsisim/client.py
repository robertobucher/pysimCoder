#!/usr/bin/env python3
import asyncio
from pydoc import locate
from threading import Thread

from shv import RpcUrl, SimpleClient, RpcError, SHVType


def _start_background_loop(loop: asyncio.AbstractEventLoop) -> None:
        asyncio.set_event_loop(loop)
        loop.run_forever()

async def _disconnect_client(client: SimpleClient):
    print("ASYNC Disconnecting to broker...")
    await client.disconnect()

async def _connect_client(user: str, addr: str, port: str, password: str) -> SimpleClient | None:
    print("ASYNC Connecting to broker...")
    url: RpcUrl = RpcUrl.parse("tcp://{}@{}:{}?password={}".format(user, addr, port, password))

    print("Connection to SHV broker: ", url.to_url())
    return await SimpleClient.connect(url)


async def _get_parameter_value(client: SimpleClient, mount_point:str, device_id: str, item: str, paramName: str) -> SHVType|None:
    callUrl = "{}/{}/blocks/{}/parameters/{}".format(mount_point, device_id, paramName, item)
    # print("Get param url: ", callUrl)
    try:
        result = await client.call(callUrl, "get") 
        # print("Read result: ", result)
        return result
    except RpcError as e:
        print("Can't read parameter ", paramName)
        print(e)
        return None

async def _set_parameter_value(client: SimpleClient, mount_point:str, device_id: str, item: str, paramName: str, paramVal: SHVType):
    callUrl = "{}/{}/blocks/{}/parameters/{}".format(mount_point, device_id, paramName, item)
    # print("Set param url: ", callUrl)
    try:
        return await client.call(callUrl, "set", paramVal) 
    except RpcError as e:
        print("Can't set parameter ", paramName)
        print(e)


class BrokerConnection:
    def __init__(self):
        self.asyncio_loop = asyncio.new_event_loop()
        self.iothread = loop = asyncio.new_event_loop()
        self.connection: SimpleClient | None = None
        self.client: SimpleClient | None = None
        self.addr: str | None = None
        self.port: str | None = None
        self.user: str | None = None
        self.password: str | None = None
        self.device_id: str | None = None
        self.mount_point: str | None = None
        self.connected: bool | False = False

        t = Thread(target=_start_background_loop, args=(self.asyncio_loop,), daemon=True)
        t.start()
        
    
    def _connect(self):
        if (self.addr is None):
            return
        print("Connecting to broker...")
        connection = asyncio.run_coroutine_threadsafe(_connect_client(self.user, self.addr, self.port, self.password), self.asyncio_loop).result()

        if connection is None:
            self.connected = False
            raise RuntimeError("Error while connecting to broker.")
        

        self.connection = connection
        self.connected = True

        print("Connected to broker.")

    def _disconnect(self):
        if self.connection is None:
            return
        print("Disconnecting from broker...")
        asyncio.run_coroutine_threadsafe(_disconnect_client(self.client), self.asyncio_loop).result()

        self.connection = None
        self.connected = False
        
        print("Disconnected from broker.")

    def _get_connection(self):
        if self.connection is None:
            self._connect()

        return self.connection

    def update_parameters_and_connect(self, addr: str, port: str, user: str, password: str, device_id: str, mount_point: str):

        are_values_updated = (self.addr != addr or 
             self.port != port or 
             self.user != user or 
             self.password != password or 
             self.device_id != device_id or
             self.mount_point != mount_point)

        
        self.addr = addr
        self.port = port
        self.user = user
        self.password = password
        self.device_id = device_id
        self.mount_point = mount_point

        print("Updating connection parameters")
        if are_values_updated and self.connection is not None:
            print("Disconnecting...")
            self._disconnect()

        if self.connection is None:
            print("Connecting...")
            self._connect()

    def getParameterValue(self, item: str, paramName: str) -> SHVType|None:
        if self.connection is None:
            return
        print("GetParam get connection")
        self.client = self._get_connection()
        print("GetParam call getParam async..")
        return asyncio.run_coroutine_threadsafe(
            _get_parameter_value(self.client, self.mount_point, self.device_id, item, paramName), 
            self.asyncio_loop
            ).result()

    def setPrameterValue(self, item: str, paramName: str, paramVal: SHVType):
        if self.connection is None:
            return
        self.client = self._get_connection()
        asyncio.run_coroutine_threadsafe(
            _set_parameter_value(self.client, self.mount_point, self.device_id, item, paramName, paramVal), 
            self.asyncio_loop
            ).result()

