import sys
import os
if sys.version_info>(3,0):
    import sip
    sip.setapi('QString', 1)
    
#from pyqt45 import QApplication

from PyQt4.QtGui import QApplication
from PyQt4 import QtCore

import threading

from supsisim.pyEdit import SupsiSimMainWindow
from supsisim.library import Library

class supsisimul(threading.Thread):
    def __init__(self, filename = 'untitled', runflag = False):
        threading.Thread.__init__(self)
        if filename!='untitled':
            self.fname = QtCore.QFileInfo(filename)
            self.mypath = str(self.fname.absolutePath())
            self.fname = str(self.fname.baseName())
        else:
            self.fname = 'untitled'
            self.mypath =  os.getcwd()
        self.runflag = runflag

    def run(self):
        app = QApplication(sys.argv)
        library = Library()
        library.setGeometry(20, 20, 400, 980)
        library.show()
        main = SupsiSimMainWindow(library, self.fname, self.mypath, self.runflag)
        main.setGeometry(500,100,1024,768)

        main.show()
        ret = app.exec_()
        app.deleteLater()

def supsim(fn = 'untitled'):
    th = supsisimul(fn)
    th.start()
