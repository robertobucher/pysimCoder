#!/usr/bin/env python3
import argparse
import asyncio
import logging
from os import environ

from shv import RpcUrl, SimpleClient

log_levels = (
    logging.DEBUG,
    logging.INFO,
    logging.WARNING,
    logging.ERROR,
    logging.CRITICAL,
)


async def example_client(url: RpcUrl, item, name) -> None:
    client = await SimpleClient.connect(url)
    assert client is not None
    res = await client.call("test/untitled/blocks/{}/parameters/{}".format(name, item), "get") 
    
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
