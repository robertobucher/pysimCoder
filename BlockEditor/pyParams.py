#!/usr/bin/python

from PyQt5 import QtGui, QtCore, uic
from PyQt5.QtGui import *
from PyQt5.QtCore import *
from PyQt5.QtWidgets import QMainWindow, QApplication, QAction, QFileDialog, QMessageBox, \
QLabel, QLineEdit, QPushButton, QTableWidget, QTableWidgetItem

import supsictrl.unixsocket as sk
import os
import sys
import json

import socket
import struct
import time

path = os.environ.get('PYSUPSICTRL') + '/'
form_class = uic.loadUiType(path+'BlockEditor/pyParams.ui')[0]    
mypath = path + 'resources/icons/'

class MainWindow(QMainWindow, form_class):
    def __init__(self):
        QMainWindow.__init__(self)
        self.setupUi(self)
        self.devCbBox.clear()
        self.devCbBox.insertItem(0, '/tmp/ssock')
        self.addactions()
        self.addMenubar()
        self.addToolbars()
        self.filename = ''
        self.connectWidgets()
        self.N = 1;
        self.valuesTab.setRowCount(1)
        el1 = QTableWidgetItem('Var 0')
        el2 = QTableWidgetItem('0.00')
        self.valuesTab.setItem(0, 0, el1)
        self.valuesTab.setItem(0, 1, el2)
        
        portN =  self.devCbBox.currentIndex()
        self.portName = self.devCbBox.itemText(portN)
        self.sock = sk.open_client(self.portName)

    def addactions(self):
        self.newFileAction = QAction(QIcon(mypath+'filenew.png'),
                                         '&New', self,
                                         shortcut = 'Ctrl+N',
                                         statusTip = 'New File',
                                         triggered = self.newFile)

        self.openFileAction = QAction(QIcon(mypath+'fileopen.png'),
                                          '&Open', self,
                                          shortcut = 'Ctrl+O',
                                          statusTip = 'Open File',
                                          triggered = self.openFile)

        self.saveFileAction = QAction(QIcon(mypath+'filesave.png'),
                                          '&Save', self,
                                          shortcut = 'Ctrl+S',
                                          statusTip = 'Save File',
                                          triggered = self.saveFile)
                              
        self.saveAsFileAction = QAction(QIcon(mypath+'filesaveas.png'),
                                          'Save As...', self,
                                          shortcut = 'Ctrl+A',
                                          statusTip = 'Save File As...',
                                          triggered = self.saveAsFile)
                              
    def addToolbars(self):
        toolbarF = self.addToolBar('File')
        toolbarF.addAction(self.newFileAction)
        toolbarF.addAction(self.openFileAction)
        toolbarF.addAction(self.saveFileAction)
        toolbarF.addAction(self.saveAsFileAction)
         
    def addMenubar(self):
        menubar = self.menuBar()
        fileMenu = menubar.addMenu('&File')
        fileMenu.addAction(self.newFileAction)
        fileMenu.addAction(self.openFileAction)
        fileMenu.addAction(self.saveFileAction)
        fileMenu.addAction(self.saveAsFileAction)
        
    def connectWidgets(self):
        self.sbNsig.valueChanged.connect(self.modSignalsN)
        self.pbSendData.clicked.connect(self.sendData)

    def newFile(self):
        self. newFields()
        self.filename=''
            
    def openFile(self):
        filename, _ = QFileDialog.getOpenFileName(self, 'Open','.' , filter='*.pars')        
        if filename != '':
            self.filename = filename
            f = open(self.filename,'r')
            d = f.read()
            d = json.loads(d)
            self.setFields(d)
            f.close()
 
    def saveFile(self):
        if self.filename == '':
            filename, _ = QFileDialog.getSaveFileName(self,'Save', self.filename,  filter='*.pars')
            if filename.find('.pars') == -1:
                 self.filename =  filename + '.pars'
            else:
                self.filename = filename
                
        if self.filename != '':
            f = open(self.filename,'w')
            d = self.getFields()
            js = json.dumps(d)
            f.write(js)
            f.close()

    def saveAsFile(self):
        filename, _ = QFileDialog.getSaveFileName(self,'Save As ...', self.filename,  filter='*.pars')
        if filename.find('.pars') == -1:
            self.filename =  filename + '.pars'
        else:
            self.filename = filename
                
        if self.filename != '':
            f = open(self.filename,'w')
            d = self.getFields()
            js = json.dumps(d)
            f.write(js)
            f.close()

    def newFields(self):
        self.cleanFields()
             
        self.devCbBox.insertItem(0, '/tmp/ssock')
        self.devCbBox.setCurrentIndex(0)
            
        self.valuesTab.insertRow(0)
        el1 = QTableWidgetItem('Var 0')
        el2 = QTableWidgetItem('0.00')
        self.valuesTab.setItem(0, 0, el1)
        self.valuesTab.setItem(0, 1, el2)        
        
    def cleanFields(self):
        self.devCbBox.clear()
        N = self.sbNsig.value()
        for n in range(0,N):
             self.valuesTab.removeRow(n)
        self.valuesTab.setRowCount(0)
        self.N = 1
        self.sbNsig.setValue(1)
        
    def getFields(self):
        d = {}
        d['devices'] = []
        for n in range(0, self.devCbBox.count()):
            d['devices'].append(self.devCbBox.itemText(n))
        d['currentIndex'] = self.devCbBox.currentIndex()

        d['values'] = []
        for n in range(0, self.sbNsig.value()):
            el1 = self.valuesTab.item(n, 0)
            el2 = self.valuesTab.item(n, 1)
            d['values'].append([el1.text(), el2.text()])
        return(d)

    def setFields(self, d):
        self.cleanFields()

        self.devCbBox.insertItems(0, d['devices'])
        self.devCbBox.setCurrentIndex(int(d['currentIndex']))
        
        self.N = len(d['values'])
        self.valuesTab.setRowCount(self.N)
        self.valuesTab.setColumnCount(2)
        for n in range(0, self.N):
            el1 = QTableWidgetItem(d['values'][n][0])
            el2 = QTableWidgetItem(d['values'][n][1])
            self.valuesTab.setItem(n, 0, el1)
            self.valuesTab.setItem(n, 1, el2)

        self.sbNsig.setValue(self.N)
            
    def modSignalsN(self, val):
        if val>self.N:
            self.valuesTab.insertRow(self.N)
            el1  = QTableWidgetItem('Var ' + self.N.__str__())
            el2  = QTableWidgetItem('0.00')
            self.valuesTab.setItem(self.N, 0, el1)
            self.valuesTab.setItem(self.N, 1, el2)
            self.N += 1
        elif val<self.N:
             self.valuesTab.removeRow(self.N-1)
             self.N -= 1
    
    def sendData(self):
        val = []
        strdata = 'i' + str(self.N)+'d'
        for n in range(0, self.N):
            el = self.valuesTab.item(n, 1)
            val.append(float(el.text()))
            
        msg = struct.pack(strdata, 1, *val)
        self.sock.sendall(msg)

    def closeEvent(self, event):
        msg = struct.pack('i d', 0, 0.0)
        self.sock.sendall(msg)
        time.sleep(1)
        sk.close_client(self.sock, self.portName)

app = QApplication(sys.argv)
frame = MainWindow()
frame.show()
sys.exit(app.exec_())
