#!/usr/bin/python3

import sys
import os
import socket
import numpy as np
import struct

from supsisim.qtvers import *
import threading

import warnings
warnings.filterwarnings("ignore", category=DeprecationWarning)

# globals
SOCKET_NAME = sys.argv[1]
CONNECTION_TRIES = 9999 # just in case
NIN = int(sys.argv[2]);
DOUBLE_SIZE = 8
PACKET_LEN = NIN * DOUBLE_SIZE
BASE_DIM = 30

GRAY = Qt.GlobalColor.gray
RED = Qt.GlobalColor.red
BLACK = Qt.GlobalColor.black

WIDTH = 3*BASE_DIM
HEIGHT = BASE_DIM+NIN* 2*BASE_DIM

class MyWidget(QMainWindow):
    def __init__(self):
        QMainWindow.__init__(self)
        self.setWindowTitle("LED")
        self.N = NIN
        self.leds = np.zeros(NIN)
        self.th = rcv_data(self)

        self.th.start()

    def paintEvent(self, event):
        painter = QPainter(self)
        for n in range(0, NIN):
            if self.leds[n] >= 0.5:
                painter.setBrush(RED)
            else:
                painter.setBrush(GRAY)
            painter.setPen(QPen(BLACK, 3))
            painter.drawEllipse(BASE_DIM, BASE_DIM+2*BASE_DIM*n, BASE_DIM, BASE_DIM)

    def closeEvent(self,event):
        sock.close()

# connect to model
sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
for i in range(CONNECTION_TRIES):
    try:
        sock.connect(SOCKET_NAME)
        break;
    except OSError as msg:
        os.write(2, str.encode(msg))

class rcv_data(threading.Thread):
    def __init__(self, mainw):
        threading.Thread.__init__(self)
        self.mainw = mainw
        self.N = self.mainw.N
        self.st = struct.Struct(self.N*'d')

    def run(self):
        while(True):
            try:
                data = sock.recv(PACKET_LEN, socket.MSG_WAITALL)
                if not data:
                    # other end closed connection
                    sock.close()
                    return

                self.mainw.leds = struct.unpack('d' * NIN , data)
                self.mainw.update()
            except:
                pass
            
app = QApplication([])
frame = MyWidget()
frame.resize(WIDTH, HEIGHT)

frame.show()
app.exec()
