#!/usr/bin/python

from pyqt45 import QGraphicsScene, QMainWindow, QWidget, QVBoxLayout, \
                   QHBoxLayout, QGraphicsView,QTabWidget, QApplication, \
                   QTransform, QDrag, QtCore, QMenu, QMessageBox, QComboBox, \
                   set_orient

#import dircache
import os
import json

from supsisim.block import Block
from supsisim.const import respath

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
        if event.buttons() == QtCore.Qt.LeftButton and isinstance(self.actComp, Block):
            mimeData = QtCore.QMimeData()
            if self.actComp.iosetble:
                io = '1'
            else:
                io = '0'
            data = self.actComp.name+'@'+self.actComp.inp.__str__()+'@'+self.actComp.outp.__str__() + '@' + io +'@' + self.actComp.icon + '@' + self.actComp.params
            mimeData.setText(data)
            drag = QDrag(self.parent)
            drag.setMimeData(mimeData)
            drag.exec_(QtCore.Qt.CopyAction)

    def mouseReleaseEvent(self, event):
        pass
        
class Library(QMainWindow):
    '''
    '''

    def __init__(self, parent=None):
        QMainWindow.__init__(self, parent)

        self.centralWidget = QWidget()
        self.resize(800, 500)
        self.setWindowTitle('Library')
        self.libConfig = []
        self.readLib()
        self.closeFlag = False
 
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

            io = (el['st'] == 1)
            b = Block(None, diagram, el['name'], el['ip'], el['op'], io, el['icon'], el['params'], False)
            px = (i-1) % 2
            py = (i-1)/2
            b.setPos(px*150,py*150)
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

    def getBlock(self,fn):
        f = open(fn,'r')
        d = f.read()
        f.close()
        d = json.loads(d)
        return d
        
    def readLib(self):
        myDir = os.environ.get('PYTHONPATH')+'/blocks'
        
        blkList = []
        try:
            fn = open(myDir + '/common.blks')
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
        
        files = os.listdir(respath+'blocks/blocks')
        for f in sorted(files):
            if f.endswith('.xblk'):
                d = self.getBlock(respath +'blocks/blocks/' + f)
                blkList.append(d)

        try:
            files = os.listdir(myDir)
            for f in sorted(files):
                if f.endswith('.xblk'):
                    d = self.getBlock(myDir + '/'  + f)
                    blkList.append(d)
        except:
            pass
        
        self.libConfig = sorted(blkList, key=lambda k: (k['lib'].lower()))
            
    def closeEvent(self,event):
        if self.closeFlag:
            event.accept()
        else:
            event.ignore()

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
