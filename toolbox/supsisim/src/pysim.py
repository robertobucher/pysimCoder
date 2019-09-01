import sys
import os
if sys.version_info>(3,0):
    import sip
    sip.setapi('QString', 1)
    
from pyqt5 import QApplication, QtCore

from supsisim.pyEdit import SupsiSimMainWindow
from supsisim.library import Library

class supsisimul():
    def __init__(self, filename = 'untitled', runflag = False):
        if filename!='untitled':
            self.fname = QtCore.QFileInfo(filename)
            self.mypath = str(self.fname.absolutePath())
            self.fname = str(self.fname.baseName())
        else:
            self.fname = 'untitled'
            self.mypath =  os.getcwd()
        self.runflag = runflag

        app = QApplication(sys.argv)
        library = Library()
        library.setGeometry(20, 20, 400, 980)
        library.show()
        main = SupsiSimMainWindow(library, self.fname, self.mypath, self.runflag)
        main.setGeometry(500,100,1024,800)

        main.show()
        ret = app.exec_()
        app.deleteLater()

def supsim(fn = 'untitled'):
    th = supsisimul(fn)
