#import sys

from PyQt5.QtWidgets import QDialog, QGridLayout, QLabel, QLineEdit, QPushButton, \
    QListWidget, QMessageBox
from PyQt5.QtCore import Qt

from supsisim.const import pyrun, respath

class BlkDlg(QDialog):
    def __init__(self, line):
        super(BlkDlg, self).__init__(None)
        grid = QGridLayout()
        self.line = line
        self.blkID = ''
        self.labels, self.params = self.parseParams(line)
        self.setWindowTitle(self.blkID)
        N = len(self.labels)
        self.Values = []
        for n in range(0,N):
            Lab = QLabel(self.labels[n])
            Val = QLineEdit(self.params[n].__str__())
            self.Values.append(Val)
            grid.addWidget(Lab,n,0)
            grid.addWidget(Val,n,1)
        
        self.pbOK = QPushButton('OK')
        self.pbCANCEL = QPushButton('CANCEL')
        self.pbHELP = QPushButton('HELP')
        grid.addWidget(self.pbHELP,N,0)
        grid.addWidget(self.pbOK,N+1,0)
        grid.addWidget(self.pbCANCEL,N+1,1)
        self.pbHELP.clicked.connect(self.blkHelp)
        self.pbOK.clicked.connect(self.accept)
        self.pbCANCEL.clicked.connect(self.reject)
        self.setLayout(grid)

    def blkHelp(self):
        fn = respath + 'blocks/rcpBlk/' + self.fname + '.hlp'
        try:
            f = open(fn,'r')
        except:
            f = open(respath + 'blocks/rcpBlk/noHelp.hlp', 'r')
        text = f.read()
        msg = QMessageBox(self)
        msg.setIcon(QMessageBox.Information)
        msg.setWindowTitle('Help for ' + self.fname)
        msg.setText(text)
        msg.exec_()
    
    def parseParams(self, line):
        ln = line.split('|')
        N = len(ln)
        lab = []
        val = []
        self.blkID = ln[0]
        self.fname = ln[0]
        for n in range(1,N):
            ll = ln[n].split(':')
            lab.append(ll[0].__str__())
            par = ll[1].__str__()
            par = par.lstrip(' ')
            val.append(par)
        return lab,val

    def accept(self):
        N = len(self.labels)
        self.line = self.blkID
        for n in range(0,N):
            self.line += '|' + self.labels[n] +': ' + str(self.Values[n].text())
        super(BlkDlg, self).accept()

def parsDialog(pars):
    #app = app = QApplication(sys.argv)
    dialog = BlkDlg(pars)
    res = dialog.exec_()
    return dialog.line

