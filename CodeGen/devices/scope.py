#!/usr/bin/python

import sys
import os
import socket
import numpy as np
import pyqtgraph as pg
from pyqtgraph import QtGui, QtCore

# globals
SOCKET_NAME = sys.argv[1]
CONNECTION_TRIES = 9999 # just in case
PACKET_NUM = int(sys.argv[2])
NIN = int(sys.argv[3]);
PLOT_LEN = 2048
DOUBLE_SIZE = 8
PACKET_LEN = NIN * PACKET_NUM * DOUBLE_SIZE
PLOT_LINE_COLORS = ['y', 'r', 'g', 'b', 'c', 'm', 'k', 'w']
PLOT_WINDOM_SIZE = (1200, NIN*200)
TIMER_PERIOD = 50

# connect to model
sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
for i in range(CONNECTION_TRIES):
    try:
        sock.connect(SOCKET_NAME)
        break;
    except OSError as msg:
        os.write(2, str.encode(msg))

# some plot related stuff
app = QtGui.QApplication([])
win = pg.GraphicsWindow(title="Scope")
win.resize(PLOT_WINDOM_SIZE[0], PLOT_WINDOM_SIZE[1])
pg.setConfigOptions(antialias=True, useWeave=True)
plots = []
curves = []
# add plots for all inputs to blk
for i in range(NIN):
    p = win.addPlot(title="u"+str(i))
    p.showGrid(x=True, y=True)
    c = PLOT_LINE_COLORS[i % len(PLOT_LINE_COLORS)]
    plots.append(p)
    curves.append(p.plot(pen=c))
    win.nextRow()

tdata = np.zeros(shape=(NIN, PLOT_LEN))
# time bounds, s.t. we start at 0 Delta
t = 1 - PLOT_LEN

def update():
    """Will receive data from model and plot it."""
    import struct
    global t, curves, tdata, PACKET_NUM, NIN, timer
    data = sock.recv(PACKET_LEN, socket.MSG_WAITALL)
    if not data:
        # other end closed connection
        sock.close()
        timer.stop()        
        return
        
    t += PACKET_NUM
    data = list(struct.unpack('d' * (NIN * PACKET_NUM), data))
    tdata = np.roll(tdata, -PACKET_NUM)
    for j in range(NIN):
        tdata[j][-PACKET_NUM:] = data[j::NIN]
        curves[j].setData(tdata[j])
        curves[j].setPos(t, 0)

timer = QtCore.QTimer()
timer.timeout.connect(update)
timer.start(TIMER_PERIOD)

if __name__ == "__main__":
    import sys
    if (1 != sys.flags.interactive) or not hasattr(QtCore, "PYQT_VERSION"):
        QtGui.QApplication.instance().exec_()
