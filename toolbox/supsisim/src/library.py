#!/usr/bin/python

import os

from PyQt5.QtWidgets import QGraphicsScene, QMainWindow, QWidget, QVBoxLayout, \
    QHBoxLayout, QGraphicsView,QTabWidget, QApplication, QFileDialog, \
    QAction,  QMenu, QMessageBox, QComboBox

from PyQt5.QtGui import QTransform, QDrag, QIcon
from PyQt5.QtCore import Qt, QMimeData, QFileInfo

from supsisim.const import respath
from supsisim.pyEdit import NewEditorMainWindow

import os
import json
from lxml import etree

from supsisim.block import Block
from supsisim.const import respath, BWmin

class CompViewer(QGraphicsScene):
    def __init__(self, parent=None):
        super(CompViewer, self).__init__()
        self.parent = parent

        self.componentList = []	 
        self.activeComponent = None 

    def dropEvent(self, event):
        event.accept()

    def dragEnterEvent(self, event):
        event.accept()

    def dragMoveEvent(self, event):
        event.accept()

    def mousePressEvent(self, event):
        x = event.scenePos().x()
        y = event.scenePos().y()

        t = QTransform()
        self.actComp = self.itemAt(x, y, t)

    def mouseMoveEvent(self, event):
        if event.buttons() == Qt.LeftButton and isinstance(self.actComp, Block):
            root = etree.Element('root')
            self.actComp.save(root)
            msg = etree.tostring(root, pretty_print=True)           
            mimeData = QMimeData()
            mimeData.setText(msg.decode())
            
            drag = QDrag(self.parent)
            drag.setMimeData(mimeData)
            drag.exec_(Qt.CopyAction)

    def mouseReleaseEvent(self, event):
        pass
        
class Library(QMainWindow):
    '''
    '''

    def __init__(self, parent=None):
        QMainWindow.__init__(self, parent)

        self.centralWidget = QWidget()
        self.setWindowTitle('Library')

        self.addActions()
        self.addMenubar()
        self.addToolbars()
       
        self.path =  os.getcwd()
        
        self.libConfig = []
        self.readLib()
        self.closeFlag = False
 
        self.mainWins = []
        
        self.tabs = QTabWidget()
        tabS = ''

        index = 0
        for el in self.libConfig:
            if el['lib'] != tabS:
                if tabS != '':
                    tab = QWidget()
                    layout = QVBoxLayout()
                    layout.addWidget(view)
                    tab.setLayout(layout)
                    self.tabs.addTab(tab, tabS)
                    if tabS=='common':
                        index = self.tabs.indexOf(tab)
                        
                tabS = el['lib']
                diagram = CompViewer(self)
                view = QGraphicsView(diagram)
                diagram.compLock = True
                i = 1

            try:
                w = el['width']
            except:
                w = BWmin
                
            stbin = (el['stin'] == 1)
            stbout = (el['stout'] == 1)
            b = Block(None, diagram, el['name'], el['ip'], el['op'], stbin, stbout, el['icon'], el['params'], w, False)
            px = (i-1) % 2
            py = (i-1)/2
            b.setPos(px*150,py*200)
            i += 1
            
        tab = QWidget()
        layout = QVBoxLayout()
        layout.addWidget(view)
        tab.setLayout(layout)
        self.tabs.addTab(tab, tabS)
            
        layout = QHBoxLayout()
        layout.addWidget(self.tabs)
        self.widget = QWidget()
        self.widget.setLayout(layout)
        self.setCentralWidget(self.widget)
        self.tabs.setTabPosition(QTabWidget.West)
        self.tabs.setCurrentIndex(index)

    def addActions(self):
        mypath = respath + '/icons/'

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

        self.exitAction = QAction(QIcon(mypath+'exit.png'),
                                '&Exit',self,
                                  shortcut = 'Ctrl+X',
                                  statusTip = 'Exit Application',
                                  triggered = self.close)
        
    def newFile(self):
        main = NewEditorMainWindow('untitled', self.path, self)
        self.mainWins.append(main)
        main.show()

    def openFile(self):
        filename = QFileDialog.getOpenFileName(self, 'Open', '.', filter='*.dgm')
        filename = filename[0]
        if filename != '':
            self.fopen(filename)

    def fopen(self, filename):
        fname = QFileInfo(filename)
        self.path = str(fname.absolutePath())
        fn = str(fname.baseName())
        main = NewEditorMainWindow(fn, self.path, self)
        self.mainWins.append(main)
        main.show()

    def addToolbars(self):
        toolbarF = self.addToolBar('File')
        toolbarF.addAction(self.newFileAction)
        toolbarF.addAction(self.openFileAction)
        toolbarF.addAction(self.exitAction)
        
    def addMenubar(self):
        menubar = self.menuBar()
        fileMenu = menubar.addMenu('&File')
        fileMenu.addAction(self.newFileAction)
        fileMenu.addAction(self.openFileAction)
        fileMenu.addAction(self.exitAction)
       
    def getBlock(self,fn):
        f = open(fn,'r')
        d = f.read()
        f.close()
        d = json.loads(d)
        return d
        
    def readLib(self):
        commonDir = respath+'blocks/blocks'
        
        blkList = []
        try:
            fn = open(commonDir + '/common.blks')
            for f in fn:
                f = f.rstrip()
                try:
                    d = self.getBlock(respath +'blocks/blocks/' + f)
                    d['lib'] = 'common'
                    blkList.append(d)
                except:
                    try:
                        d = self.getBlock(myDir + '/' + f)
                        d['lib'] = 'common'
                        blkList.append(d)
                    except:
                        pass                
            fn.close()
        except:
            pass
        
        files = os.listdir(commonDir)
        for f in sorted(files):
            if f.endswith('.xblk'):
                d = self.getBlock(respath +'blocks/blocks/' + f)
                blkList.append(d)
        
        self.libConfig = sorted(blkList, key=lambda k: (k['lib'].lower()))

    def closeWindow(self, mainW):
        self.mainWins.remove(mainW)
   
    def closeEvent(self,event):
        for el in self.mainWins:
            el.close()
        event.accept()

if __name__ == '__main__':
    import sys
    import logging
    logging.basicConfig()
    app = QApplication(sys.argv)

    library = Library()
    library.setGeometry(20, 20, 400, 500)
    library.show()

    app.exec_()
    sys.exit()
