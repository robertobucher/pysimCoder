"""
This is a procedural interface to the unixsocket.py library

roberto.bucher@supsi.ch

The following commands are provided:
    open_server   -  Create a unix server
    close_server  -  Close the unix server
  
"""

import os
import sys
import socket

def open_server(server_address):
    """Create and open a Socket Unix Server

    Call:
    sock, conn, client = open_server(server_address)

    Parameters
    ----------
    server address : string with the unix socket address (ex. '/tmp/bucher_sock') 

    Returns
    -------
    sock   : socket
    conn   : connection
    client : client address

    """

    try:
        os.unlink(server_address)
    except OSError:
        if os.path.exists(server_address):
            raise

    # Create a UDS socket
    sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)

    # Bind the socket to the port
    sock.bind(server_address)

    # Listen for incoming connections
    sock.listen(1)

    connection, client_address = sock.accept()

    return sock, connection, client_address

def close_server(connection, server_address):
    """Close the Unix Socket Server

    Call:
    close_server(connection, server_address)

    Parameters
    ----------
    connection     : connection
    server address : string with the unix socket address (ex. '/tmp/bucher_sock') 

    Returns
    -------
    -
    
    """
    connection.close()
    try:
        os.unlink(server_address)
    except OSError:
        if os.path.exists(server_address):
            raise

def open_client(server_address):
    """Create and open a Socket Unix Client

    Call:
    sock = open_client(server_address)

    Parameters
    ----------
    server address : string with the unix socket address (ex. '/tmp/bucher_sock') 

    Returns
    -------
    sock   : socket

    """

    # Create a UDS socket
    sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)

    try:
        sock.connect(server_address)
    except(socket.error):
        print('Error connecting')
        sys.exit(1)

    return sock

def close_client(sock, server_address):
    """Close the Unix Socket Client

    Call:
    close_client(sock, server_address)

    Parameters
    ----------
    sock  : socket
    server address : string with the unix socket address (ex. '/tmp/bucher_sock') 

    Returns
    -------
    -
    
    """
    sock.close()
    ## try:
    ##     os.unlink(server_address)
    ## except OSError:
    ##     pass
#        if os.path.exists(server_address):
#            raise

    


    
