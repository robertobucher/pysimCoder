#!/usr/bin/python

import sys

from qwt import QwtPlot, QwtPlotCurve, QwtPlotGrid

try:
    from PyQt5 import QtGui, QtCore, uic
    from PyQt5.QtGui import *
    from PyQt5.QtCore import *
    from PyQt5.QtWidgets import QMainWindow, QApplication, QAction, QFileDialog, \
        QMessageBox, QLabel, QLineEdit, QPushButton, QTableWidget, QTableWidgetItem
except:
    print("ERROR: PyQT5 not found")
    exit(1)
    from qwt.qt import QtGui, QtCore, uic
    from qwt.qt.QtGui import *
    from qwt.qt.QtCore import *

import time
import threading
import os
import numpy as np
import struct
import serial as ser
import socket

COL = 220
WIDTH = 2

SER = 1
SER4 = 2
TCP = 3
UDP = 4

from qwt import QwtPlot, QwtPlotCurve, QwtPlotGrid

path = os.environ.get('PYSUPSICTRL') + '/BlockEditor'
form_class = uic.loadUiType(path + '/pyplt.ui')[0]    

class ser_rcvServer(threading.Thread):
    def __init__(self, mainw):
        threading.Thread.__init__(self)
        self.mainw = mainw
        self.N = self.mainw.N
        self.st = struct.Struct(self.N*'d')
       
    def run(self):
        portN =  self.mainw.serCbBox.currentIndex()
        portName = self.mainw.serCbBox.itemText(portN)

        self.port = ser.Serial(portName, 1200000)
        T = 0.0
        L = 8*self.N
        
        while self.mainw.ServerActive==1:
            self.mainw.timebase.append(T);
            T+=1
            
            if len(self.mainw.timebase) > self.mainw.Hist:
                self.mainw.timebase = self.mainw.timebase[-self.mainw.Hist:]
                
            val = self.port.read(L)
            data = self.st.unpack(val)
            
            for n in range(0,self.N):
                try:
                    val = float(data[n])
                except:
                    val = 0.0
                self.mainw.x[n].append(val)
                if len(self.mainw.x[n]) > self.mainw.Hist:
                    self.mainw.x[n] = self.mainw.x[n][-self.mainw.Hist:]

class ser_rcvServer4bytes(threading.Thread):
    def __init__(self, mainw):
        threading.Thread.__init__(self)
        self.mainw = mainw
        self.N = self.mainw.N
        self.st = struct.Struct(self.N*'d')
       
    def run(self):
        portN =  self.mainw.serCbBox.currentIndex()
        portName = self.mainw.serCbBox.itemText(portN)

        self.port = ser.Serial(portName, 1200000)
        T = 0.0
        L = 4*self.N
        
        while self.mainw.ServerActive==1:
            self.mainw.timebase.append(T);
            T+=1
            
            if len(self.mainw.timebase) > self.mainw.Hist:
                self.mainw.timebase = self.mainw.timebase[-self.mainw.Hist:]
                
            val = self.port.read(L)
            data = self.st.unpack(val)
            
            for n in range(0,self.N):
                try:
                    val = float(data[n])
                except:
                    val = 0.0
                self.mainw.x[n].append(val)
                if len(self.mainw.x[n]) > self.mainw.Hist:
                    self.mainw.x[n] = self.mainw.x[n][-self.mainw.Hist:]

class tcp_rcvServer(threading.Thread):
    def __init__(self, mainw):
        threading.Thread.__init__(self)
        self.mainw = mainw
        self.N = self.mainw.N
        self.st = struct.Struct(self.N*'d')

    def run(self):
        portN =  self.mainw.tcpCbBox.currentIndex()
        portNum = int(self.mainw.tcpCbBox.itemText(portN))

        self.port = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.port.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        T = 0.0
        L = 8*self.N
        self.port.bind(('', portNum))
        self.port.listen(5)
        
        while self.mainw.ServerActive==1:
            conn, addr = self.port.accept()
            while True:
                self.mainw.timebase.append(T)
                T+=1

                if len(self.mainw.timebase) > self.mainw.Hist:
                    self.mainw.timebase = self.mainw.timebase[-self.mainw.Hist:]

                buf = bytearray(conn.recv(L))
                if (len(buf) == 0):
                    conn.close()
                    break

                data = self.st.unpack(buf)
                for n in range(0,self.N):
                    try:
                        val = float(data[n])
                    except:
                        val = 0.0
                    self.mainw.x[n].append(val)
                    if len(self.mainw.x[n]) > self.mainw.Hist:
                        self.mainw.x[n] = self.mainw.x[n][-self.mainw.Hist:]

class udp_rcvServer(threading.Thread):
    def __init__(self, mainw):
        threading.Thread.__init__(self)
        self.mainw = mainw
        self.N = self.mainw.N
        self.st = struct.Struct(self.N*'d')

    def run(self):
        portN =  self.mainw.udpCbBox.currentIndex()
        portNum = int(self.mainw.udpCbBox.itemText(portN))

        self.port = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.port.bind(('0.0.0.0', portNum))
        T = 0.0
        L = 8*self.N
        
        while self.mainw.ServerActive==1:
            while True:
                self.mainw.timebase.append(T)
                T+=1

                if len(self.mainw.timebase) > self.mainw.Hist:
                    self.mainw.timebase = self.mainw.timebase[-self.mainw.Hist:]

                buf, addr = self.port.recvfrom(L)
                
                if (len(buf) == 0):
                    conn.close()
                    break

                data = self.st.unpack(buf)
                for n in range(0,self.N):
                    try:
                        val = float(data[n])
                    except:
                        val = 0.0
                    self.mainw.x[n].append(val)
                    if len(self.mainw.x[n]) > self.mainw.Hist:
                        self.mainw.x[n] = self.mainw.x[n][-self.mainw.Hist:]

