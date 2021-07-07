#!/usr/bin/python

from PyQt5 import QtGui, QtCore, uic
from PyQt5.QtGui import *
from PyQt5.QtCore import *
from PyQt5.QtWidgets import QMainWindow, QApplication, QAction, QFileDialog, QMessageBox, QLabel, QLineEdit, QPushButton, QTableWidget, QTableWidgetItem

import os
import sys
import json

path = os.environ.get('PYSUPSICTRL') + '/'
form_class = uic.loadUiType(path+'BlockEditor/BlockParams.ui')[0]    

mypath = path + 'resources/icons/'
blkpath =  path + 'resources/blocks/blocks/'

class MainWindow(QMainWindow, form_class):
    def __init__(self):
        QMainWindow.__init__(self)
        self.setupUi(self)
        self.addactions()
        self.addMenubar()
        self.addToolbars()
        self.filename = ''
        self.connectWidgets()
        self.parRows = 0

    def connectWidgets(self):
        self.pbAddParamUp.clicked.connect(self.addParamUp)        
        self.pbAddParamDw.clicked.connect(self.addParamDw)        
        self.pbRemParam.clicked.connect(self.remParam)        

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

    def params2grid(self,pars):
        ln = pars.split('|')
        N = len(ln)
        self.edFun.setText(ln[0])
        self.tableWidget.setRowCount(N-1)
        self.tableWidget.setColumnCount(2)
 
        for n in range(1,N):
            ll = ln[n].split(':')
            el = QTableWidgetItem(ll[0])
            self.tableWidget.setItem(n-1,0,el)
            el = QTableWidgetItem(ll[1])
            self.tableWidget.setItem(n-1,1,el)
            self.tableWidget.show()
        self.parRows = N-1
       
    def grid2params(self):
        pars = self.edFun.text()
        for n in range(0,self.parRows):
            el1 = self.tableWidget.item(n,0)
            el2 =  self.tableWidget.item(n,1)
            pars = pars + '|' + el1.text().rstrip(':') + ':' + el2.text()
        return pars

    def cleanGrid(self):
        self.tableWidget.clearContents()
        self.parRows = 0;
        self.tableWidget.setRowCount(0)
           
    def askSaving(self):
        msg = QMessageBox()
        msg.setIcon(QMessageBox.Question)
        msg.setWindowTitle('The document has been modified')
        msg.setText('Do you want to save your changes?')
        msg.setStandardButtons(QMessageBox.Save | QMessageBox.Discard| QMessageBox.Cancel)
        ret = msg.exec_()
        return ret
    
    def newFile(self):
        ret = self.askSaving()
        if ret == QMessageBox.Save:
            self.saveFile()
        elif ret != QMessageBox.Cancel:
            self.edLibrary.setText('')
            self.edName.setText('')
            self.spInputs.setValue(0)
            self.spOutputs.setValue(0)
            self.spSettable.setCheckState(Qt.Unchecked)
            self.edIcon.setText('')
            self.edFun.setText('')
            self.helpText = ''
            self.filename = ''
            self.cleanGrid()
            
    def openFile(self):
        filename, _ = QFileDialog.getOpenFileName(self, 'Open','.' , filter='*.xblk')

        if filename != '':
            self.filename = filename
            f = open(self.filename,'r')
            d = f.read()
            d = json.loads(d)
            f.close()
            self.edLibrary.setText(d['lib'])
            self.edName.setText(d['name'])
            self.spInputs.setValue(d['ip'])
            self.spOutputs.setValue(d['op'])
            if d['stin'] == 1:
                self.ckbinsettable.setCheckState(Qt.Checked)
            else:
                self.ckbinsettable.setCheckState(Qt.Unchecked)
            if d['stout'] == 1:
                self.ckboutsettable.setCheckState(Qt.Checked)
            else:
                self.ckboutsettable.setCheckState(Qt.Unchecked)
                    
            self.edIcon.setText(d['icon'])
            self.params2grid(d['params'])
            self.helpText.setText(d['help'])

    def saveFile(self):
        params = self.grid2params()

        if self.filename == '':
            filename, _ = QFileDialog.getSaveFileName(self,'Save', self.filename,  filter='*.xblk')
            if filename.find('.xblk') == -1:
                 self.filename =  filename + '.xblk'
            else:
                self.filename = filename
                
        if self.filename != '':
            d = self.getFields()
            d['params'] = params
            d['help'] = self.helpText.document(). toPlainText()
            js = json.dumps(d)
            f = open(self.filename,'w')
            f.write(js)
            f.close()

    def saveAsFile(self):
        params = self.grid2params()

        filename, _ = QFileDialog.getSaveFileName(self,'Save As ...', self.filename,  filter='*.xblk')
        if filename.find('.xblk') == -1:
            self.filename =  filename + '.xblk'
        else:
            self.filename = filename
                
        if self.filename != '':
            d = self.getFields()
            d['params'] = params
            d['help'] = self.helpText.document(). toPlainText()
            js = json.dumps(d)
            f = open(self.filename,'w')
            f.write(js)
            f.close()

    def addParamUp(self):
        row = self.tableWidget.currentRow()
        #self.tableWidget.setRowCount(self.parRows+1)
        self.tableWidget.insertRow(row)
        self.parRows += 1
    
    def addParamDw(self):
        row = self.tableWidget.currentRow()
        self.tableWidget.insertRow(row+1)
        self.parRows += 1

    def remParam(self):
        self.tableWidget.removeRow(self.tableWidget.currentRow())
        self.parRows -= 1

    def getFields(self):
        d = {}
        d['lib'] = self.edLibrary.text().__str__()
        d['name'] = self.edName.text().__str__()
        d['ip'] = self.spInputs.value()
        d['op'] = self.spOutputs.value()
        
        if self.ckbinsettable.checkState() == Qt.Checked:
            d['stin'] = 1
        else:
            d['stin'] = 0    
        if self.ckboutsettable.checkState() == Qt.Checked:
            d['stout'] = 1
        else:
            d['stout'] = 0
            
        d['icon'] = self.edIcon.text().__str__()
        return d
 
app = QApplication(sys.argv)
frame = MainWindow()
frame.show()
sys.exit(app.exec_())
