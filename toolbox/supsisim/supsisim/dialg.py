from supsisim.qtvers import *

from supsisim.const import path
from supsisim.getTemplates import dictTemplates
from os import listdir

class IO_Dialog(QDialog):
    def __init__(self,parent=None):
        super(IO_Dialog, self).__init__(parent)
        layout = QGridLayout()
        self.setWindowModality(Qt.WindowModality.ApplicationModal)
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
        self.setWindowModality(Qt.WindowModality.ApplicationModal)
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
        self.setWindowModality(Qt.WindowModality.ApplicationModal)
        self.resize(600, 100)

        lab1 = QLabel('Template Makefile')
        self.template = QComboBox()
        self.template.addItems(listdir(path+'CodeGen/templates'))
        self.template.setEditable(False)

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
        lab7 = QLabel('Integration method')
        self.intgMethod = QComboBox()
#         template = str(self.template.currentText())
#         self.intgMethod.addItems(dictTemplates[template])
#         self.intgMethod.addItems(['standard RK4', 'gsl_odeiv2_step_rkf45'])

        pbOK = QPushButton('OK')
        pbCANCEL = QPushButton('CANCEL')
        grid = QGridLayout()

        grid.addWidget(lab1, 0, 0)
        grid.addWidget(self.template, 0, 1)
        grid.addWidget(lab2, 1, 0)
        grid.addWidget(self.parscript, 1, 1)
        grid.addWidget(btn_script, 1, 2)
        grid.addWidget(lab3, 2, 0)
        grid.addWidget(self.addObjs, 2, 1)
        grid.addWidget(btn_addObjs, 2, 2)
        grid.addWidget(lab6, 3, 0)
        grid.addWidget(self.prio, 3, 1)
        grid.addWidget(lab7, 4, 0)
        grid.addWidget(self.intgMethod, 4, 1)
        grid.addWidget(lab4, 5, 0)
        grid.addWidget(self.Ts, 5, 1)
        grid.addWidget(lab5, 6, 0)
        grid.addWidget(self.Tf, 6, 1)
        grid.addWidget(pbOK, 7, 0)
        grid.addWidget(pbCANCEL, 7, 1)
        pbOK.clicked.connect(self.accept)
        pbCANCEL.clicked.connect(self.reject)
        btn_addObjs.clicked.connect(self.getObjs)
        btn_script.clicked.connect(self.getScript)
        self.template.currentIndexChanged.connect(self.templateChanged)
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
            self.parscript.setText(fname)

    def templateChanged(self, ind):
        template = str(self.template.currentText())
        self.intgMethod.clear()
        self.intgMethod.addItems(dictTemplates[template])

class SHVDlg(QDialog):
    def __init__(self, parent=None):
        super(SHVDlg, self).__init__(None)
        self.setWindowModality(Qt.WindowModality.ApplicationModal)
        self.resize(600, 100)

        lab1 = QLabel('Enable SHV protocol')
        self.SHVused = QCheckBox('')
        self.SHVused.setChecked(False)
        lab2 = QLabel('Enable tuning over SHV')
        self.SHVtune = QCheckBox('')
        self.SHVtune.setChecked(False)
        lab3 = QLabel('SHV Broker IP')
        self.SHVip = QLineEdit('')
        lab4 = QLabel('SHV Broker Port')
        self.SHVport = QLineEdit('')
        lab5 = QLabel('SHV Broker User')
        self.SHVuser = QLineEdit('')
        lab6 = QLabel('SHV Broker Password')
        self.SHVpassw = QLineEdit('')
        lab7 = QLabel('SHV Device ID')
        self.SHVdevid = QLineEdit('')
        lab8 = QLabel('SHV Device Mount Point')
        self.SHVmount = QLineEdit('')
        lab9 = QLabel('SHV Tree Type')
        self.SHVtree = QComboBox()
        self.SHVtree.addItems(['GAVL', 'GSA', 'GSA_STATIC'])

        pbOK = QPushButton('OK')
        pbCANCEL = QPushButton('CANCEL')

        grid = QGridLayout()

        grid.addWidget(lab1, 0, 0)
        grid.addWidget(self.SHVused, 0, 1)
        grid.addWidget(lab2, 1, 0)
        grid.addWidget(self.SHVtune, 1, 1)
        grid.addWidget(lab3, 2, 0)
        grid.addWidget(self.SHVip, 2, 1)
        grid.addWidget(lab4, 3, 0)
        grid.addWidget(self.SHVport, 3, 1)
        grid.addWidget(lab5, 4, 0)
        grid.addWidget(self.SHVuser, 4, 1)
        grid.addWidget(lab6, 5, 0)
        grid.addWidget(self.SHVpassw, 5, 1)
        grid.addWidget(lab7, 6, 0)
        grid.addWidget(self.SHVdevid, 6, 1)
        grid.addWidget(lab8, 7, 0)
        grid.addWidget(self.SHVmount, 7, 1)
        grid.addWidget(lab9, 8, 0)
        grid.addWidget(self.SHVtree, 8, 1)
        grid.addWidget(pbOK, 9, 0)
        grid.addWidget(pbCANCEL, 9, 1)
        pbOK.clicked.connect(self.accept)
        pbCANCEL.clicked.connect(self.reject)

        self.setLayout(grid)
