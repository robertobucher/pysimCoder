#!/usr/bin/env python3
import argparse
import asyncio
import logging
from os import environ

from shv import RpcUrl, SimpleClient, RpcError


class BrokerConnection:
    def __init__(self):
        self.addr: str | None = None
        self.port: str | None = None
        self.user: str | None = None
        self.password: str | None = None
        self.device_id: str | None = None
        self.mount_point: str | None = None
        self.connection: SimpleClient | None = None


    async def _disconnect_client(self):
        await self.client.disconnect()

        self.client = None

    async def _connect_client(self):
        url: RpcUrl = RpcUrl.parse("tcp://{}@{}:{}?password={}".format(self.user, self.addr, self.port, self.password))

        print("Connection to SHV broker: ", url.to_url())
        self.connection = await SimpleClient.connect(url)

    def _connect(self):
        asyncio.run(self._connect_client())

        print("Connected to broker.")

        if self.connection is None:
            raise RuntimeError("Error while connecting to broker.")

    def _disconnect(self):
        asyncio.run(self._disconnect_client())

        if self.connection is not None:
            raise RuntimeError("Could not disconnect from broker.")

    def _getClient(self):
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

        if are_values_updated and self.connection is not None:
            self._disconnect()

        if self.connection is None:
            self._connect()

    async def _getParameterValue(self, item: str, paramName: str):
        callUrl = "{}/{}/blocks/{}/parameters/{}".format(self.mount_point, self.device_id, paramName, item)
        print("Get param url: ", callUrl)
        try:
            client = self._getClient()
            result = await client.call(callUrl, "get") 
            print("Read result: ", result)
            return result
        except RpcError:
            self._disconnect_client()
            self.connection = None

    async def _setParameterValue(self, item: str, paramName: str, paramVal):
        callUrl = "{}/{}/blocks/{}/parameters/{}".format(self.mount_point, self.device_id, paramName, item)
        print("Set param url: ", callUrl)
        try:
            client = self._getClient()
            return await client.call(callUrl, "set", paramVal) 
        except RpcError:
            self._disconnect_client()
            self.connection = None


    # async def _exampleStuff(self, item, name):
    #     callUrl = "{}/{}/blocks/{}/parameters/{}".format(self.mount_point, self.device_id, name, item)
    #     print("Example stuf, callUrls: ", callUrl)
    #     res = await self._getClient().call(callUrl, "get") 
    
    #     print(f"Gain received: {repr(res)}")
    #     return res

    def getParameterValue(self, item: str, paramName: str):
        return asyncio.run(self._getParameterValue(item, paramName))

    def setPrameterValue(self, item: str, paramName: str, paramVal):
        asyncio.run(self._setParameterValue(item, paramName, paramVal))

    def doExampleStuff(self, item, name):
        print("Do example stuff")
        # asyncio.run(self._exampleStuff(item, name))
        returnedVal = self.getParameterValue(item, name)
        print("Example returned val: ", returnedVal)




log_levels = (
    logging.DEBUG,
    logging.INFO,
    logging.WARNING,
    logging.ERROR,
    logging.CRITICAL,
)


async def example_client(url: RpcUrl, item, name) -> None:
    print("Example client url: ", url.to_url())

    client = await SimpleClient.connect(url)
    assert client is not None

    callUrl="test/myDeviceId/blocks/{}/parameters/{}".format(name, item)
    print("Example stuff call url: ", callUrl)
    res = await client.call(callUrl, "get") 
    
    print(f"Gain received: {repr(res)}")
    gain = int(res) + 10
    # res = await client.call("test/test_pyshv/blocks/{}/parameters/{}".format(name, item), "set", gain)
    # print(f"Gain set: {repr(res)}")
    await client.disconnect()
    return res


def parse_args(address, password) -> argparse.Namespace:
    """Parse passed arguments and return result."""
    parser = argparse.ArgumentParser(description="Silicon Heaven example client")
    parser.add_argument(
        "-v",
        action="count",
        default=0,
        help="Increase verbosity level of logging",
    )
    parser.add_argument(
        "-q",
        action="count",
        default=0,
        help="Decrease verbosity level of logging",
    )
    parser.add_argument(
        "URL",
        nargs="?",
        default="tcp://admin@{}?password={}".format(address, password),
        help="SHV RPC URL specifying connection to the broker.",
    )
    return parser.parse_args()


def run_client(address, pas, items, name):
    args = parse_args(address, pas)
    logging.basicConfig(
        level=log_levels[sorted([1 - args.v + args.q, 0, len(log_levels) - 1])[1]],
        format="[%(asctime)s] [%(levelname)s] - %(message)s",
    )
    return asyncio.run(example_client(RpcUrl.parse(args.URL), items, name))
