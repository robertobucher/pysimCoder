# -*- coding: utf-8 -*-
"""
Created on Thu Jun  8 17:35:41 2017

@author: paul
"""

import sys
use_pyqt = None

#==============================================================================
# try if PyQt4 is already loaded
#==============================================================================
if use_pyqt == None:
    if 'PyQt4' in sys.modules:
        use_pyqt = 4

    else:
        try:
            import PyQt5
            use_pyqt = 5
    #        print('using Pyqt5')
        except ImportError:
            import PyQt4
            use_pyqt = 4

#==============================================================================
# PyQt4       
#==============================================================================
if use_pyqt == 4:
    from PyQt4.QtGui import QAction, QApplication, QDialog, QFileDialog, \
        QGraphicsItem, QGraphicsPathItem, QGraphicsScene, QGraphicsTextItem, \
        QGraphicsView, QGridLayout, QHBoxLayout, QLabel, QLineEdit, \
        QListWidget, QMainWindow, QMenu, QMessageBox, QPushButton, \
        QSpinBox, QTabWidget, QVBoxLayout, QWidget, \
        QTableWidget, QTableWidgetItem, QLayout, QTextEdit, QFont, QComboBox, \
        QDialogButtonBox

    from PyQt4.QtGui import QPrinter, QPrintDialog

    from PyQt4.QtGui import QDrag, QIcon, QImage, QPainter, QPainterPath, \
        QPen, QBrush, QTransform, QPolygonF
        
    from PyQt4 import QtCore, uic
#    print('using Pyqt4')
        

#==============================================================================
# PyQt5       
#==============================================================================
elif use_pyqt == 5:
    from PyQt5.QtWidgets import QAction, QApplication, QDialog, QFileDialog, \
        QGraphicsItem, QGraphicsPathItem, QGraphicsScene, QGraphicsTextItem, \
        QGraphicsView, QGridLayout, QHBoxLayout, QLabel, QLineEdit, \
        QListWidget, QMainWindow, QMenu, QMessageBox, QPushButton, \
        QSpinBox, QTabWidget, QVBoxLayout, QWidget, \
        QTableWidget, QTableWidgetItem, QLayout, QTextEdit, QComboBox, \
        QDialogButtonBox


    from PyQt5.QtPrintSupport import QPrinter, QPrintDialog

    from PyQt5.QtGui import QDrag, QIcon, QImage, QPainter, QPainterPath, \
        QPen, QBrush, QTransform, QFont, QPolygonF

    from PyQt5 import QtCore, uic
#    print('using Pyqt5')
        
#==============================================================================
# no PyQt found
#==============================================================================
else:
    raise Exception('No PyQt4 or PyQt5 found')





class textItem(QGraphicsTextItem):
    '''convenience class, extension of QGraphicsTextItem, that realises aligned text
    textItem.setFlipped() will mirror the text  (in place)
    textItem.setNormal() will put txt in normal (non-mirrored) state
    
    anchor is (look at numpad):
    1: bottom-left
    2: bottom-center
    3: bottom-right
    4: center-left
    5: center-center
    6: center-right
    7: top-left
    8: top-center
    9: top-right'''
    def __init__(self, text, anchor=1, parent=None):
        super(textItem, self).__init__(text, parent)
        self.anchor = anchor
        
        # compute dx, dy absed on anchor
        self.dx, self.dy = 0, 0
        if anchor in (4,5,6):
            self.dy = -0.5*self.boundingRect().height()
        elif anchor in (1,2,3):
            self.dy = -self.boundingRect().height()
        
        if anchor in (2,5,8):
            self.dx = -0.5*self.boundingRect().width()
        if anchor in (3,6,9):
            self.dx = -self.boundingRect().width()
            
        self.setNormal()
        
    def setFlipped(self):
        '''mirror in place (use when parent is flipped'''
        self.setTransform(QTransform().translate(self.dx, self.dy).scale(-1,1).translate(-self.boundingRect().width(),0))

    def setNormal(self):
        '''normal orientation'''
        self.setTransform(QTransform.fromScale(1,1).translate(self.dx, self.dy))





    
def set_orient(item, flip=False, scale=0, rotate=0, combine=False):
    '''returns QTransform, operation order: flip (mirror in Y axis), scale, rorate (in degrees)'''
    if flip:
        item.setTransform(QTransform.fromScale(-scale, scale).rotate(rotate), combine)
    else:
         item.setTransform(QTransform.fromScale(scale, scale).rotate(rotate), combine)
       