class dataPlot(QwtPlot):
    def __init__(self, N):
        QwtPlot.__init__(self)
        self.setTitle('Time signals')
        self.setCanvasBackground(QBrush(QColor(COL, COL, COL)))
        grid = QwtPlotGrid()
        pen = QPen(Qt.black, 0, Qt.DashDotLine)
        grid.setPen(pen)
        grid.attach(self)
                                
class MainWindow(QMainWindow, form_class):
    def __init__(self):
        QMainWindow.__init__(self)
        self.setupUi(self)
          
        self.connect_widget()

        self.ServerActive = 0
        self.colors = ["red", "green", "blue","yellow", "cyan", "magenta", "white", "gray"]
        self.ymin = -1
        self.ymax = 1
        self.autoAxis = True

    def connect_widget(self):
        self.pbStart_ser.clicked.connect(lambda: self.pbServerClicked(SER))
        self.pbStart_ser4.clicked.connect(lambda: self.pbServerClicked(SER4))
        self.pbStart_tcp.clicked.connect(lambda: self.pbServerClicked(TCP))
        self.pbStart_udp.clicked.connect(lambda: self.pbServerClicked(UDP))

        self.edHist.textEdited.connect(self.edHistEdited)
        self.ckAutoscale.stateChanged.connect(self.setAutoscale)
        self.edYmax.editingFinished.connect(self.YAxes)
        self.edYmin.editingFinished.connect(self.YAxes)

    def edHistEdited(self, val):
        self.Hist = int(val.__str__())

    def setAutoscale(self):
        if self.ckAutoscale.isChecked():
            self.autoAxis = True
            self.label_5.setEnabled(False)
            self.label_6.setEnabled(False)
            self.edYmin.setEnabled(False)
            self.edYmax.setEnabled(False)
        else:
            self.autoAxis = False
            self.label_5.setEnabled(True)
            self.label_6.setEnabled(True)
            self.edYmin.setEnabled(True)
            self.edYmax.setEnabled(True)
 
    def YAxes(self):
        self.ymax = float(self.edYmax.text())
        self.ymin = float(self.edYmin.text())
        
    def pbServerClicked(self, porttype):
        if self.ServerActive == 0:
            self.N = self.sbNsig.value()
            self.Hist = int(self.edHist.text().__str__())
            
            if porttype == SER:
                self.pbStart_ser.setText('Stop Server')
            elif porttype == SER4:
                self.pbStart_ser4.setText('Stop Server')
            elif porttype == TCP:
                self.pbStart_tcp.setText('Stop Server')
            else:
                self.pbStart_udp.setText('Stop Server')
            self.ServerActive = 1
            
            self.plot = dataPlot(self.N)
            self.plot.resize(800, 500)
            self.plot.show()
            
            self.timebase = []
            self.x = []
            self.c = []
            for n in range(0, self.N):
                self.x.append([])
                cv = QwtPlotCurve()
                pen = QPen(QColor(self.colors[n % 8]))
                pen.setWidth(WIDTH)
                cv.setPen(pen)
                cv.setData([],[])
                self.c.append(cv)
                self.c[n].attach(self.plot)
                
            self.timer = QtCore.QTimer()
            self.timer.timeout.connect(self.pltRefresh)
            refTimer = self.sbRefT.value()
            self.timer.start(refTimer)
            
            if porttype == SER:
                self.th = ser_rcvServer(self)
            elif porttype == SER4:
                self.th = ser_rcvServer4bytes(self)
            elif porttype == TCP:
                self.th = tcp_rcvServer(self)
            else:
                 self.th = udp_rcvServer(self)
               
            self.th.start()
        else:
            if porttype == SER:
                self.pbStart_ser.setText('Start Server')
            elif porttype == TCP:
                self.pbStart_tcp.setText('Start Server')
            else:
                self.pbStart_udp.setText('Start Server')
            self.ServerActive = 0
            self.stopServer()

    def stopServer(self):
        self.timer.stop()

    def pltRefresh(self):
        if len(self.x[0]) > self.Hist:
            for n in range(0,self.N):
                self.x[n] = self.x[n][-self.Hist:]
            self.timebase = self.timebase[-self.Hist:]
        if(len(self.timebase)>2):
            self.plot.setAxisScale(QwtPlot.xBottom,self.timebase[0],self.timebase[-1])
        
        for n in range(0,self.N):
            try:
                t = self.timebase[0:len(self.x[n])]
                self.c[n].setData(t,self.x[n])
            except:
                pass
        if self.autoAxis:
            self.plot.setAxisAutoScale(QwtPlot.yLeft)
            self.plot.replot()
        else:
            self.plot.setAxisScale(QwtPlot.yLeft, self.ymin, self.ymax)
            self.plot.replot()

app = QApplication(sys.argv)
frame = MainWindow()
frame.show()
sys.exit(app.exec_())


