#!/usr/bin/python3

import sys
import os

from supsisim.qtvers import *

import time
import threading
import numpy as np
import struct
import serial as ser
import socket
import json

SER = 1
SER4 = 2
TCP = 3
UDP = 4

path = os.environ.get('PYSUPSICTRL') + '/BlockEditor'
form_class = uic.loadUiType(path + '/PlotJugglerIntf.ui')[0]

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
                                      QMessageBox.StandardButton.Ok, QMessageBox.StandardButton.Ok)
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
                                      QMessageBox.StandardButton.Ok, QMessageBox.StandardButton.Ok)
            return
        L = 8*self.N
        while self.mainw.ServerActive==1:
            while True:
                try:
                    buf, addr = self.port.recvfrom(L)
                    info = 'recvfrom ' + len(buf).__str__()
                except:
                    pass
                                    
                if (len(buf) == 0):
                    conn.close()
                    break
                    
                data = self.st.unpack(buf)
                self.mainw.setData(data)        

class MainWindow(QMainWindow, form_class):
    def __init__(self):
        QMainWindow.__init__(self)
        self.setupUi(self)
        self.setFixedSize(690, 325)
          
        self.connect_widget()        
        self.ServerActive = 0
 
    def connectJuggler(self):
        self._sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        
    def connect_widget(self):
        self.pbStart_ser.clicked.connect(lambda: self.pbServerClicked(SER))
        self.pbStart_ser4.clicked.connect(lambda: self.pbServerClicked(SER4))
        self.pbStart_tcp.clicked.connect(lambda: self.pbServerClicked(TCP))
        self.pbStart_udp.clicked.connect(lambda: self.pbServerClicked(UDP))
        self.ckTimeEnabled.stateChanged.connect(self.ckTimeEnabled_stateChanged)
        
    def setData(self, data):
        vals = np.round(data, 3)
        try:
            if self.ckTimeEnabled.isChecked():
                self.sendData[self.pjTs]=vals[0]
                self.sendData['y'] = list(vals[1:])
            else:
                self.sendData['y'] = list(vals)
                
            datas = json.dumps(self.sendData)
            datab = datas.encode('utf-8')
            self._sock.sendto(datab, (self._host, self._port))
        except:
            pass
        
    def pbServerClicked(self, porttype):
        if self.ServerActive == 0:    
            self.N = self.sbNsig.value()
            self.pjTs = self.ed_timeName.text()
            self._host = self.ed_PJ_IP.text()
            self._port = int(self.ed_PJ_Port.text())
        
            self.connectJuggler()        
            
            if self.ckTimeEnabled.isChecked():
                self.N += 1
                self.sendData = {self.pjTs: '0', 'y' : np.zeros(self.N)}
            else:
                self.sendData = {'y' : list(np.zeros(self.N))}        

            if porttype == SER:
                self.pbStart_ser.setText('Stop Server')
            elif porttype == SER4:
                self.pbStart_ser4.setText('Stop Server')
            elif porttype == TCP:
                self.pbStart_tcp.setText('Stop Server')
            else:
                self.pbStart_udp.setText('Stop Server')
            self.ServerActive = 1
            
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

    def ckTimeEnabled_stateChanged(self):
        if self.ckTimeEnabled.isChecked():
            self.ed_timeName.setEnabled(True)
        else:
            self.ed_timeName.setEnabled(False)
           
    def closeEvent(self,event):          
        try:
            self.port.shutdown()
        except:
            pass
            
        try:
            self.port.close()
        except:
            print('Port close failed')
       
        event.accept()
                    
app = QApplication(sys.argv)
frame = MainWindow()
frame.show()
sys.exit(app.exec())


