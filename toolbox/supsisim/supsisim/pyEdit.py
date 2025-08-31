#!/usr/bin/python3

import os

from supsisim.qtvers import *

from supsisim.block import Block
from supsisim.subsblock import subsBlock
from supsisim.connection import Connection
from supsisim.editor import Editor
from supsisim.scene import Scene, GraphicsView
from supsisim.dialg import IO_Dialog
from supsisim.const import respath, pycmd, DP

import json

DEBUG = False

class NewEditorMainWindow(QMainWindow):
    def __init__(self, fname, mypath, parent, scene= None):
        super(NewEditorMainWindow, self).__init__(parent)
        self.filePath = mypath
        self.centralWidget = QWidget(self)
        self.verticalLayout = QVBoxLayout(self.centralWidget)
        self.view = GraphicsView(self.centralWidget)
        self.view.setMouseTracking(True)
        self.filename = fname
        if scene == None:
            self.scene = Scene(self)
            if fname != 'untitled':
                self.scene.loadDgm(self.getFullFileName())
        else:
            self.scene = scene
            self.scene.mainw = self
        self.view.setScene(self.scene)
        self.view.setRenderHint(QPainter.RenderHint.Antialiasing)
        self.verticalLayout.addWidget(self.view)
        self.setCentralWidget(self.centralWidget)
        self.addactions()
        self.addMenubar()
        self.addToolbars()
        self.workingFolder = os.getcwd()
        self.library = parent
        
        settings = QSettings('SUPSI', 'pysimCoder')
        try:
            recFolders = settings.value('RecentFolders')
            for el in recFolders:
                self.setFolders(el)
        except:
            pass
        self.actFolders.setCurrentIndex(0)
                
        self.setWindowTitle(self.filename)
        self.status = self.statusBar()
        self.statusLabel = QLabel('')
        self.status.addWidget(self.statusLabel)
        self.evpos = QPointF(0,0)
        self.editor = Editor(self)
        self.editor.install(self.scene)
        self.editor.redrawNodes()
        self.statusLabel.setText('Ready')
        
        self.notSubsystem = True

        self.modified = False

    def addactions(self):
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

        self.changeDirAction = QAction(QIcon(mypath+'folder.png'),
                                                '&Change directory', self,
                                                shortcut = 'Ctrl+D',
                                                statusTip = 'Change directory...',
                                                triggered = self.changeDirAct)

        self.clearDirAction = QAction(QIcon(mypath+'edit-clear.png'),
                                                '&Clear directory list', self,
                                                shortcut = 'Ctrl+L',
                                                statusTip = 'Clear directory list',
                                                triggered = self.clearDirAct)
         
        self.copyAction = QAction(QIcon(mypath+'copy.png'),
                                            '&Copy', self,
                                            shortcut = 'Ctrl+C',
                                            statusTip = 'Copy',
                                            triggered = self.copyAct)

        self.cutAction = QAction(QIcon(mypath+'cut.png'),
                                            'C&ut', self,
                                            shortcut = 'Ctrl+X',
                                            statusTip = 'Cut',
                                            triggered = self.cutAct)

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
                                                'Settings',self,
                                                statusTip = 'Settings',
                                                triggered = self.setcodegenAct)

        self.setSHVAction = QAction(QIcon(mypath+'shv.png'),
                                                'SHV Support',self,
                                                statusTip = 'SHV Support',
                                                triggered = self.setSHVAct)

        self.setUpdimgAction = QAction(QIcon(mypath+'updateimg.png'),
                                                'Upload and update firmware',self,
                                                statusTip = 'Upload and update firmware',
                                                triggered = self.setupdimgAct)

        self.showLibAction = QAction(QIcon(mypath+'library.png'),
                                     'Show/Hide Block Library',self,
                                     statusTip = 'Show/Hide Block Library',
                                     triggered = self.showLibAct)

        self.debugAction = QAction(QIcon(mypath+'debug.png'),
                                             'Debugging',self,
                                             statusTip = 'Debug infos',
                                             triggered = self.debugAct)
                                             
        self.aboutAction = QAction(QIcon(mypath+'help-about.png'),
                                             'About pysimCoder',self,
                                             statusTip = 'About pysimCoder',
                                             triggered = self.aboutAct)

    def addToolbars(self):
        toolbarL = self.addToolBar('Library')
        toolbarL.addAction(self.showLibAction)
        toolbarF = self.addToolBar('File')
        toolbarF.addAction(self.newFileAction)
        toolbarF.addAction(self.openFileAction)
        toolbarF.addAction(self.saveFileAction)
        toolbarF.addAction(self.saveFileAsAction)
        toolbarF.addAction(self.changeDirAction)

        toolbarP = self.addToolBar('Print')
        toolbarP.addAction(self.printAction)
 
        toolbarE = self.addToolBar('Edit')
        toolbarE.addAction(self.cutAction)
        toolbarE.addAction(self.copyAction)
        toolbarE.addAction(self.pasteAction)
        toolbarE.addAction(self.undoAction)
        #toolbarE.addAction(self.updateAction)

        toolbarS = self.addToolBar('Simulation')
        toolbarS.addAction(self.runAction)
        toolbarS.addAction(self.codegenAction)
        toolbarS.addAction(self.setCodegenAction)
        toolbarS.addAction(self.setSHVAction)
        toolbarS.addAction(self.setUpdimgAction)

        toolbarP = self.addToolBar('Python')
        toolbarP.addAction(self.startPythonAction)
        if DEBUG:
            toolbarD = self.addToolBar('Debug')
            toolbarD.addAction(self.debugAction)

        toolbarDir = self.addToolBar('Folder')
        self.actFolders = QComboBox()
        self.actFolders.setMaximumWidth(400)
        self.actFolders.setSizeAdjustPolicy(QComboBox.SizeAdjustPolicy.AdjustToContents)
        self.actFolders.addItem(os.getcwd())
        toolbarDir.addWidget(self.actFolders)
        self.actFolders.currentIndexChanged.connect(self.changeDir)
        toolbarDir.addAction(self.clearDirAction)

    def addMenubar(self):
        menubar = self.menuBar()
        libMenu = menubar.addMenu('&Library')
        libMenu.addAction(self.showLibAction)
        fileMenu = menubar.addMenu('&File')
        fileMenu.addAction(self.newFileAction)
        fileMenu.addAction(self.openFileAction)
        fileMenu.addAction(self.saveFileAction)
        fileMenu.addAction(self.saveFileAsAction)
        fileMenu.addAction(self.changeDirAction)

        editMenu = menubar.addMenu('&Edit')
        editMenu.addAction(self.cutAction)
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

        shvMenu = menubar.addMenu('&Communication')
        shvMenu.addAction(self.setSHVAction)
        
        helpMenu = menubar.addMenu('&Help')
        helpMenu.addAction(self.aboutAction)

    def copyAct(self):
        self.scene.selection = []
        self.scene.selection = self.scene.selectedItems()
        
        dgmBlocks = []
        dgmConnections = []
        dgmSubsystems = []                
        
        items = self.scene.selection
        for item in items:
            if isinstance(item, subsBlock):
                dgmSubsystems.append(item)
            elif isinstance(item, Block):
                dgmBlocks.append(item)
            else:
                pass
        for item in items:                
            if isinstance(item, Connection):
                if (item.port1.parent in dgmBlocks or \
                item.port1.parent in dgmSubsystems) and \
                (item.port2.parent in dgmBlocks or \
                item.port2.parent in dgmSubsystems):
                    dgmConnections.append(item)
                
        data = {}
        
        blk = []
        for item in dgmBlocks:
            b = item.save()
            blk.append(b)
        data['blocks'] = blk

        conn = []
        for item in dgmConnections:
            c = item.save()
            conn.append(c)
        data['connections'] = conn

        subs = []
        for item in dgmSubsystems:
            s = item.save()
            subs.append(s)
        data['subsystems'] = subs
        
        msg = json.dumps(data)
        clipboard = QApplication.clipboard()
        clipboard.setText(msg)

    def cutAct(self):
        self.copyAct()
        self.editor.deleteSelected()
            
    def pasteAct(self):
        self.scene.DgmToUndo()
        try:
            msg = QApplication.clipboard().text()
            data = json.loads(msg)
            self.scene.DictToDgm(data, DP, DP)
        except:
            pass
        
    def undoAct(self):
         self.scene.undoDgm()
    
    def updateAct(self):
        self.scene.updateDgm()
        
    def getFullFileName(self):
        return(self.filePath + '/' + self.filename + '.dgm')

    def askSaving(self):
        items = self.scene.items()
        if len(items) == 0:
            return QMessageBox.StandardButton.Discard
        
        ret = QMessageBox.question(self, 'The Document has been modified',
                                   'Do you want to save your changes?',
                                   QMessageBox.StandardButton.Save | QMessageBox.StandardButton.Discard | QMessageBox.StandardButton.Cancel,
                                   QMessageBox.StandardButton.Cancel)
        return ret
            
    def newFile(self):
        main = NewEditorMainWindow('untitled', self.workingFolder, self.library)
        self.library.mainWins.append(main)
        main.show()

    def openFile(self):
        filename = QFileDialog.getOpenFileName(self, 'Open', self.workingFolder, filter='*.dgm')
        filename = filename[0]
        if filename != '':
            self.fopen(filename)

    def fopen(self, filename, scene = None):
        fname = QFileInfo(filename)
        filePath = str(fname.absolutePath())
        fn = str(fname.baseName())
        main = NewEditorMainWindow(fn, filePath, self.library, scene)
        self.library.mainWins.append(main)
        main.show()
        
    def openSubs(self, name, scene):
        main = NewEditorMainWindow(name, self.filePath, self.library, scene)
        self.library.mainWins.append(main)
        main.notSubsystem = False
        main.show()
        
    def saveFile(self):
        if self.filename == 'untitled':
            self.saveFileAs()
        else:
            filename = self.filename
            if filename != '':
                self.scene.saveDgm(self.getFullFileName())
                self.modified = False

    def saveFileAs(self):
        filename = QFileDialog.getSaveFileName(self, 'Save', self.workingFolder+'/'+self.filename, filter='*.dgm')
        filename = filename[0]
        if filename != '':
            fname = QFileInfo(filename)
            self.filename = str(fname.baseName())
            self.filePath = str(fname.absolutePath())
            self.setWindowTitle(self.filename)
            self.scene.saveDgm(self.getFullFileName())
            self.modified = False

    def setFolders(self, dirname):
        itemIndex = self.actFolders.findText(dirname)
        if itemIndex == -1:
            self.actFolders.addItem(dirname);
            self.actFolders.setCurrentIndex(self.actFolders.count()-1)
        else:
            self.actFolders.setCurrentIndex(itemIndex)

    def changeDirAct(self):
        newDir = QFileDialog.getExistingDirectory(self, 'Select a folder:', '.', QFileDialog.Option.ShowDirsOnly)
        self.setFolders(newDir)

    def clearDirAct(self):
        self.actFolders.clear()
        self.actFolders.addItem(os.getcwd())
        
    def aboutAct(self):
        msg = QMessageBox(self)
        msg.setWindowTitle('About pysimCoder')
        text = 'https://github.com/robertobucher/pysimCoder\n\n'
        text += 'roberto.bucher.2812@gmail.com'
        text += '\n\n built on PyQt '
        text += QT_VERSION_STR
        msg.setText(text)
        pix = QPixmap(respath+'/icons/application-x-pysimcoder.svg')
        pix = pix.scaledToHeight(128)
        msg.setIconPixmap(pix)
        msg.setStandardButtons(QMessageBox.StandardButton.Close)
        ret = msg.exec()
        
    def changeDir(self, index):
        try:
            os.chdir(self.actFolders.itemText(index))
            self.workingFolder =  self.actFolders.itemText(index)
        except:
            pass
            
    def print_scheme(self):
        self.printer = QPrinter()
        printDialog = QPrintDialog(self.printer)
        if printDialog.exec():
            painter = QPainter(self.printer)
            painter.setRenderHint(QPainter.RenderHint.Antialiasing)
            painter.setRenderHint(QPainter.RenderHint.TextAntialiasing)
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
        helpTxt = item.helpTxt
        dims = item.dims
        res = dialog.exec()
        if res == 1 and (insetble or outsetble):
            item.remove()
            inp = dialog.spbInput.value()
            outp = dialog.spbOutput.value()
            b = Block(None, self.scene, name, inp, outp, insetble, outsetble,
                      icon, params, helpTxt, dims, flip)
            b.setPos(self.scene.evpos)
            ok = True
        else:
            ok = False
        return ok

    def startpythonAct(self):
        os.system(pycmd)

    def showLibAct(self):
        if self.library.isMinimized():
            self.library.setWindowState(Qt.WindowState.WindowMaximized)
        else:
            self.library.setWindowState(Qt.WindowState.WindowMinimized)
            
    def debugAct(self):
        self.scene.debugInfo()

    def runAct(self):
         self.scene.simrun()

    def codegenAct(self):
        return self.scene.codegen(True)

    def setrunAct(self):
        self.scene.runDlg()

    def setcodegenAct(self):
        self.scene.codegenDlg()

    def setSHVAct(self):
        self.scene.SHVSetDlg()

    def setupdimgAct(self):
        self.scene.updimgDlg()
        
    def getScene(self):
        return Scene(self)

    def closeEvent(self,event):          
        try:
            os.remove('tmp.py')
        except:
            pass
            
        if self.modified and self.notSubsystem:
            ret = self.askSaving()
            if ret == QMessageBox.StandardButton.Save:
                self.saveFile()
            elif ret == QMessageBox.StandardButton.Cancel:
                event.ignore()
                return

        settings = QSettings('SUPSI', 'pysimCoder')
        recFolders = []
        for index in range(0, self.actFolders.count()):
            recFolders.append(self.actFolders.itemText(index))
        if recFolders:
            recFolders = QVariant(recFolders)
        else:
            recFolders = QVariant()
            
        settings.setValue('RecentFolders', recFolders)
        self.editor.active = False
        self.library.closeWindow(self)
