from supsisim.qtvers import *

from supsisim.block import Block
from supsisim.subsblock import subsBlock
from supsisim.port import Port, InPort, OutPort
from supsisim.connection import Connection
from supsisim.dialg import RTgenDlg, SHVDlg
from supsisim.const import VERSION, pyrun, TEMP, respath, BWmin
from supsisim.client import BrokerConnection
from lxml import etree
import os
import subprocess
import time
import json


IDLE = 0

class GraphicsView(QGraphicsView):
    def __init__(self, parent=None):
        super(GraphicsView, self).__init__(parent)
        self.setDragMode(QGraphicsView.DragMode.RubberBandDrag)
        self.setSceneRect(QRectF(-2000, -2000, 4000, 4000))
        self.setRenderHint(QPainter.RenderHint.Antialiasing)
        self.setRenderHint(QPainter.RenderHint.TextAntialiasing)
        self.setAcceptDrops(True)

    def wheelEvent(self, event):
        factor = 1.41 ** (-event.angleDelta().y() / 240.0)
        self.scale(factor, factor)

class SHVInstance:
    def __init__(self, filename):
        self.used = False
        self.ip = '127.0.0.1'
        self.port = '3755'
        self.user = 'admin'
        self.passw = 'admin!123'
        self.devid = filename
        self.mount = 'test'

        self.tuned = False

        self.tree = 'GAVL'

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

        self.SHV = SHVInstance(self.mainw.filename)
    
        self.brokerConnection = BrokerConnection()

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
            blk = json.loads(msg)
            blk['pos'] = (event.scenePos().x(), event.scenePos().y())
            self.loadBlock(blk)

    def DgmToDict(self, dataDict):
        # Transform the block diagram into a python dict
        init = {'code': 'pysimCoder',
                'ver' : VERSION,
                'date' : time.strftime("%d.%m.%Y - %H:%M:%S"),
            }
        dataDict['init'] = init

        keys = ['template', 'Ts', 'AddObj', 'script', 'Tf', 'prio']
        vals = [self.template, self.Ts, self.addObjs, self.script, self.Tf, self.prio]
        dataDict['simulate'] = dict(zip(keys, vals))

        keys = ['used', 'ip', 'port', 'user', 'passwd', 'devid', 'mount', 'tree']
        vals = [self.SHV.used, self.SHV.ip, self.SHV.port, self.SHV.user, self.SHV.passw,
                self.SHV.devid, self.SHV.mount, self.SHV.tree]
        dataDict['SHV'] = dict(zip(keys, vals))

        self.saveItems(dataDict)

    def saveItems(self, dataDict):
        items = self.items()

        dgmBlocks = []
        dgmSubsystem = []
        dgmConnections = []

        for item in items:
            if isinstance(item, subsBlock):
                dgmSubsystem.append(item)
            elif isinstance(item, Block):
                dgmBlocks.append(item)
            elif isinstance(item, Connection):
                dgmConnections.append(item)
            else:
                pass

        blk = []
        for item in dgmBlocks:
            b = item.save()
            blk.append(b)
        dataDict['blocks'] = blk

        conn = []
        for item in dgmConnections:
            c = item.save()
            conn.append(c)
        dataDict['connections'] = conn

        subs = []
        for item in dgmSubsystem:
            s = item.save()
            subs.append(s)
        dataDict['subsystems'] = subs

    def clearDgm(self):
        items = self.items()
        for item in items:
            for thing in item.childItems():
                if isinstance(thing, Port):
                    for conn in thing.connections:
                        conn.remove()
            self.removeItem(item)
        self.blocks.clear()

    def old_MsgToDgm(self, msg, dataDict):
    # Required for loading files saved with previous format
        QMessageBox.warning(self.mainw,'Old file format',
        'This file is in an old format that will\n  \
        no more supported in the future!\n \
        Please save it to transform in the new format!')
        root = etree.fromstring(msg)

        init = {'code': 'pysimCoder',
                'ver' : '0.91',
                'date' : time.strftime("%d.%m.%Y - %H:%M:%S"),
            }
        dataDict['init'] = init

        sim = root.findall('Simulation')[0]

        addObjs = sim.findtext('AddObj')
        if addObjs==None or addObjs=='':
            addObjs=''
        script = sim.findtext('ParScript')
        if script==None or script=='':
            script=''
        Tf = sim.findtext('Tf')
        if Tf==None:
            Tf=''
        prio = sim.findtext('Priority')
        if prio==None:
            prio=''
        simulate= {
                'template' : sim.findtext('Template'),
                'Ts' : sim.findtext('Ts'),
                'AddObj' : addObjs,
                'script' : script,
                'Tf' : Tf,
                'prio' : prio,
                }
        dataDict['simulate'] = simulate
        """
        We need to acess SHV field with try/except to keep support
        for older pysimCoder diagrams.
        """

        try:
            shv = root.findall('SHV')[0]
            used = (shv.findtext('SHVused') == "True")
            ip = shv.findtext('SHVip')
            port = shv.findtext('SHVport')
            user = shv.findtext('SHVuser')
            passw = shv.findtext('SHVpassw')
            devid = shv.findtext('SHVdevid')
            mount = shv.findtext('SHVmount')
            tree = shv.findtext('SHVtree')
            keys = ['used', 'ip', 'port', 'user', 'passwd', 'devid', 'mount', 'tree']
            vals = [used, ip, port, user, passw, devid, mount, tree]
            shv = dict(zip(keys, vals))
            dataDict['SHV'] = shv
        except:
            None

        blocks = root.findall('block')
        blk = []
        for item in blocks:
            b = self.getBlock(item)
            blk.append(b)
        dataDict['blocks'] = blk

        connections = root.findall('connection')
        conn = []
        for item in connections:
            c = self.getConnection(item)
            conn.append(c)
        dataDict['connections'] = conn

    def getBlock(self, item):
        pos = (float(item.findtext('posX')), float(item.findtext('posY')))
        vals = [item.findtext('name'), int(item.findtext('inp')), int(item.findtext('outp')),
                item.findtext('inset')=='1', item.findtext('outset')=='1', item.findtext('icon'),
                item.findtext('params'), item.findtext('help'),
                int(item.findtext('width')), item.findtext('flip')=='1', pos]
        keys = ['name', 'inp', 'outp', 'inset', 'outset', 'icon', 'params', 'help', 'width', 'flip', 'pos']
        return dict(zip(keys, vals))

    def getConnection(self, item):
        pos1 = (float(item.findtext('pos1X')), float(item.findtext('pos1Y')))
        pos2 = (float(item.findtext('pos2X')), float(item.findtext('pos2Y')))
        points = item.findall('pt')
        pts = []
        for el in points:
            pt = el.text.split(',')
            pt = (float(pt[0]), float(pt[1]))
            pts.append(pt)
        keys = ['pos1', 'pos2', 'points']
        vals = [pos1, pos2, pts]
        return dict(zip(keys, vals))

    def DictToDgm(self, dataDict, dx = 0, dy = 0):
        # Transform the file dict into a block diagram
        try:
            self.template = dataDict['simulate']['template']
            self.Ts = dataDict['simulate']['Ts']
            self.addObjs = dataDict['simulate']['AddObj']
            self.script = dataDict['simulate']['script']
            self.Tf = dataDict['simulate']['Tf']
            self.prio = dataDict['simulate']['prio']
        except:
            pass

        """
        We need to access SHV field with try/except to keep support
        for older pysimCoder diagrams.
        """
        try:
            self.SHV.used = dataDict['SHV']['used']
            self.SHV.ip = dataDict['SHV']['ip']
            self.SHV.port = dataDict['SHV']['port']
            self.SHV.user = dataDict['SHV']['user']
            self.SHV.passw = dataDict['SHV']['passwd']
            self.SHV.devid = dataDict['SHV']['devid']
            self.SHV.mount = dataDict['SHV']['mount']
            self.SHV.tree = dataDict['SHV']['tree']
        except:
            pass

        try:
            for item in dataDict['blocks']:
                self.loadBlock(item, dx, dy)
        except:
            pass

        try:
            for item in dataDict['subsystems']:
                self.loadSubsystem(item, dx, dy)
        except:
            pass

        try:
            for item in dataDict['connections']:
                self.loadConn(item, dx, dy)
        except:
            pass

        try:
            self.mainw.editor.redrawNodes()
        except:
            pass

    def loadBlock(self, item, dx = 0, dy = 0):
        b = Block(None, self, item['name'], item['inp'], item['outp'],
                  item['inset'], item['outset'], item['icon'],
                  item['params'], item['help'], item['width'], item['flip'] )

        b.setPos(item['pos'][0]+dx, item['pos'][1]+dy)

    def loadConn(self, item, dx = 0.0, dy = 0.0):
        c = Connection(None, self)
        c.load(item, dx, dy)

    def loadSubsystem(self, subs, dx = 0, dy = 0):
        item = subs['block']
        b = subsBlock(None, self, item['name'], item['inp'], item['outp'],
                  item['inset'], item['outset'], item['icon'],
                  item['params'], item['help'], item['width'], item['flip'] )

        b.setPos(item['pos'][0]+dx, item['pos'][1]+dy)
        b.load(subs)

    def clearLastUndo(self):
        if len(self.undoList) > 1:
            msg = self.undoList.pop()

    def DgmToUndo(self):
        fileDict = {}
        self.mainw.modified = True
        self.DgmToDict(fileDict)
        self.undoList.append(fileDict)

    def undoDgm(self):
        if len(self.undoList) > 1:
            dgm = self.undoList.pop()
        else:
            dgm = self.undoList[0]
            self.mainw.modified = False

        self.clearDgm()
        self.DictToDgm(dgm)
        self.mainw.editor.state = IDLE

    def updateDgm(self):
        items = self.items()

        for item in items:
            if isinstance(item, Block):
                pos = item.pos()
                item.setPos(pos)

    def saveDgm(self, fname):
        fileDict = {}
        self.DgmToDict(fileDict)
        f = open(fname,'w')
        js = json.dumps(fileDict)
        f.write(js)
        f.close()

    def loadDgm(self, fname):
        f = open(fname,'r')
        msg = f.read()
        f.close()
        try:
            root = etree.fromstring(msg)
            fileDict = {}
            self.clearDgm()
            self.old_MsgToDgm(msg, fileDict)
        except:
            fileDict = json.loads(msg)
            self.clearDgm()

        self.DictToDgm(fileDict)
        self.undoList = [fileDict]

    def find_itemAt(self, pos):
        items = self.items(QRectF(pos-QPointF(1,1), QSizeF(3,3)))
        for item in items:
            if isinstance(item, QGraphicsItem) and not isinstance(item, Connection):
                return item
        return None

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
        res = dialog.exec()
        if res != 1:
            return

        self.template = str(dialog.template.text())
        self.addObjs = str(dialog.addObjs.text())
        self.Ts = str(dialog.Ts.text())
        self.script = str(dialog.parscript.text())
        self.prio =  str(dialog.prio.text())
        self.Tf = str(dialog.Tf.text())

    def SHVSetDlg(self):
        dialog = SHVDlg(self)
        dialog.SHVused.setChecked(self.SHV.used)
        dialog.SHVtune.setChecked(self.SHV.tuned)
        dialog.SHVip.setText(self.SHV.ip)
        dialog.SHVport.setText(self.SHV.port)
        dialog.SHVuser.setText(self.SHV.user)
        dialog.SHVpassw.setText(self.SHV.passw)
        dialog.SHVdevid.setText(self.SHV.devid)
        dialog.SHVmount.setText(self.SHV.mount)
        dialog.SHVtree.setCurrentText(self.SHV.tree)
        res = dialog.exec()
        if res != 1:
            return

        self.SHV.used = dialog.SHVused.isChecked()
        self.SHV.tuned = dialog.SHVtune.isChecked()
        self.SHV.ip = str(dialog.SHVip.text())
        self.SHV.port = str(dialog.SHVport.text())
        self.SHV.user = str(dialog.SHVuser.text())
        self.SHV.passw = str(dialog.SHVpassw.text())
        self.SHV.devid = str(dialog.SHVdevid.text())
        self.SHV.mount = str(dialog.SHVmount.text())
        self.SHV.tree = str(dialog.SHVtree.currentText())

        if not self.SHV.tuned and self.brokerConnection is not None:
            self.brokerConnection.disconnect()

    def findAllItems(self, scene):
        items = []
        count = 0
        for item in scene.items():
            if isinstance(item, subsBlock):
                blk = item.getInternalBlocks()
                for el in blk:
                    el.setSysPath(f'/{item.name}')
                    el.ident = count
                    items.append(el)
            elif isinstance(item, Block):
                item.setSysPath('')
                item.ident = count
                items.append(item)

            else:
                pass
            
            count = count + 1

        items.sort(key=lambda p: p.name)
        return items

    def cleanBlkList(self, items):
        item2rem = []
        for item in items:
            if item.params == 'IOBlk':
                item2rem.append(item)
                pin, pout = item.getPorts()
                pInSub, pOutSub = item.subsParent.getPorts()
                for p in pin:
                    # This is a subsystem output!
                    n = int(item.name.lstrip('out_'))
                    for c in p.connections:
                        prev_port = c.port1
                        portSub =  pOutSub[n-1]
                        for cs in portSub.connections:
                            newConn = Connection(None, item.scene)
                            newConn.port1 = c.port1
                            newConn.pos1 = c.pos1
                            newConn.port2 = cs.port2
                            newConn.pos2 = cs.pos2
                            prev_port.connections.append(newConn)
                            try:
                                prev_port.connections.remove(c)
                            except:
                                pass
                            next_port = cs.port2
                            next_port.connections.append(newConn)
                            try:
                                next_port.connections.remove(cs)
                            except:
                                pass
                            item.scene.removeItem(newConn)

                for p in pout:
                    # This is a subsystem input!
                    n = int(item.name.lstrip('in_'))
                    for c in p.connections:
                        next_port = c.port2
                        portSub =  pInSub[n-1]
                        for cs in portSub.connections:
                            newConn = Connection(None, item.scene)
                            newConn.port1 = cs.port1
                            newConn.pos1 = cs.pos1
                            newConn.port2 = c.port2
                            newConn.pos2 = c.pos2
                            next_port.connections.append(newConn)
                            try:
                                next_port.connections.remove(c)
                            except:
                                pass
                            prev_port = cs.port1
                            prev_port.connections.append(newConn)
                            try:
                                prev_port.connections.remove(cs)
                            except:
                                pass
                            item.scene.removeItem(newConn)

        for item in item2rem:
            items.remove(item)

        return items

    def codegen(self, flag):
        dgmBlocks = self.findAllItems(self)

        # Clean Subsystems and reattach
        dgmBlocks = self.cleanBlkList(dgmBlocks)
        try:
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

            self.generateCCode(dgmBlocks)

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

            # Reset block diagram to previous state
            del(dgmBlocks)
            return True

        except:
            self.mainw.statusLabel.setText('Error by Code generation!')
            return False

    def blkInstance(self, item):
        ln = item.params.split('|')
        txt = item.getCodeName().replace(' ','_') + ' = ' + ln[0] + '('
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
        parArr = []
        for n in range(1,N):
            par = ln[n].split(':')
            txt += ', ' + par[1].__str__()
            parType = ""
            try:
                parType = str(par[2])
                parType = parType.replace(' ','')
            except:
                parType = None

            parArr.append((par[0], par[1], parType))

        # Check if Block is PLOT
        if ln[0] == 'plotBlk':
            txt += ", '" + item.getCodeName().replace(' ','_') + "'"

        txt += ')'
        txt = txt.replace('(, ', '(')
        return txt, parArr

    def generateCCode(self, items):
        try:
            f = open(self.script,'r')
            txt = f.read()
            f.close()
            txt += '\n'
        except:
            txt = ''

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
        txt += '        if True: #try:\n'
        txt += '            cmd = "from " + el + "." + f + " import *"\n'
        txt += '            exec(cmd)\n'
        txt += '        #except:\n'
        txt += '        #    print("import of block class failed " + cmd)\n'
        txt += '        #    pass\n'

        txt += 'from supsisim.RCPgen import *\n'
        txt += 'from control import *\n'

        blkList = []
        realParNames = []
        intParNames = []
        sysPathList = []
        for item in items:
            if isinstance(item, Block):
 #               print(item)
                blkList.append(item.getCodeName().replace(' ','_'))
                blkText, blkPar = self.blkInstance(item)
                txt += blkText + '\n'

                blkRealNames = []
                blkIntNames = []
                for par in blkPar:
                    if (par[2] == "double"):
                        blkRealNames.append(par[0])
                    elif (par[2] == "int"):
                        blkIntNames.append(par[0])

                sysPathList.append(item.syspath)
                realParNames.append(tuple(blkRealNames))
                intParNames.append(tuple(blkIntNames))

        fname = self.mainw.filename
        fn = open('tmp.py','w')
        fn.write(txt)
        fn.write('\n')

        for item in blkList:
            fn.write(item + '.name = \'' + item + '\'\n')

        txt = '\nblks = ['
        for item in blkList:
            txt += item + ','
        txt += ']\n\n'
        fn.write(txt)

        txt =  'realParNames = ' + str(tuple(realParNames)) + '\n'
        txt += 'intParNames = ' + str(tuple(intParNames)) + '\n'
        txt += 'sysPath = '+ str(sysPathList) + '\n\n'
        fn.write(txt)

        txt =  'tmp = 0\n'
        txt += 'for item in realParNames:\n'
        txt += '  for par in item:\n'
        txt += '    blks[tmp].realParNames.append(par)\n'
        txt += '  tmp += 1\n\n'
        txt += 'tmp = 0\n'
        txt += 'for item in intParNames:\n'
        txt += '  for par in item:\n'
        txt += '    blks[tmp].intParNames.append(par)\n'
        txt += '  tmp += 1\n\n'
        txt += 'tmp = 0\n'
        txt += 'for item in sysPath:\n'
        txt += '  blks[tmp].sysPath = item\n'
        txt += '  tmp += 1\n\n'  
        fn.write(txt)

        txt =  'os.environ["SHV_USED"] = \"' + str(self.SHV.used) + '\"\n'
        txt += 'os.environ["SHV_BROKER_IP"] = \"' + self.SHV.ip + '\"\n'
        txt += 'os.environ["SHV_BROKER_PORT"] = \"' + self.SHV.port + '\"\n'
        txt += 'os.environ["SHV_BROKER_USER"] = \"' + self.SHV.user + '\"\n'
        txt += 'os.environ["SHV_BROKER_PASSWORD"] = \"' + self.SHV.passw + '\"\n'
        txt += 'os.environ["SHV_BROKER_DEV_ID"] = \"' + self.SHV.devid + '\"\n'
        txt += 'os.environ["SHV_BROKER_MOUNT"] = \"' + self.SHV.mount + "/" + self.SHV.devid + '\"\n'
        txt += 'os.environ["SHV_TREE_TYPE"] = \"' + self.SHV.tree + '\"\n\n'
        fn.write(txt)

        fnm = './' + fname + '_gen'

        fn.write('fname = ' + "'" + fname + "'\n")
        fn.write('os.chdir("'+ fnm +'")\n')
        fn.write('genCode(fname, ' + self.Ts + ', blks)\n')
        fn.write("genMake(fname, '" + self.template + "', addObj = '" + self.addObjs + "')\n")
        fn.write('\nimport os\n')
        fn.write('os.system("make clean")\n')
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

    def getBrokerConnection(self) -> BrokerConnection:
        
        shv = self.SHV
        self.brokerConnection.update_parameters_and_connect(shv.ip, shv.port, shv.user, shv.passw, shv.devid, shv.mount)

        return self.brokerConnection

    def mousePressEvent(self, event):
        self.pos1 = event.scenePos()
        super(Scene,self).mousePressEvent(event)

    def mouseReleaseEvent(self, event):
        items = self.selectedItems()
        try:
            rect = QRectF(self.pos1, event.scenePos())
            for el in items:
                if isinstance(el, Connection):
                    if el.connInSelection(rect):
                        el.setSelected(True)
                    else:
                        el.setSelected(False)
        except:
            pass

        super(Scene,self).mouseReleaseEvent(event)
