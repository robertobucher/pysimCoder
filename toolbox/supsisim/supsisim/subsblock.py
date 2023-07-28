from supsisim.qtvers import *

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

            blksList = args[2]
            self.blksList = [el for el in blksList if el.params != 'IOBlk']

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

        # First find not connected ports

        for item in self.blksList:
            outPorts = [p for p in item.childItems() if isinstance(p, OutPort)]
            for p in outPorts:
                if not p.connections:
                    pPos = p.scenePos().y()
                    pDict = {'port' : p, 'pos'  : pPos, 'orphan' : True}
                    outpP.append(pDict)

            inPorts = [p for p in item.childItems() if isinstance(p, InPort)]
            for p in inPorts:
                if not p.connections:
                    pPos = p.scenePos().y()
                    pDict = {'port' : p, 'pos'  : pPos, 'orphan' : True}
                    inpP.append(pDict)

        # Find connections outside of the superblock

        # Search for blocks outside of superblock, connected to block in superblock
        items = [item for item in self.scene.items() \
        if isinstance(item, Block) and item not in self.blksList]

        for item in items:
            outPorts = [p for p in item.childItems() if isinstance(p, OutPort)]
            cin = []
            for p in outPorts:
                cin = [c for c in p.connections if c.port2.parent in self.blksList]
                try:
                    cin = list(set(cin))
                    if len(cin) != 0:
                        pPos = min([c.port2.scenePos().y() for c in cin])
                        pDict = {'port' : p, 'pos'  : pPos, 'orphan' : False}
                        inpP.append(pDict)
                except:
                    pass

        # Search for blocks in superblock, connected to block outside
        for item in self.blksList:
            outPorts = [p for p in item.childItems() if isinstance(p, OutPort)]
            cout = []
            for p in outPorts:
                cout = [c for c in p.connections if c.port2.parent not in self.blksList]
                try:
                    cout = list(set(cout))
                    if len(cout) != 0:
                        pPos = min(c.port2.scenePos().y() for c in cout)
                        pDict = {'port' : p, 'pos'  : pPos, 'orphan' : False}
                        outpP.append(pDict)
                except:
                    pass

        inpP.sort(key=lambda p: p.get('pos'))
        self.inpP = []
        for item in inpP:
            self.inpP.append(item)
        self.outpP = []
        outpP.sort(key=lambda p: p.get('pos'))
        for item in outpP:
            self.outpP.append(item)

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
        if c.pos1.x() > c.pos2.x():
            p1 = self.gridPos(QPointF(c.pos1.x()+40, c.pos1.y()))
            p2 = self.gridPos(QPointF(c.pos1.x()+40, c.pos1.y()+200))
            p3 = self.gridPos(QPointF(c.pos2.x()-40, c.pos1.y()+200))
            p4 = self.gridPos(QPointF(c.pos2.x()-40, c.pos2.y()))
            c.connPoints.append(p1)
            c.connPoints.append(p2)
            c.connPoints.append(p3)
            c.connPoints.append(p4)
        else:
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
        for item in self.inpP:
            el = item['port']
            n = self.inpP.index(item)
            name = 'in_' + str(n+1)
            b = Block(self.parent, self.sceneSubs, name,
                     0, 1, False, False, 'IO',
                     'IOBlk', 'IO for subsystem', BWmin, False)
            pPos = el.scenePos()-QPointF(100.0,0.0)
            b.setPos(pPos)

            if item['orphan']:
                pIO = [p for p in b.childItems() if isinstance(p, OutPort)][0]
                cnew = self.newConn(pIO, el, self.sceneSubs)
                pIO.connections.append(cnew)
                el.connections.append(cnew)
            else:
                pIO = [p for p in b.childItems() if isinstance(p, OutPort)][0]
                pSub = subsIn[n]

                # Set all the connections related to this port
                c2sub = [c for c in el.connections if c.port2.parent in self.sceneSubs.items()]

                # Connection to subsystem
                cnew = self.newConn(el, pSub, self.scene)
                el.connections.append(cnew)
                el.connections = list(set(el.connections))
                pSub.connections.append(cnew)
                pSub.connections = list(set(pSub.connections))

                # Connections in subsystem
                for c in c2sub:
                    inPort2 = c.port2
                    cnew = self.newConn(pIO, inPort2, self.sceneSubs)
                    pIO.connections.append(cnew)
                    pIO.connections = list(set(pIO.connections))
                    inPort2.connections.append(cnew)
                    inPort2.connections = list(set(inPort2.connections))

                    c.remove()

        # Output ports
        for item in self.outpP:
            el = item['port']
            n = self.outpP.index(item)
            name = 'out_' + str(n+1)
            b = Block(self.parent, self.sceneSubs, name,
                     1, 0, False, False, 'IO',
                     'IOBlk', 'IO for subsystem', BWmin, False)
            pPos = el.scenePos()+QPointF(100.0,0.0)
            b.setPos(pPos)

            if item['orphan']:
                pIO = [p for p in b.childItems() if isinstance(p, InPort)][0]
                cnew = self.newConn(el, pIO, self.sceneSubs)
                pIO.connections.append(cnew)
                el.connections.append(cnew)
            else:
                pIO = [p for p in b.childItems() if isinstance(p, InPort)][0]
                pSub = subsOut[n]

                # Set all the connections related to this port
                c2out = [c for c in el.connections if c.port2.parent in self.scene.items()]

                # Connection in subsystem
                cnew = self.newConn(el, pIO, self.sceneSubs)
                el.connections.append(cnew)
                el.connections = list(set(el.connections))
                pIO.connections.append(cnew)
                pIO.connections = list(set(pIO.connections))

                # Connection from subsystem
                for c in c2out:
                    outPort2 = c.port2
                    cnew = self.newConn(pSub, outPort2, self.scene)
                    pSub.connections.append(cnew)
                    pSub.connections = list(set(pSub.connections))
                    outPort2.connections.append(cnew)
                    outPort2.connections = list(set(outPort2.connections))

                    c.remove()

    def openSubsystem(self):
        self.sceneSubs.mainw.openSubs(self.name, self.sceneSubs)

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
                item.subsParent = self
                items.append(item)
            else:
                pass

        return items

    def gridPos(self, pt):
         gr = GRID
         x = gr * ((pt.x() + gr /2) // gr)
         y = gr * ((pt.y() + gr /2) // gr)
         return QPointF(x,y)


