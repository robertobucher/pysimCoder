from PyQt5.QtWidgets import QDialog, QGridLayout, QSpinBox, QLabel, QPushButton, \
    QLineEdit, QFileDialog, QVBoxLayout, QTextEdit, QDialogButtonBox, QApplication
from PyQt5.QtGui import QFont
from PyQt5.QtCore import Qt

from supsisim.const import path

class IO_Dialog(QDialog):
    def __init__(self,parent=None):
        super(IO_Dialog, self).__init__(parent)
        layout = QGridLayout()
        self.setWindowModality(Qt.ApplicationModal)
        self.resize(380, 180)
        self.spbInput = QSpinBox()
        self.spbOutput = QSpinBox()
        self.spbInput.setValue(1)
        self.spbOutput.setValue(1)

        label2 = QLabel('Number of inputs:')
        label3 = QLabel('Number of outputs')
        self.pbOK = QPushButton('OK')
        self.pbCANCEL = QPushButton('CANCEL')
        layout.addWidget(self.spbInput,0,1)
        layout.addWidget(self.spbOutput,1,1)
        layout.addWidget(label2,0,0)
        layout.addWidget(label3,1,0)
        layout.addWidget(self.pbOK,2,0)
        layout.addWidget(self.pbCANCEL,2,1)
        self.setLayout(layout)
        self.pbOK.clicked.connect(self.accept)
        self.pbCANCEL.clicked.connect(self.reject)
        
class BlockName_Dialog(QDialog):
    def __init__(self,parent=None):
        super(BlockName_Dialog, self).__init__(parent)
        layout = QGridLayout()
        self.setWindowModality(Qt.ApplicationModal)
        self.resize(380, 100)
        self.name = QLineEdit()

        label1 = QLabel('Block ID:')
        self.pbOK = QPushButton('OK')
        self.pbCANCEL = QPushButton('CANCEL')
        layout.addWidget(label1,0,0)
        layout.addWidget(self.name,0,1)
        layout.addWidget(self.pbOK,2,0)
        layout.addWidget(self.pbCANCEL,2,1)
        self.setLayout(layout)
        self.pbOK.clicked.connect(self.accept)
        self.pbCANCEL.clicked.connect(self.reject)

class RTgenDlg(QDialog):
    def __init__(self, parent=None):
        super(RTgenDlg, self).__init__(None)
        self.setWindowModality(Qt.ApplicationModal)
        self.resize(600, 100)
        
        lab1 = QLabel('Template Makefile')
        self.template = QLineEdit('')
        btn_template = QPushButton('BROWSE...')
        lab2 = QLabel('Parameter script')
        self.parscript = QLineEdit('')
        btn_script = QPushButton('BROWSE...')
        lab3 = QLabel('Additional Objs')
        self.addObjs = QLineEdit('')
        btn_addObjs = QPushButton('BROWSE...')
        lab4 = QLabel('Sampling Time')
        self.Ts = QLineEdit('')
        lab5 = QLabel('Final Time')
        self.Tf = QLineEdit('')
        lab6 = QLabel('Priority')
        self.prio = QLineEdit('')
        
        pbOK = QPushButton('OK')
        pbCANCEL = QPushButton('CANCEL')
        grid = QGridLayout()
        
        grid.addWidget(lab1, 0, 0)
        grid.addWidget(self.template, 0, 1)
        grid.addWidget(btn_template, 0, 2)
        grid.addWidget(lab2, 1, 0)
        grid.addWidget(self.parscript, 1, 1)
        grid.addWidget(btn_script, 1, 2)
        grid.addWidget(lab3, 2, 0)
        grid.addWidget(self.addObjs, 2, 1)
        grid.addWidget(btn_addObjs, 2, 2)
        grid.addWidget(lab6, 3, 0)
        grid.addWidget(self.prio, 3, 1)
        grid.addWidget(lab4, 4, 0)
        grid.addWidget(self.Ts, 4, 1)
        grid.addWidget(lab5, 5, 0)
        grid.addWidget(self.Tf, 5, 1)
        grid.addWidget(pbOK, 6, 0)
        grid.addWidget(pbCANCEL, 6, 1)
        pbOK.clicked.connect(self.accept)
        pbCANCEL.clicked.connect(self.reject)
        btn_template.clicked.connect(self.getTemplate)
        btn_addObjs.clicked.connect(self.getObjs)
        btn_script.clicked.connect(self.getScript)
        self.setLayout(grid)

    def getTemplate(self):
        fname = QFileDialog.getOpenFileName(self,'Open Template Makefile',
                                                  path+'CodeGen/templates', 'Template (*.tmf)')
        fname = fname[0]
        if len(fname) != 0:
            ln = fname.split('/')
            templ = ln[-1].__str__()
            self.template.setText(templ)

    def getObjs(self):
        fname = QFileDialog.getOpenFileName(self,'Additional libraries',
                                                  '.','Dynamic libraries (*.so)')
        fname = fname[0]
        if len(fname) != 0:
            ln = fname.split('/')
            libname = ln[-1].__str__()
            self.addObjs.setText(libname)

    def getScript(self):
        fname = QFileDialog.getOpenFileName(self,'Open Python script',
                                                  '.', 'Python file (*.py)')
        fname = fname[0]
        if len(fname) != 0:
            #ln = fname.split('/')
            #script = ln[-1].__str__()
            self.parscript.setText(fname)

