#!/usr/bin/python

from PyQt5.QtGui import *
from PyQt5.QtCore import *
from PyQt5.QtWidgets import QMainWindow, QApplication, QGridLayout, QWidget, QPushButton, QSpinBox, QCheckBox, QTextEdit, QAction, QMenuBar, QLabel, QFileDialog, QLineEdit, QMessageBox, QComboBox
from PyQt5 import QtCore, uic

import os
import sys
import json

path = os.environ.get('PYSUPSICTRL') + '/'
mypath = path + 'resources/icons/'

form_class = uic.loadUiType(path+'BlockEditor/xblk.ui')[0]    

class myCombo(QComboBox):
    def __init__(self):
        super(myCombo,self).__init__()
        self.addItem('Real')
        self.addItem('Integer')
        self.addItem('String')

    def getType(self):
        return self.currentIndex()
        
class MainWindow(QMainWindow, form_class):
    def __init__(self):
        QMainWindow.__init__(self)
        self.setupUi(self)
        self.setWindowTitle("xblk2blk")
        self.resize(800,700)
        self.N = 1
        self.addActions()
        self.connectWidgets()

        self.gridLayout.addWidget(QLabel('Label'),0,0)
        self.gridLayout.addWidget(QLabel('Value'),0,1)
        self.gridLayout.addWidget(QLabel('Var Name'),0,2)
        self.gridLayout.addWidget(QLabel('Type'),0,3)
       
    def addActions(self):
        openFileAction = QAction(QIcon(mypath+'fileopen.png'),
                                     '&Open', self,
                                     shortcut = 'Ctrl+O',
                                     statusTip = 'Open File',
                                     triggered = self.openFile)
       
        menubar = QMenuBar(self)
        self.setMenuBar(menubar)
        fileMenu = menubar.addMenu('&File')      
        fileMenu.addAction(openFileAction)
        toolbarF = self.addToolBar('File')
        toolbarF.addAction(openFileAction)

    def connectWidgets(self):
        self.btnGenText.clicked.connect(self.genFunction)
        self.btnWriteText.clicked.connect(self.writeFunction)
        self.btnGenSkel.clicked.connect(self.genSkeleton)
       
    def params2grid(self,pars):
        ln = pars.split('|')
        self.N = len(ln)
        self.edFun = ln[0]
 
        for n in range(1,self.N):
            ll = ln[n].split(':')
            self.gridLayout.addWidget(QLabel(ll[0]),n,0)
            self.gridLayout.addWidget(QLabel(ll[1]),n,1)
            self.gridLayout.addWidget(QLineEdit(''),n,2)
            self.gridLayout.addWidget(myCombo(),n,3)

    def openFile(self):
        self.text.setText('')
        
        for i in range(1,self.N):
            for j in range(0,4):
                item = self.gridLayout.itemAtPosition(i,j).widget()
                self.gridLayout.removeWidget(item)
                item.deleteLater()
                del(item)
        filename, _ = QFileDialog.getOpenFileName(self, 'Open','.' , filter='*.xblk')

        if filename != '':
            self.filename = filename
            f = open(self.filename,'r')
            d = f.read()
            d = json.loads(d)
            f.close()
            self.inputs = int(d['ip'])
            self.outputs = int(d['op'])
            self.params2grid(d['params'])
        
    def genFunction(self):
        self.text.setText('')
        callingFun = self.edFun + '('
        if self.inputs != 0:
            callingFun += 'pin, '
            pin = 'pin'
        else:
            pin = '[]'
            
        if self.outputs != 0:
            callingFun += 'pout, '
            pout = 'pout'
        else:
            pout = '[]'
            
        for n in range(1, self.N):
            callingFun += self.gridLayout.itemAtPosition(n,2).widget().text() + ', '
        callingFun = callingFun.rstrip(', ') + ')'
        fun   = 'from supsisim.RCPblk import RCPblk\n'
        fun += 'from scipy import size\n\n'
        
        fun += 'def ' + callingFun + ':\n'
            
        # write comment lines
        fun += '    """\n'
        fun += '\n    Call:   ' + callingFun + '\n\n'
        fun += '    Parameters\n'
        fun +='    ----------\n'
        if pin != '[]':
            fun += '       pin: connected input port(s)\n'
        if pout != '[]':
            fun += '       pout: connected output port(s)\n'
        
        for n in range(1, self.N):
            fun += '       ' + self.gridLayout.itemAtPosition(n,2).widget().text()
            fun += ' : ' + self.gridLayout.itemAtPosition(n,0).widget().text() + '\n'
        fun += '\n'
        fun += '    Returns\n'
        fun += '    -------\n'
        fun += '       blk: RCPblk\n\n    """\n\n    '    
        
        name = self.edFun.rstrip('Blk')

        states = '[' + str(self.spBoxCS.value()) + ',' + str(self.spBoxDS.value()) + ']'
        if self.ckbRelation.checkState() == Qt.Checked:
            feedfw = '1'
        else:
            feedfw = '0'  

        rpar = '['
        ipar = '['
        strpar = ''

        for n in range(1, self.N):
            index = self.gridLayout.itemAtPosition(n,3).widget().getType()
            if index == 0:
                rpar += self.gridLayout.itemAtPosition(n,2).widget().text() + ', '
            elif index == 1:
                ipar += self.gridLayout.itemAtPosition(n,2).widget().text() + ', '
            else:
                if strpar == '':
                    strpar = self.gridLayout.itemAtPosition(n,2).widget().text()
                else:
                    msg = QMessageBox()
                    msg.setIcon(QMessageBox.Warning)
                    msg.setText('Maximum 1 string variable!')
                    msg.exec_()
                    return
        rpar = rpar.rstrip(', ') + ']'
        ipar = ipar.rstrip(', ') + ']'

        fun += 'blk = RCPblk(' + "'" + name + "'" + ', ' + pin + ', ' + pout + ', ' + states + ', ' + feedfw + ', '
        fun += rpar + ', ' + ipar
        if strpar == '':
            fun += ')'
        else:
            fun += ', ' + strpar +')'

        fun += '\n    return blk\n'
        
        self.text.setText(fun)

    def writeFunction(self):
        if self.text.toPlainText() != '':
            fun = '\n' + self.text.toPlainText()
            f = open(self.edFun + '.py', 'w')
            f.write(fun)
            f.close()
            
    def genSkeleton(self):
        if self.text.toPlainText() != '':
            name = self.edFun.rstrip('Blk')
            cmd = 'gen_pydev ' + name
            os.system(cmd)
        
app = QApplication(sys.argv)
frame = MainWindow()
frame.show()
sys.exit(app.exec_())
