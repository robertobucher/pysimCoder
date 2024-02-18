#!/usr/bin/python3

import sys
import os
import socket
import numpy as np

from PyQt5 import QtWidgets, QtCore
import pyqtgraph as pg

COL = 170
PENWIDTH = 2.0
pg.setConfigOption('background', pg.mkColor((COL, COL, COL)))
pg.setConfigOption('foreground', 'k')

# globals
SOCKET_NAME = sys.argv[1]
CONNECTION_TRIES = 9999 # just in case
PACKET_NUM = int(sys.argv[2])
NIN = int(sys.argv[3]);
DT = float(sys.argv[4]);
PLOT_LEN = 2048

if DT != 1:
    PLOT_LEN = int(20/DT)
    
DOUBLE_SIZE = 8
PACKET_LEN = NIN * PACKET_NUM * DOUBLE_SIZE
PLOT_LINE_COLORS = ['y', 'g', 'r', 'b', 'c', 'm', 'k', 'w']
PLOT_WINDOM_SIZE = (1000, 600)
TIMER_PERIOD = 20

# connect to model
sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
for i in range(CONNECTION_TRIES):
    try:
        sock.connect(SOCKET_NAME)
        break;
    except OSError as msg:
        os.write(2, str.encode(msg))

# some plot related stuff
app = QtWidgets.QApplication([])
#win = pg.GraphicsWindow(title="Scope")
win = pg.GraphicsLayoutWidget(title="Scope")
win.resize(PLOT_WINDOM_SIZE[0], PLOT_WINDOM_SIZE[1])
pg.setConfigOptions(antialias=True)
plots = []
curves = []
p = win.addPlot(title="u"+str(i))
plots.append(p)
# add plots for all inputs to blk
for i in range(NIN):
    p.showGrid(x=True, y=True)
    c = PLOT_LINE_COLORS[i % len(PLOT_LINE_COLORS)]
    curves.append(p.plot(pen={'color':c, 'width' : PENWIDTH}))
    win.nextRow()
win.show()

xdata = np.zeros(shape=(NIN, PLOT_LEN))
ydata = np.zeros(shape=(NIN, PLOT_LEN))

# time bounds, s.t. we start at 0 Delta
t = -PLOT_LEN
x = np.arange(-PLOT_LEN, 0)*DT
for i in range(NIN):
    xdata[i][-PLOT_LEN:] = x


def update():
    """Will receive data from model and plot it."""
    import struct
    global t, curves, xdata, ydata, PACKET_NUM, NIN, timer
    data = sock.recv(PACKET_LEN, socket.MSG_WAITALL)
    if not data:
        # other end closed connection
        sock.close()
        timer.stop()
        return

    data = list(struct.unpack('d' * (NIN * PACKET_NUM), data))
    x=(np.arange(0,PACKET_NUM)+PLOT_LEN+t)*DT
    t += PACKET_NUM
    xdata = np.roll(xdata, -PACKET_NUM)
    ydata = np.roll(ydata, -PACKET_NUM)

    for j in range(NIN):
        ydata[j][-PACKET_NUM:] = data[j::NIN]
        xdata[j][-PACKET_NUM:] = x
        curves[j].setData(xdata[j], ydata[j])

timer = QtCore.QTimer()
timer.timeout.connect(update)
timer.start(TIMER_PERIOD)

if __name__ == "__main__":
    import sys
    if (1 != sys.flags.interactive) or not hasattr(QtCore, "PYQT_VERSION"):
        QtWidgets.QApplication.instance().exec_()
