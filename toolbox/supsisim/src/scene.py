from pyqt45 import QGraphicsView, QGraphicsScene, QGraphicsItem, QPainter, QtCore, use_pyqt


from supsisim.block import Block
from supsisim.port import Port, InPort, OutPort
from supsisim.node import Node
from supsisim.connection import Connection
from supsisim.dialg import RTgenDlg
from supsisim.const import pyrun, TEMP
from lxml import etree
import os
import subprocess
import time

class GraphicsView(QGraphicsView):
    def __init__(self, parent=None):
        super(GraphicsView, self).__init__(parent)
        self.setDragMode(QGraphicsView.RubberBandDrag)
        self.setSceneRect(QtCore.QRectF(-2000, -2000, 4000, 4000))
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

        self.nameList = []
        self.selection = []
        self.currentItem = None
        self.blocks = set()
        
        self.template = 'sim.tmf'
        self.addObjs = ''
        self.Ts = '0.01'
        self.script = ''
        self.Tf = '10'

        self.undoList = []

    def getIndex(self,name):
        try:
            index = self.nameList.index(name)
        except:
            index = -1
        return index

    def setUniqueName(self, block):
        cnt = 1
        name = block.name
        N = len(name)
        while str.isdigit(name[N-1]):
            N -= 1
        name = name[0:N]
        nm = name
        while self.getIndex(nm) != -1:
            nm = name + str(cnt)
            cnt += 1
        self.nameList.append(nm)
        self.nameList.sort()
        return nm
        
    def dragLeaveEvent(self, event):
        self.DgmToUndo()
        data = event.mimeData().text()
        b = Block(None, self, data)
        b.setPos(event.scenePos())

    def DgmToMsg(self):
        items = self.items()
        dgmBlocks = []
        dgmNodes = []
        dgmConnections = []
        for item in items:
            if isinstance(item, Block):
                dgmBlocks.append(item)
            elif isinstance(item, Node):
                dgmNodes.append(item)
            elif isinstance(item, Connection):
                dgmConnections.append(item)
            else:
                pass

        root = etree.Element('root')
        now = etree.SubElement(root,'Date')
        etree.SubElement(now, 'SavingDate').text = time.strftime("%d.%m.%Y - %H:%M:%S")
        sim = etree.SubElement(root,'Simulation')
        etree.SubElement(sim,'Template').text = self.template
        etree.SubElement(sim,'Ts').text = self.Ts
        etree.SubElement(sim,'AddObj').text = self.addObjs
        etree.SubElement(sim,'ParScript').text = self.script
        etree.SubElement(sim,'Tf').text = self.Tf
        for item in dgmBlocks:
            item.save(root)
        for item in dgmNodes:
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

        blocks = root.findall('block')
        for item in blocks:
            self.loadBlock(item)
        nodes = root.findall('node')
        for item in nodes:
            self.loadNode(item)
        connections = root.findall('connection')
        for item in connections:
            self.loadConn(item)
        
    def newDgm(self):
        self.clearDgm()
        self.addObjs=''
        self.script=''
        self.Tf='10.0'
        self.nameList = []
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
        
    def updateDgm(self):
        items = self.items()
        for item in items:
            if isinstance(item, Node):
                    if len(item.port_in.connections)==0 and len(item.port_out.connections)==0:
                            item.remove()
                    
        for item in items:
            if isinstance(item, Node):
                pN = item.pos()                   
                try:
                    Cn =  item.port_in.connections[0]
                    if isinstance(Cn.port1, OutPort):
                        pC = Cn.pos1
                        pN.setY(pC.y())
                        item.setPos(pN)
                        Cn.pos2 = pN
                        Cn.update_path()
                except:
                    pass
                    
        for item in items:
            if isinstance(item, Node):
                pN = item.pos()
                try:
                    Cn =  item.port_out.connections[0]
                    if isinstance(Cn.port2, InPort):
                        pC = Cn.pos2
                        pN.setY(pC.y())
                        item.setPos(pN)
                        Cn.pos1 = pN
                        Cn.update_path()
                except:
                    pass
                        
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
        self.nameList = []
        f = open(fname,'r')
        msg = f.read()
        f.close()
        self.MsgToDgm(msg)
        self.undoList = [msg]
              
    def loadBlock(self, item):
        b = Block(None, self, item.findtext('name'),
                      int(item.findtext('inp')), int(item.findtext('outp')),
                      item.findtext('ioset')=='1', item.findtext('icon'),
                      item.findtext('params'), item.findtext('flip')=='1' )
        b.setPos(float(item.findtext('posX')), float(item.findtext('posY')))

    def loadNode(self, item):
        n = Node(None, self)
        n.setPos(float(item.findtext('posX')), float(item.findtext('posY')))       

    def find_itemAt(self, pos):
        items = self.items(QtCore.QRectF(pos-QtCore.QPointF(1,1), QtCore.QSizeF(3,3)))
        for item in items:
            if isinstance(item, QGraphicsItem) and not isinstance(item, Connection):
                return item
        return None
    
    def loadConn(self, item):
        c = Connection(None, self)
        pt1 = QtCore.QPointF(float(item.findtext('pos1X')), float(item.findtext('pos1Y')))
        pt2 = QtCore.QPointF(float(item.findtext('pos2X')), float(item.findtext('pos2Y')))
        c.pos1 = pt1
        c.pos2 = pt2
        c.update_ports_from_pos()
        
    def setParamsBlk(self):
        self.mainw.paramsBlock()

    def codegenDlg(self):
        dialog = RTgenDlg(self)
        dialog.template.setText(self.template)
        dialog.addObjs.setText(self.addObjs)
        dialog.Ts.setText(self.Ts)
        dialog.parscript.setText(self.script)
        dialog.Tf.setText(self.Tf)
        res = dialog.exec_()
        if res != 1:
            return

        self.template = str(dialog.template.text())
        self.addObjs = str(dialog.addObjs.text())
        self.Ts = str(dialog.Ts.text())
        self.script = str(dialog.parscript.text())
        self.Tf = str(dialog.Tf.text())
        
    def codegen(self, flag):
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
                    c = thing.connections[0]
                    while not isinstance(c.port1, OutPort):
                        try:
                            c = c.port1.parent.port_in.connections[0]
                        except (AttributeError, ValueError):
                            raise ValueError('Problem in diagram: outputs connected together!')
                    thing.nodeID = c.port1.nodeID
        self.generateCCode()
        try:
            os.mkdir('./' + self.mainw.filename + '_gen')
        except:
            pass
        if flag:
            if self.mainw.runflag:
                cmd = pyrun + ' tmp.py'
                try:
                    p = subprocess.Popen(cmd, shell=True)
                except:
                    pass
                p.wait()
            else:
                print('Generate code -> run -i tmp.py')
        
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
        txt = ''
        if self.mainw.runflag:
            try:
                f = open(self.script,'r')
                txt = f.read()
                f.close()
                txt += '\n'
            except:
                pass

        items = self.items()
        txt += 'from supsisim.RCPblk import RCPblk\n'
        txt += 'from supsisim.pyeditBlocks import *\n'
        txt += 'try:\n'
        txt += '    from supsisim.dsPICblk import *\n'
        txt += 'except:\n'
        txt += '    pass\n'        
        txt += 'try:\n'
        txt += '    from blocks.userBlocks import *\n'
        txt += 'except:\n'
        txt += '    pass\n'        
        txt += 'from supsisim.RCPgen import *\n'
        txt += 'from control import *\n'
        txt += 'import os\n'
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
        self.codegen(False)
        cmd  = '\n'
        cmd += 'import matplotlib.pyplot as plt\n'
        cmd += 'import numpy as np\n\n'
        cmd += 'os.system("./' + self.mainw.filename + ' -f ' + self.Tf + '")\n'
        
        try:
            os.mkdir(TEMP + '/' + self.mainw.filename + '_gen')
        except:
            pass
        f = open('tmp.py','a')
        f.write(cmd)
        f.close()
        if self.mainw.runflag:
            cmd = pyrun + ' tmp.py'
            try:
                os.system(cmd)
                self.mainw.status.showMessage('Simulation finished')
            except:
                pass
        else:
            print('Simulate system -> run -i tmp.py')
         
    def debugInfo(self):
        items = self.items()
        dgmBlocks = []
        dgmNodes = []
        dgmConnections = []
        for item in items:
            if isinstance(item, Block):
                dgmBlocks.append(item)
            elif isinstance(item, Node):
                dgmNodes.append(item)
            elif isinstance(item, Connection):
                dgmConnections.append(item)
            else:
                pass
        print('Blocks:')
        for item in dgmBlocks:
            print(item)
        print('\nNodes:')
        for item in dgmNodes:
            print(item)
        print('\nConnections:')
        for item in dgmConnections:
            print(item)
       
