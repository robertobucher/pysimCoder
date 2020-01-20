#!/usr/bin/python

import os

from PyQt5.QtWidgets import QMainWindow, QWidget, QVBoxLayout,  QAction, \
    QMessageBox, QFileDialog, QDialog
from PyQt5.QtGui import QPainter, QIcon
from PyQt5.QtPrintSupport import QPrinter, QPrintDialog
from PyQt5.QtCore import QPointF, QFileInfo

from supsisim.block import Block
from supsisim.connection import Connection
from supsisim.editor import Editor
from supsisim.library import Library
from supsisim.scene import Scene, GraphicsView
from supsisim.dialg import IO_Dialog
from supsisim.const import respath, pycmd

DEBUG = False

class NewEditorMainWindow(QMainWindow):
    def __init__(self, library, fname, mypath, runflag, parent=None):
        super(NewEditorMainWindow, self).__init__(parent)
        self.centralWidget = QWidget(self)
        self.verticalLayout = QVBoxLayout(self.centralWidget)
        self.view = GraphicsView(self.centralWidget)
        self.view.setMouseTracking(True)
        self.scene = Scene(self)
        self.view.setScene(self.scene)
        self.view.setRenderHint(QPainter.Antialiasing)
        self.verticalLayout.addWidget(self.view)
        self.setCentralWidget(self.centralWidget)
        self.addactions()
        self.addMenubar()
        self.addToolbars()
        self.filename = fname
        self.path = mypath
        self.library = library
        self.runflag = runflag
        if fname != 'untitled':
            self.scene.loadDgm(self.getFullFileName())
            
        self.setWindowTitle(self.filename)
        self.status = self.statusBar()
        self.evpos = QPointF(0,0)
        self.editor = Editor(self)
        self.editor.install(self.scene)
        self.editor.redrawNodes()
        self.status.showMessage('Ready')

        self.modified = False

    def addactions(self):
        mypath = respath + '/icons/'

        self.newFileAction = QAction(QIcon(mypath+'filenew.png'),
                                                '&Open', self,
                                                shortcut = 'Ctrl+N',
                                                statusTip = 'New File',
                                                triggered = self.newFile)
        
        self.openFileAction = QAction(QIcon(mypath+'fileopen.png'),
                                                '&Open', self,
                                                shortcut = 'Ctrl+O',
                                                statusTip = 'Open File',
                                                triggered = self.openFile)

        self.saveFileAction = QAction(QIcon(mypath+'filesave.png'),
                                                '&Save', self,
                                                shortcut = 'Ctrl+S',
                                                statusTip = 'Save File',
                                                triggered = self.saveFile)
        
        self.saveFileAsAction = QAction(QIcon(mypath+'filesaveas.png'),
                                                '&Save as', self,
                                                shortcut = 'Ctrl+A',
                                                statusTip = 'Save File As...',
                                                triggered = self.saveFileAs)

        self.copyAction = QAction(QIcon(mypath+'copy.png'),
                                            '&Copy', self,
                                            shortcut = 'Ctrl+C',
                                            statusTip = 'Copy',
                                            triggered = self.copyAct)

        self.pasteAction = QAction(QIcon(mypath+'paste.png'),
                                             '&Paste', self,
                                             shortcut = 'Ctrl+V',
                                             statusTip = 'Paste',
                                             triggered = self.pasteAct)

        self.undoAction = QAction(QIcon(mypath+'undo.png'),
                                             '&Undo', self,
                                             shortcut = 'Ctrl+Z',
                                             statusTip = 'Undo',
                                             triggered = self.undoAct)

        self.updateAction = QAction(QIcon(mypath+'refresh.png'),
                                             '&Update Diagram', self,
                                             shortcut = 'Ctrl+U',
                                             statusTip = 'Update Diagram',
                                             triggered = self.updateAct)

        self.printAction = QAction(QIcon(mypath+'print.png'),
                                             '&Print', self,
                                             shortcut = 'Ctrl+P',
                                             statusTip = 'Print schematic',
                                             triggered = self.print_scheme)

        self.exitAction = QAction(QIcon(mypath+'exit.png'),
                                            '&Exit',self,
                                            shortcut = 'Ctrl+X',
                                            statusTip = 'Exit Application',
                                            triggered = self.close)
                                            
        self.startPythonAction = QAction(QIcon(mypath+'python.png'),
                                               'Start iPython',self,
                                               statusTip = 'Start iPython',
                                               triggered = self.startpythonAct)

        self.runAction = QAction(QIcon(mypath+'run.png'),
                                           'Simulate',self,
                                           statusTip = 'Simulate',
                                           triggered = self.runAct)

        self.codegenAction = QAction(QIcon(mypath+'codegen.png'),
                                               'Generate C-code',self,
                                               shortcut = 'Ctrl+B',
                                               statusTip = 'Generate C-Code',
                                               triggered = self.codegenAct)

        self.setCodegenAction = QAction(QIcon(mypath+'settings.png'),
                                                'Block settings',self,
                                                statusTip = 'Block settings',
                                                triggered = self.setcodegenAct)

        self.debugAction = QAction(QIcon(mypath+'debug.png'),
                                             'Debugging',self,
                                             statusTip = 'Debug infos',
                                             triggered = self.debugAct)                                           
    def addToolbars(self):
        toolbarF = self.addToolBar('File')
        toolbarF.addAction(self.newFileAction)
        toolbarF.addAction(self.openFileAction)
        toolbarF.addAction(self.saveFileAction)
        toolbarF.addAction(self.saveFileAsAction)
        toolbarF.addAction(self.printAction)
        toolbarF.addAction(self.exitAction)

        toolbarE = self.addToolBar('Edit')
        toolbarE.addAction(self.copyAction)
        toolbarE.addAction(self.pasteAction)
        toolbarE.addAction(self.undoAction)
        #toolbarE.addAction(self.updateAction)

        toolbarS = self.addToolBar('Simulation')
        toolbarS.addAction(self.runAction)
        toolbarS.addAction(self.codegenAction)
        toolbarS.addAction(self.setCodegenAction)

        toolbarP = self.addToolBar('Python')
        toolbarP.addAction(self.startPythonAction)
        if DEBUG:
            toolbarD = self.addToolBar('Debug')
            toolbarD.addAction(self.debugAction)

    def addMenubar(self):
        menubar = self.menuBar()
        fileMenu = menubar.addMenu('&File')
        fileMenu.addAction(self.openFileAction)
        fileMenu.addAction(self.saveFileAction)
        fileMenu.addAction(self.saveFileAsAction)
        fileMenu.addSeparator()
        fileMenu.addAction(self.exitAction)
        
        editMenu = menubar.addMenu('&Edit')
        editMenu.addAction(self.copyAction)
        editMenu.addAction(self.pasteAction)
        editMenu.addAction(self.undoAction)
        editMenu.addSeparator()
        editMenu.addAction(self.updateAction)

        simMenu = menubar.addMenu('&Simulation')
        simMenu.addAction(self.runAction)
        simMenu.addAction(self.codegenAction)

        setMenu = menubar.addMenu('Se&ttings')
        setMenu.addAction(self.setCodegenAction)

    def copyAct(self):
        self.scene.selection = []
        p = self.scene.selectionArea()
        self.scene.selection = self.scene.items(p)
        if self.scene.selection == []:
            self.scene.selection = self.scene.selectedItems()

    def pasteAct(self):
        if self.scene.selection != []:
            for item in self.scene.selection:
                if isinstance(item, Connection):
                    try:
                        c = item.clone(QPointF(200,200))
                        c.update_ports_from_pos()
                    except:
                        pass        

    def undoAct(self):
         self.scene.undoDgm()
    
    def updateAct(self):
        self.scene.updateDgm()
        
    def getFullFileName(self):
        return(self.path + '/' + self.filename + '.dgm')

    def askSaving(self):
        items = self.scene.items()
        if len(items) == 0:
            return QMessageBox.Discard
        
        ret = QMessageBox.question(self, 'The Document has been modified',
                                   'Do you want to save your changes?',
                                   QMessageBox.Save | QMessageBox.Discard | QMessageBox.Cancel,
                                   QMessageBox.Cancel)
        return ret
            
    def newFile(self):
        fname = self.filename
        try:
            os.remove(fname+'.py')
        except:
            pass
        if self.modified:
            ret = self.askSaving()
            if ret == QMessageBox.Save:
                self.saveFile()
            elif ret == QMessageBox.Cancel:
                return
            
        self.scene.newDgm()
        self.filename = 'untitled'
        self.path = os.getcwd()
        self.setWindowTitle(self.filename)
        self.modified = False
        
    def openFile(self):
        fname = self.filename
        try:
            os.remove(fname+'.py')
        except:
            pass
        if self.modified:
            ret = self.askSaving()
            if ret == QMessageBox.Save:
                self.saveFile()
            elif ret == QMessageBox.Cancel:
                return
        self.scene.newDgm()            
                
        filename = QFileDialog.getOpenFileName(self, 'Open', '.', filter='*.dgm')
        filename = filename[0]
        if filename != '':
            fname = QFileInfo(filename)
            self.filename = str(fname.baseName())
            self.path = str(fname.absolutePath())
            self.setWindowTitle(self.filename)
            self.scene.loadDgm(self.getFullFileName())
            self.editor.redrawNodes()
            self.modified = False
        
    def saveFile(self):
        if self.filename == 'untitled':
            self.saveFileAs()
        else:
            filename = self.filename
            if filename != '':
                fname = QFileInfo(filename)
                self.filename = str(fname.baseName())
                self.path = str(fname.absolutePath())
                self.setWindowTitle(self.filename)
                self.scene.saveDgm(self.getFullFileName())
                self.modified = False

    def saveFileAs(self):
        filename = QFileDialog.getSaveFileName(self, 'Save', self.path+'/'+self.filename, filter='*.dgm')
        filename = filename[0]
        if filename != '':
            fname = QFileInfo(filename)
            self.filename = str(fname.baseName())
            self.path = str(fname.absolutePath())
            self.setWindowTitle(self.filename)
            self.scene.saveDgm(self.getFullFileName())
            self.modified = False

    def print_scheme(self):
        self.printer = QPrinter()
        printDialog = QPrintDialog(self.printer)
        if (printDialog.exec_() == QDialog.Accepted):
            painter = QPainter(self.printer)
            painter.setRenderHint(QPainter.Antialiasing)
            painter.setRenderHint(QPainter.TextAntialiasing)
            self.scene.clearSelection()
            self.scene.render(painter)

    def parBlock(self):
        item = self.scene.item
        dialog = IO_Dialog(self)
        dialog.spbInput.setValue(item.inp)
        dialog.spbOutput.setValue(item.outp)
        if item.insetble==False:
            dialog.spbInput.setEnabled(False)
        if item.outsetble==False:
            dialog.spbOutput.setEnabled(False)
            
        name = item.name
        flip = item.flip
        icon = item.icon
        params = item.params
        insetble = item.insetble
        outsetble = item.outsetble
        width = item.width
        res = dialog.exec_()
        if res == 1 and (insetble or outsetble):
            item.remove()
            inp = dialog.spbInput.value()
            outp = dialog.spbOutput.value()
            b = Block(None, self.scene, name, inp, outp, insetble, outsetble, icon, params, width, flip)
            b.setPos(self.scene.evpos)
            ok = True
        else:
            ok = False
        return ok

    def startpythonAct(self):
        os.system(pycmd)

    def debugAct(self):
        self.scene.debugInfo()

    def runAct(self):
         self.scene.simrun()

    def codegenAct(self):
        self.scene.codegen(True)

    def setrunAct(self):
        self.scene.runDlg()

    def setcodegenAct(self):
        self.scene.codegenDlg()

    def closeEvent(self,event):          
        try:
            os.remove('tmp.py')
        except:
            pass
        
        if self.modified:
            ret = self.askSaving()
            if ret == QMessageBox.Save:
                self.saveFile()
            elif ret == QMessageBox.Cancel:
                event.ignore()
                return
            
        self.library.closeFlag = True
        self.library.close()
        event.accept()
                
