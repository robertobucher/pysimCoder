#!/usr/bin/python3

from PyQt5 import QtGui, QtCore

from PyQt5.QtWidgets import QMainWindow, QApplication, QWidget, \
    QLabel, QCheckBox, QGridLayout, QPushButton
from PyQt5.QtCore import Qt

import os
import sys

path = os.environ.get('PYSUPSICTRL') + '/'

class MainWindow(QMainWindow):
    def __init__(self):
        QMainWindow.__init__(self)

        self.commonDir = path+'resources/blocks/blocks'
        f = open(self.commonDir + '/folders','r')
        self.dirs = []
        self.folders = []
        for el in f:
            el = el.rstrip('\n')
            self.dirs.append(el)
        f.close()

        for el in os.listdir(self.commonDir):
            if os.path.isdir(self.commonDir + '/' + el):
                self.folders.append(el)

        self.setGeometry(0,0,500,500)
        self.setWindowTitle('Choose libraries')
        self.grid = QGridLayout()
        widget = QWidget()
        widget.setLayout(self.grid)
        self.setCentralWidget(widget)
        self.setWindow()

        self.btnSAVE.clicked.connect(self.saveCh)

        self.show()

    def setWindow(self):
        n = 0
        for el in self.folders:
            qc = QCheckBox(el)
            self.grid.addWidget(qc, n, 0)
            if qc.text() in self.dirs:
                qc.setCheckState(Qt.Checked)
            n += 1
        self.btnSAVE = QPushButton('SAVE')
        self.grid.addWidget(self.btnSAVE, n, 0)

    def saveCh(self):
        N = self.grid.rowCount()
        f = open(self.commonDir + '/folders','w')
        for n in range(0, N-1):
            qc = self.grid.itemAt(n).widget()
            if qc.checkState() == Qt.Checked:
                f.write(qc.text() + '\n')
        f.close()

app = QApplication(sys.argv)
frame = MainWindow()
frame.show()
sys.exit(app.exec_())

