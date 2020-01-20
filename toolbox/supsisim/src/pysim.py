import sys
import os
if sys.version_info>(3,0):
    import sip
    sip.setapi('QString', 1)
    
from PyQt5.QtWidgets import QApplication
from PyQt5.QtCore import QFileInfo



from supsisim.pyEdit import NewEditorMainWindow
from supsisim.library import Library

class supsisimul():
    def __init__(self, filename = 'untitled', runflag = False):
        if filename!='untitled':
            self.fname = QFileInfo(filename)
            self.mypath = str(self.fname.absolutePath())
            self.fname = str(self.fname.baseName())
        else:
            self.fname = 'untitled'
            self.mypath =  os.getcwd()
        self.runflag = runflag

        app = QApplication(sys.argv)
        V = app.desktop().screenGeometry()
        h = V.height()
        w = V.width()
        
        library = Library()
        library.setGeometry(0, 0, 400, h)
        library.show()
        main = NewEditorMainWindow(library, self.fname, self.mypath, self.runflag)
        main.setGeometry(420, 0, int(w-420), int(h*4/5))

        main.show()
        ret = app.exec_()
        app.deleteLater()

def supsim(fn = 'untitled'):
    th = supsisimul(fn)
