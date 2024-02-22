#!/usr/bin/python3

import sys
import os

try:
    from PyQt5.Qwt import QwtPlot, QwtPlotCurve, QwtPlotGrid
    
except:
    print("Warning: PyQt5.Qwt not found, trying qwt")
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

path = os.environ.get('PYSUPSICTRL') + '/BlockEditor'
form_class = uic.loadUiType(path + '/pyplt.ui')[0]

class ser_rcvServer(threading.Thread):
    def __init__(self, mainw):
        threading.Thread.__init__(self)
        self.mainw = mainw
        self.N = self.mainw.N
        self.st = struct.Struct(self.N*'d')
        self.daemon = True
       
    def run(self):
        portN =  self.mainw.serCbBox.currentIndex()
        portName = self.mainw.serCbBox.itemText(portN)
        baudN = self.mainw.serBaudRate.currentIndex()
        baudRate = self.mainw.serBaudRate.itemText(baudN)

        self.port = ser.Serial(portName, baudRate)
        self.mainw.port = self.port
        T = 0.0
        L = 8*self.N
        
        while self.mainw.ServerActive==1:
            val = self.port.read(L)
            data = self.st.unpack(val)
            
            self.mainw.setData(data)

class ser_rcvServer4bytes(threading.Thread):
    def __init__(self, mainw):
        threading.Thread.__init__(self)
        self.mainw = mainw
        self.N = self.mainw.N
        self.st = struct.Struct(self.N*'d')
        self.daemon = True
       
    def run(self):
        portN =  self.mainw.ser4CbBox.currentIndex()
        portName = self.mainw.ser4CbBox.itemText(portN)
        baudN = self.mainw.ser4BaudRate.currentIndex()
        baudRate = self.mainw.ser4BaudRate.itemText(baudN)

        self.port = ser.Serial(portName, baudRate)
        self.mainw.port = self.port
        T = 0.0
        L = 4*self.N
        
        while self.mainw.ServerActive==1:
            val = self.port.read(L)
            data = self.st.unpack(val)
            
            self.mainw.setData(data)
class tcp_rcvServer(threading.Thread):
    def __init__(self, mainw):
        threading.Thread.__init__(self)
        self.mainw = mainw
        self.N = self.mainw.N
        self.st = struct.Struct(self.N*'d')
        self.daemon = True

    def run(self):
        portN =  self.mainw.tcpCbBox.currentIndex()
        portNum = int(self.mainw.tcpCbBox.itemText(portN))

        self.port = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.mainw.port = self.port
        
        self.port.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
       
        T = 0.0
        L = 8*self.N
        try:
            self.port.bind(('', portNum))
            self.port.listen(5)
        except:
            ret = QMessageBox.warning(self.mainw, '', 'Port already in use, please close it',
                                      QMessageBox.Ok, QMessageBox.Ok)
            return

        while self.mainw.ServerActive==1:
            conn, addr = self.port.accept()
            buf = bytearray()
            while True:
                chunk = bytearray(conn.recv(L - len(buf)))
                if (len(chunk) == 0):
                    conn.close()
                    break
                buf.extend(chunk)
                if len(buf) < L:
                    continue

                data = self.st.unpack(buf)
                buf = bytearray()
                self.mainw.setData(data)
            
class udp_rcvServer(threading.Thread):
    def __init__(self, mainw):
        threading.Thread.__init__(self)
        self.mainw = mainw
        self.N = self.mainw.N
        self.st = struct.Struct(self.N*'d')
        self.daemon = True

    def run(self):
        portN =  self.mainw.udpCbBox.currentIndex()
        portNum = int(self.mainw.udpCbBox.itemText(portN))

        self.port = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.mainw.port = self.port
        
        try:
           self.port.bind(('0.0.0.0', portNum))
        except:
            ret = QMessageBox.warning(self.mainw, '', 'Port already in use, please close it',
                                      QMessageBox.Ok, QMessageBox.Ok)
            return
        L = 8*self.N
        while self.mainw.ServerActive==1:
            while True:
                try:
                    buf, addr = self.port.recvfrom(L)
