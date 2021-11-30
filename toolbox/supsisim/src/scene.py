from PyQt5.QtWidgets import QGraphicsItem, QGraphicsScene, QGraphicsView
from PyQt5.QtGui import QPainter
from PyQt5.QtCore import QRectF, QPointF, QSizeF, QEvent

from supsisim.block import Block
from supsisim.port import Port, InPort, OutPort
from supsisim.connection import Connection
from supsisim.editor import IDLE
from supsisim.dialg import RTgenDlg
from supsisim.const import pyrun, TEMP, respath, BWmin
from lxml import etree
import os
import subprocess
import time

class GraphicsView(QGraphicsView):
    def __init__(self, parent=None):
        super(GraphicsView, self).__init__(parent)
        self.setDragMode(QGraphicsView.RubberBandDrag)
        self.setSceneRect(QRectF(-2000, -2000, 4000, 4000))
        self.setRenderHint(QPainter.Antialiasing)
        self.setRenderHint(QPainter.TextAntialiasing)
        self.setAcceptDrops(True)
        
    def wheelEvent(self, event):
        factor = 1.41 ** (-event.angleDelta().y() / 240.0)
        self.scale(factor, factor)

class Scene(QGraphicsScene):
    def __init__(self, main, parent=None):
        super(Scene,self).__init__(parent)
        self.mainw = main

        self.selection = []
        self.currentItem = None
        self.blocks = set()
        
        self.template = 'sim.tmf'
        self.addObjs = ''
        self.Ts = '0.01'
        self.script = ''
        self.Tf = '10'
        self.prio = ''

        self.undoList = []

    def dragMoveEvent(self, event):
        if event.mimeData().hasText():
            event.accept()
        else:
            event.ignore()
            
    def dragLeaveEvent(self, event):
        return

    def dropEvent(self,event):
        if event.mimeData().hasText():
            self.DgmToUndo()
            msg = event.mimeData().text()
            root = etree.fromstring(msg)
            item = root.findall('block')[0]
            b = Block(None, self, item.findtext('name'),
                      int(item.findtext('inp')), int(item.findtext('outp')),
                      item.findtext('inset')=='1', item.findtext('outset')=='1',
                      item.findtext('icon'),
                      item.findtext('params'), item.findtext('help'),
                      int(item.findtext('width')), item.findtext('flip')=='1' )
            b.setPos(event.scenePos())
       
    def DgmToMsg(self):
        items = self.items()
        dgmBlocks = []
        dgmConnections = []
        for item in items:
            if isinstance(item, Block):
                dgmBlocks.append(item)
            elif isinstance(item, Connection):
                dgmConnections.append(item)
            else:
                pass

        root = etree.Element('root')
        vers = etree.SubElement(root,'pysimCoder')
        etree.SubElement(vers, 'pysimCoderVersion').text = '0.9'
        now = etree.SubElement(root,'Date')
        etree.SubElement(now, 'SavingDate').text = time.strftime("%d.%m.%Y - %H:%M:%S")
        sim = etree.SubElement(root,'Simulation')
        etree.SubElement(sim,'Template').text = self.template
        etree.SubElement(sim,'Ts').text = self.Ts
        etree.SubElement(sim,'AddObj').text = self.addObjs
        etree.SubElement(sim,'ParScript').text = self.script
        etree.SubElement(sim,'Tf').text = self.Tf
        etree.SubElement(sim,'Priority').text = self.prio
        for item in dgmBlocks:
            item.save(root)
        for item in dgmConnections:
            item.save(root)
        msg = etree.tostring(root, pretty_print=True)
        return msg

    def clearDgm(self):
        items = self.items()
        for item in items:
            for thing in item.childItems():
                if isinstance(thing, Port):
                    for conn in thing.connections:
                        conn.remove()
            self.removeItem(item)
        self.blocks.clear()
        
    def MsgToDgm(self, msg):
        self.clearDgm()
        root = etree.fromstring(msg)
        sim = root.findall('Simulation')[0]
        self.template = sim.findtext('Template')
        self.Ts = sim.findtext('Ts')
        self.addObjs = sim.findtext('AddObj')
        if self.addObjs==None or self.addObjs=='':
            self.addObjs=''
        self.script = sim.findtext('ParScript')
        if self.script==None or self.script=='':
            self.script=''
        self.Tf = sim.findtext('Tf')
        if self.Tf==None:
            self.Tf=''
        self.prio = sim.findtext('Priority')
        if self.prio==None:
            self.prio=''
            
        blocks = root.findall('block')
        for item in blocks:
            self.loadBlock(item)
        connections = root.findall('connection')
        for item in connections:
            self.loadConn(item)
        try:
            self.mainw.editor.redrawNodes()
        except:
            pass
            
    def newDgm(self):
        self.clearDgm()
        self.addObjs=''
        self.script=''
        self.Tf='10.0'
        self.undoList = []

    def clearLastUndo(self):
        if len(self.undoList) > 1:
            msg = self.undoList.pop()

    def DgmToUndo(self):
        self.mainw.modified = True
        msg = self.DgmToMsg()
        self.undoList.append(msg)

    def undoDgm(self):
        if len(self.undoList) > 1:
            msg = self.undoList.pop()
        else:
            msg = self.undoList[0]
            self.mainw.modified = False
            
        self.MsgToDgm(msg)
        self.mainw.editor.state = IDLE
        
    def updateDgm(self):
        items = self.items()
                    
        for item in items:
            if isinstance(item, Block):
                pos = item.pos()
                item.setPos(pos)
                    
    def saveDgm(self,fname):
        f = open(fname,'w')
        msg = self.DgmToMsg()
        msg = msg.decode()
        f.write(msg)
        f.close()

    def loadDgm(self, fname):
        f = open(fname,'r')
        msg = f.read()
        f.close()
        self.MsgToDgm(msg)
        self.undoList = [msg]
              
    def loadBlock(self, item, dx = 0, dy = 0):
        b = Block(None, self, item.findtext('name'),
                  int(item.findtext('inp')), int(item.findtext('outp')),
                  item.findtext('inset')=='1', item.findtext('outset')=='1', item.findtext('icon'),
                  item.findtext('params'), item.findtext('help'),
                  int(item.findtext('width')), item.findtext('flip')=='1' )
        b.setPos(float(item.findtext('posX'))+dx, float(item.findtext('posY'))+dy)

    def find_itemAt(self, pos):
        items = self.items(QRectF(pos-QPointF(1,1), QSizeF(3,3)))
        for item in items:
            if isinstance(item, QGraphicsItem) and not isinstance(item, Connection):
                return item
        return None
    
    def loadConn(self, item, dx = 0.0, dy = 0.0):
        c = Connection(None, self)
        c.load(item, dx, dy)            
        
    def setParamsBlk(self):
        self.mainw.paramsBlock()

    def codegenDlg(self):
        dialog = RTgenDlg(self)
        dialog.template.setText(self.template)
        dialog.addObjs.setText(self.addObjs)
        dialog.Ts.setText(self.Ts)
        dialog.parscript.setText(self.script)
        dialog.Tf.setText(self.Tf)
        dialog.prio.setText(self.prio)
        res = dialog.exec_()
        if res != 1:
            return

        self.template = str(dialog.template.text())
        self.addObjs = str(dialog.addObjs.text())
        self.Ts = str(dialog.Ts.text())
        self.script = str(dialog.parscript.text())
        self.prio =  str(dialog.prio.text())
        self.Tf = str(dialog.Tf.text())
        
    def codegen(self, flag):
        try:
            items = self.items()
            dgmBlocks = []
            for item in items:
                if isinstance(item, Block):
                    dgmBlocks.append(item)
                else:
                    pass

            nid = 1
            for item in dgmBlocks:
                for thing in item.childItems():
                    if isinstance(thing, OutPort):
                        thing.nodeID = str(nid)
                        nid += 1
                        
            for item in dgmBlocks:
                for thing in item.childItems():
                    if isinstance(thing, InPort):
                        try:
                            c = thing.connections[0]
                        except:
                            print('Problem in diagram: input signals probably not connected!')
                        while not isinstance(c.port1, OutPort):
                            try:
                                c = c.port1.parent.port_in.connections[0]
                            except (AttributeError, ValueError):
                                raise ValueError('Problem in diagram: outputs connected together!')
                        thing.nodeID = c.port1.nodeID
                        
            self.generateCCode()
            self.mainw.statusLabel.setText('Code generation OK!')
            try:
                os.mkdir('./' + self.mainw.filename + '_gen')
            except:
                pass
            if flag:
                cmd = pyrun + ' tmp.py'
                try:
                    p = subprocess.Popen(cmd, shell=True)
                except:
                    pass
                p.wait()
            return True
        
        except:
            self.mainw.statusLabel.setText('Error by Code generation!')
            return False
        
    def blkInstance(self, item):
        ln = item.params.split('|')
        txt = item.name.replace(' ','_') + ' = ' + ln[0] + '('
        if item.inp != 0:
            inp = '['
            for thing in item.childItems():
                if isinstance(thing, InPort):
                    inp += thing.nodeID +','
            inp = inp.rstrip(',') + ']'
            txt += inp + ','
            
        if item.outp != 0:
            outp = '['
            for thing in item.childItems():
                if isinstance(thing, OutPort):
                    outp += thing.nodeID +','
            outp = outp.rstrip(',') + ']'
            txt += outp +','
        txt = txt.rstrip(',')
        N = len(ln)
        for n in range(1,N):
            par = ln[n].split(':')
            txt += ', ' + par[1].__str__()

        # Check if Block is PLOT
        if ln[0] == 'plotBlk':
            txt += ", '" + item.name.replace(' ','_') + "'"
        
        txt += ')'
        txt = txt.replace('(, ', '(')
        return txt
    
    def generateCCode(self):
        try:
            f = open(self.script,'r')
            txt = f.read()
            f.close()
            txt += '\n'
        except:
            txt = ''
 
        items = self.items()
        dir1 = respath + 'blocks/rcpBlk'
        txt += 'import os\n\n'
        
        txt += 'from supsisim.RCPblk import RCPblk\n\n'
        txt += 'dirs = os.listdir("' + dir1 + '")\n'
        txt += 'for el in dirs:\n'
        txt += '    files = os.listdir("' + dir1 + '" + "/" + el)\n'
        txt += '    for f in files:\n'
        txt += '        if not f.endswith(".py"):\n'
        txt += '            continue\n'
        txt += '        f = f.rstrip("*.py")\n'
        txt += '        try:\n'
        txt += '            cmd = "from " + el + "." + f + " import *"\n'
        txt += '            exec(cmd)\n'
        txt += '        except:\n'
        txt += '            print("import of block class failed " + cmd)\n'
        txt += '            pass\n'
       
        txt += 'from supsisim.RCPgen import *\n'
        txt += 'from control import *\n'
        blkList = []
        for item in items:
            if isinstance(item, Block):
                blkList.append(item.name.replace(' ','_'))
                txt += self.blkInstance(item) + '\n'
        fname = self.mainw.filename
        fn = open('tmp.py','w')
        fn.write(txt)
        fn.write('\n')
        txt = 'blks = ['
        for item in blkList:
            txt += item + ','
        txt += ']\n'
        fn.write(txt)
        fnm = './' + fname + '_gen'
                
        fn.write('fname = ' + "'" + fname + "'\n")
        fn.write('os.chdir("'+ fnm +'")\n')
        fn.write('genCode(fname, ' + self.Ts + ', blks)\n')
        fn.write("genMake(fname, '" + self.template + "', addObj = '" + self.addObjs + "')\n")
        fn.write('\nimport os\n')
        fn.write('os.system("make")\n')
        fn.write('os.chdir("..")\n')
        fn.close()

    def simrun(self):
        if self.codegen(False):
            cmd  = '\n'
            cmd += 'import matplotlib.pyplot as plt\n'
            cmd += 'import numpy as np\n\n'
            prio = self.prio.replace(' ','')
            if prio != '':
                prio = ' -p ' + prio
            cmd += 'os.system("./' + self.mainw.filename + prio + ' -f ' + self.Tf + '")\n'
            fnm = self.mainw.filename
            cmd += 'os.system("' + 'rm -r ' + fnm + ' ' + fnm + '_gen' + '" )'

            try:
                os.mkdir(TEMP + '/' + self.mainw.filename + '_gen')
            except:
                pass
            f = open('tmp.py','a')
            f.write(cmd)
            f.close()
            cmd = pyrun + ' tmp.py'
            try:
                os.system(cmd)
                self.mainw.statusLabel.setText('Simulation finished')
            except:
                pass
         
    def debugInfo(self):
        items = self.items()
        dgmBlocks = []
        dgmConnections = []
        for item in items:
            if isinstance(item, Block):
                dgmBlocks.append(item)
            elif isinstance(item, Connection):
                dgmConnections.append(item)
            else:
                pass
        print('Blocks:')
        for item in dgmBlocks:
            print(item)
        print('\nConnections:')
        for item in dgmConnections:
            print(item)
