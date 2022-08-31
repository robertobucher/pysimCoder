from PyQt5.QtWidgets import QGraphicsPathItem, QGraphicsTextItem, QMessageBox
from PyQt5.QtGui import QPainterPath, QPen, QImage, QTransform
from PyQt5.QtCore import Qt, QPointF

from supsisim.block import Block
from supsisim.connection import Connection
from supsisim.port import Port, InPort, OutPort
from supsisim.const import GRID, PW, LW, BWmin, BHmin, PD, respath

class subsBlock(Block):
    def __init__(self, *args):
        self.parent = args[0]
        self.scene = args[1]
        self.sceneSubs = self.scene.mainw.getScene()

        if len(args)==12:
            self.parent, self.scene, name, inp, outp, insetble, outsetble, \
            icon, params, helpTxt, width, flip = args
            super(subsBlock, self).__init__(self.parent, self.scene, name,
                        inp, outp, insetble, outsetble, icon,
                        params, helpTxt, width, flip)

        elif len(args)==3:
            self.parent = args[0]
            self.scene = args[1]

            self.blksList = args[2]

            self.setupIO()
            inp = len(self.inpP)
            outp = len(self.outpP)

            name = 'Subsystem'
            icon = 'SUBSYSTEM'
            params = 'SubsystemBlk'
            helpTxt = 'Superblock'
            width = BWmin
            flip = False
            insetble = False
            outsetble = False

            super(subsBlock, self).__init__(self.parent, self.scene, name,
                        inp, outp, insetble, outsetble, icon,
                        params, helpTxt, width, flip)

            self.blksNames = []
            for el in self.blksList:
                self.blksNames.append(el.name)

            SubsPosition = self.getPosition()
            self.setPos(SubsPosition)

            for el in self.blksList:
                self.scene.removeItem(el)
                self.sceneSubs.addItem(el)

            self.setIOPorts()

    def __str__(self):
        txt  = 'Name         :' + self.name.__str__() +'\n'
        txt += 'Input ports  :' + self.inp.__str__() + '\n'
        txt += 'Output ports :' + self.outp.__str__() + '\n'
        try:
            txt += 'Blocks in subsystem\n'
            for el in self.blksList:
                txt += el.name + '\n'
        except:
            pass
        return txt

    def getPosition(self):
        N = len(self.blksList)
        p = QPointF(0,0)
        for el in self.blksList:
            p += el.pos()
        return p/N

        return(QPointF(0,0))

    def setupIO(self):
        # Find number of inputs and outputs
        # Insert IO block
        inpP = []
        outpP = []
        for el in self.blksList:
            for el2 in el.childItems():
                if isinstance(el2, InPort):
                    blk = el2.connections[0].port1.parent
                    if blk not in self.blksList:
                        inpP.append(el2.connections[0].port1)

                if isinstance(el2, OutPort):
                    found = False
                    for el3 in el2.connections:
                        blk = el3.port2.parent
                        if (blk not in self.blksList) and not found:
                            outpP.append(el3.port1)

        inpP = list(set(inpP))
        outpP = list(set(outpP))
        inpP.sort(key=lambda p: p.parent.pos().y())
        outpP.sort(key=lambda p: p.parent.pos().y())
        inp = len(inpP)
        outp = len(outpP)
        self.inpP = inpP
        self.outpP = outpP

    def hideElements(self):
        for block in self.blksList:
            for el in block.childItems():
                try:
                    for conn in el.connections:
                        try:
                            self.scene.removeItem(conn)
                            self.sceneSubs.addItem(conn)
                        except:
                            pass
                except:
                    pass

    def newConn(self, port1, port2, scene):
        c = Connection(None, scene)
        c.port1 = port1
        c.pos1 = port1.scenePos()
        c.port2 = port2
        c.pos2 = port2.scenePos()
        p1 = self.gridPos(QPointF((c.pos1.x()+c.pos2.x())/2, c.pos1.y()))
        p2 = self.gridPos(QPointF((c.pos1.x()+c.pos2.x())/2, c.pos2.y()))
        c.connPoints.append(p1)
        c.connPoints.append(p2)
        c.update_ports_from_pos()
        c.update_path()
        return c

    def setIOPorts(self):
        # Find internal connections and put them in sceneSubs
        for block in self.sceneSubs.items():
            for p in block.childItems():
                if isinstance(p, OutPort):
                    for c in p.connections:
                        if c.port1.parent in self.sceneSubs.items() and \
                        c.port2.parent in self.sceneSubs.items():
                            self.scene.removeItem(c)
                            self.sceneSubs.addItem(c)

        # Subsystems ports
        subsIn, subsOut = self.getPorts()

        # Inputs ports
        for el in self.inpP:
            n = self.inpP.index(el)
            name = 'in_' + str(n+1)
            b = Block(self.parent, self.sceneSubs, name,
                     0, 1, False, False, 'IO',
                     'IOBlk', 'IO for subsystem', BWmin, False)
            b.setPos(el.scenePos())

            pIO = [p for p in b.childItems() if isinstance(p, OutPort)][0]
            pSub = subsIn[n]

            # Set all the connections related to this port
            c2sub = [c for c in el.connections if c.port2.parent in self.sceneSubs.items()]

            for c in c2sub:
                inPort2 = c.port2
                c.remove()

                cnew = self.newConn(el, pSub, self.scene)
                el.connections.append(cnew)
                el.connections = list(set(el.connections))
                pSub.connections.append(cnew)
                pSub.connections = list(set(pSub.connections))

                cnew = self.newConn(pIO, inPort2, self.sceneSubs)
                pIO.connections.append(cnew)
                pIO.connections = list(set(pIO.connections))
                inPort2.connections.append(cnew)
                inPort2.connections = list(set(inPort2.connections))

        # Output ports
        for el in self.outpP:
            n = self.outpP.index(el)
            name = 'out_' + str(n+1)
            b = Block(self.parent, self.sceneSubs, name,
                     1, 0, False, False, 'IO',
                     'IOBlk', 'IO for subsystem', BWmin, False)
            b.setPos(el.connections[0].port2.scenePos())

            pIO = [p for p in b.childItems() if isinstance(p, InPort)][0]
            pSub = subsOut[n]

            # Set all the connections related to this port
            c2out = [c for c in el.connections if c.port2.parent in self.scene.items()]

            for c in c2out:
                outPort2 = c.port2
                c.remove()

                cnew = self.newConn(pSub, outPort2, self.scene)
                pSub.connections.append(cnew)
                pSub.connections = list(set(pSub.connections))
                outPort2.connections.append(cnew)
                outPort2.connections = list(set(outPort2.connections))

                cnew = self.newConn(el, pIO, self.sceneSubs)
                el.connections.append(cnew)
                el.connections = list(set(el.connections))
                pIO.connections.append(cnew)
                pIO.connections = list(set(pIO.connections))

    def openSubsystem(self):
        QMessageBox.warning(self.scene.mainw,'Open the subsystem',
        'If you do some modification in the subsystem,\nyou should save them and reload it\n \
        to integrate the modifications!')
        name = '/tmp/' + self.name + '.dgm'
        self.sceneSubs.saveDgm(name)
        self.sceneSubs.mainw.fopen(name)

    def loadSubsystem(self):
        name = '/tmp/' + self.name + '.dgm'
        self.sceneSubs.loadDgm(name)

    def save(self):
        subs = {}
        pos = (self.pos().x(), self.pos().y())
        vals = [self.name, self.inp, self.outp, self.insetble, self.outsetble,
                self.icon, self.params, self.helpTxt, self.width, self.flip, pos]
        keys = ['name', 'inp', 'outp', 'inset', 'outset', 'icon', 'params',
        'help', 'width', 'flip', 'pos']
        subs['block'] = dict(zip(keys, vals))
        subitems = {}
        self.sceneSubs.saveItems(subitems)
        subs['subitems'] = subitems
        return subs

    def load(self, subs):
        self.sceneSubs.DictToDgm(subs['subitems'])

    def getInternalBlocks(self):
        items = []
        for item in self.sceneSubs.items():
            if isinstance(item, subsBlock):
                blk = item.getInternalBlocks()
                for el in blk:
                    items.append(el)
            elif isinstance(item, Block):
                blk = item.cloneBlkWithPorts()
                blk.subsParent = self
                items.append(blk)
            else:
                pass

        return items

    def gridPos(self, pt):
         gr = GRID
         x = gr * ((pt.x() + gr /2) // gr)
         y = gr * ((pt.y() + gr /2) // gr)
         return QPointF(x,y)