#                     info = 'recvfrom ' + len(buf).__str__()
                except:
                    pass
                                    
                if (len(buf) == 0):
                    conn.close()
                    break
                data = self.st.unpack(buf)
                self.mainw.setData(data)
           
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
        self.setFixedSize(675, 369)
        
        self.connect_widget()
        self.port = None
  
        self.ServerActive = 0
        self.colors = ["red", "green", "blue","yellow", "cyan", "magenta", "white", "gray"]
        self.ymin = -1
        self.ymax = 1
        self.autoAxis = True
        self.filename = 'data.txt'
 
    def connect_widget(self):
        self.pbStart_ser.clicked.connect(lambda: self.pbServerClicked(SER))
        self.pbStart_ser4.clicked.connect(lambda: self.pbServerClicked(SER4))
        self.pbStart_tcp.clicked.connect(lambda: self.pbServerClicked(TCP))
        self.pbStart_udp.clicked.connect(lambda: self.pbServerClicked(UDP))

        self.edHist.textEdited.connect(self.edHistEdited)
        self.ckAutoscale.stateChanged.connect(self.setAutoscale)
        self.ckSaveData.stateChanged.connect(self.setSaveData)
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
 
    def setData(self, data):
        if self.ckSaveData.isChecked():
            self.saveData(data)
        
        if self.ckTimeEnabled.isChecked():
            self.x = np.roll(self.x,-1)
            self.x[-1] = data[0]
            dN = 1
            
        else:
            Xe = self.x[-1]+1
            self.x = np.roll(self.x, -1)
            self.x[-1] = Xe
            dN = 0
            
        for n in range(0, self.NSig):
            try:
                val = float(data[n])
            except:
                val = 0.0
                
            self.y[n] = np.roll(self.y[n],-1)
            self.y[n][-1] = data[n+dN]
        
    def setSaveData(self):
        if self.ckSaveData.isChecked():
            filename = QFileDialog.getSaveFileName(self, 'Save',
                                                   self.filename, filter='*.txt')
            filename = filename[0]
            if filename != '':
                self.f = open(filename, 'w')
                self.filename = filename
                self.lnFilename.setText(filename)
                self.N = self.sbNsig.value()
                self.T0 = 0
            else:
                self.ckSaveData.setCheckState(False)
        else:
            try:
                self.f.close()
            except:
                pass
        
    def saveData(self, data):
        strData = self.T0.__str__() +'\t'
        for n in range(0,self.N):
            try:
                val = float(data[n])
                strData += val.__str__() + '\t'
            except:
                strData += '0.0\t'
                
        strData += '\n'
        self.f.write(strData)
        self.T0 += 1
 
    def YAxes(self):
        self.ymax = float(self.edYmax.text())
        self.ymin = float(self.edYmin.text())
        
    def pbServerClicked(self, porttype):
        if self.ServerActive == 0:
            self.N = self.sbNsig.value()
            self.NSig = self.N
            self.Hist = int(self.edHist.text())
            self.plot = dataPlot(self.N)
            if self.ckTimeEnabled.isChecked():
                self.x = np.arange(-self.Hist,0)*0.01
                self.N +=1
            else:
                self.x = np.arange(-self.Hist,0)
            
            self.y = np.zeros((self.NSig, self.Hist))
            
            if porttype == SER:
                self.pbStart_ser.setText('Stop Server')
            elif porttype == SER4:
                self.pbStart_ser4.setText('Stop Server')
            elif porttype == TCP:
                self.pbStart_tcp.setText('Stop Server')
            else:
                self.pbStart_udp.setText('Stop Server')
            self.ServerActive = 1
            
            self.plot = dataPlot(self.NSig)
            self.plot.resize(800, 500)
            self.plot.show()
            
            self.c = []
            for n in range(0, self.NSig):
                cv = QwtPlotCurve()
                pen = QPen(QColor(self.colors[n % 8]))
                pen.setWidth(WIDTH)
                cv.setPen(pen)
                cv.setSamples(self.x,self.y[n])
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
        self.plot.setAxisScale(QwtPlot.xBottom, self.x[0], self.x[-1]);            
        if self.autoAxis:
            self.plot.setAxisAutoScale(QwtPlot.yLeft)
            
        for n in range(0, self.NSig):
            self.c[n].setSamples(self.x,self.y[n])            
            
        self.plot.replot()

    def closeEvent(self,event):          
        try:
            self.port.shutdown()
        except:
            print('Shutdown failed')
            
        try:
            self.port.close()
        except:
            print('Port close failed')
       
        if self.ckSaveData.isChecked():
            self.f.close()

        event.accept()
                    
app = QApplication(sys.argv)
frame = MainWindow()
frame.show()
sys.exit(app.exec_())


