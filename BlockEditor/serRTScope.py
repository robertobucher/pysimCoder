#!/usr/bin/python

import sys

from qwt.qt import QtGui, QtCore, uic
from qwt.qt.QtGui import *
from qwt.qt.QtCore import *

import time
import threading
import os
import numpy as np
import serial as ser
import struct

COL = 220
WIDTH = 2

from qwt import QwtPlot, QwtPlotCurve, QwtPlotGrid

path = os.environ.get('PYSUPSICTRL') + '/BlockEditor'
form_class = uic.loadUiType(path + '/pyplt.ui')[0]    

class rcvServer(threading.Thread):
    def __init__(self, mainw):
        threading.Thread.__init__(self)
        self.mainw = mainw
        self.N = self.mainw.N
        self.st = struct.Struct(self.N*'d')
       
    def run(self):
        portN =  self.mainw.devCbBox.currentIndex()
        portName = self.mainw.devCbBox.itemText(portN)

        self.port = ser.Serial(portName, 12000000)
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
        self.pbStartServer.clicked.connect(self.pbServerClicked)
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
        
    def pbServerClicked(self):
        if self.ServerActive == 0:
            self.N = self.sbNsig.value()
            self.Hist = int(self.edHist.text().__str__())
            self.pbStartServer.setText('Stop Server')
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
            self.th = rcvServer(self)
            self.th.start()
        else:
            self.pbStartServer.setText('Start Server')
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

app = QtGui.QApplication(sys.argv)
frame = MainWindow()
frame.show()
sys.exit(app.exec_())

