import os
import socket
import tempfile
import threading

def _sockPath(server_address):
    # Socket files live in the host temporary directory
    return os.path.join(tempfile.gettempdir(), server_address)

class unixSrvRecv(threading.Thread):
    def __init__(self, ch, nBytes):
        threading.Thread.__init__(self)
        self.ch = ch
        self.N = nBytes
        self.GO = True
        self.data = b'\x00'
       
    def run(self):
        while self.GO:
            self.data = self.ch.recv(self.N)

class unixServerSK():
    def __init__(self, server_address, nBytes):
        self.server_address = server_address
        try:
            os.unlink(_sockPath(server_address))
        except:
            pass

        self.sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        self.sock.bind(_sockPath(server_address))
        self.sock.listen(1)
        self.connection, self. client_address = self.sock.accept()
        self.th = unixSrvRecv(self.connection, nBytes)
        self.th.start()

    def getData(self):
        return self.th.data
    
    def close(self):
        self.connection.close()
        try:
            os.unlink(_sockPath(self.server_address))
        except:
            pass

class unixClientSK():
    def __init__(self,  server_address):
        self.sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)

        try:
            self.sock.connect(_sockPath(server_address))
        except(socket.error):
            print('Error connecting')

    def close(self):
        self.sock.close()

    def send(self, data):
         self.sock.sendall(data)
        
class UDPSrvRecv(threading.Thread):
    def __init__(self, sock, nBytes):
        threading.Thread.__init__(self)
        self.sock = sock
        self.N = nBytes
        self.GO = True
        self.data = b'\x00'*nBytes
       
    def run(self):
        while self.GO:
            self.data, _ = self.sock.recvfrom(self.N)

class UDPServerSK():
    def __init__(self, server_address, port, nBytes):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

        self.sock.bind((server_address, port))
        self.th = UDPSrvRecv(self.sock, nBytes)
        self.th.start()

    def getData(self):
        return self.th.data
    
    def close(self):
        self.sock.close()

class UDPClientSK():
    def __init__(self,  server_address, port):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.server_address =(server_address, port)
        
    def close(self):
        self.sock.close()

    def send(self, data):
         sent = self.sock.sendto(data, self.server_address)
        
