#!/usr/bin/python

import sys
import os

from PyQt5.QtWidgets import QApplication
from PyQt5.QtCore import QFileInfo
from supsisim.library import Library

if __name__ == "__main__":
    if len(sys.argv) > 1:
        filename = sys.argv[1]
        if filename[-4:] != '.dgm':
            filename = filename + '.dgm'
        if not os.path.isfile(filename):
            filename = 'untitled'
    else:
        filename = 'untitled'

    if filename!='untitled':
        fname = QFileInfo(filename)
        mypath = str(fname.absolutePath())
        fname = str(fname.baseName())
    else:
        fname = 'untitled'
        mypath =  os.getcwd()

    app = QApplication(sys.argv)
    V = app.desktop().screenGeometry()
    h = V.height()
    w = V.width()
        
    library = Library()
    library.setGeometry(0, 0, 400, h)
    library.show()
    if fname == 'untitled':
        library.newFile()
    else:
        library.fopen(fname)
    ret = app.exec_()
    app.deleteLater()

