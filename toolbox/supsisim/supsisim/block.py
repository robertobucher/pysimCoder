from supsisim.qtvers import *

from supsisim.port import Port, InPort, OutPort
from supsisim.connection import Connection
from supsisim.const import GRID, PW, LW, BWmin, BHmin, PD, respath

import os

class Block(QGraphicsPathItem):
    """A block holds ports that can be connected to."""
    def __init__(self, *args):
        self.scene = args[1]
        parent = args[0]
        super(Block, self).__init__(parent)
        self.scene.addItem(self)
        self.syspath = ''
        self.ident = -1

        self.roundedBlocks = True
        
        if len(args) == 12:
            parent, self.scene, self.name, self.inp, self.outp, self.insetble, self.outsetble, self.icon, self.params, self.helpTxt, self.dim, self.flip = args
        elif len(args) == 3:
            parent, self.scene, strBlk = args
            ln = strBlk.split('@')
            self.name = str(ln[0])
            self.inp = int(ln[1])
            self.outp = int(ln[2])
            self.icon = ln[5]
            self.params = ln[6]
            self.helpTxt = ln[7]
            self.dim = int(ln[8])
            self.flip = False
            stbin = int(ln[3])
            stbout = int(ln[4])
            self.insetble = (stbin==1)
            self.outsetble = (stbout==1)
        else:
            raise ValueError('Needs 12 or 3 arguments; received %i.' % len(args))

        self.line_color = Qt.GlobalColor.black
        self.fill_color = Qt.GlobalColor.black
        
        tl = type(self.dim) is list
        if not tl:
            w = self.dim
            self.dim = [w, BHmin]
        
        self.setup()
        try:
            self.scene.blocks.add(self)
        except:
            pass
            
    def __str__(self):
        txt  = 'Name         :' + self.name.__str__() +'\n'
        txt += 'Input ports  :' + self.inp.__str__() + '\n'
        txt += 'Output ports :' + self.outp.__str__() + '\n'
        txt += 'Icon         :' + self.icon.__str__() + '\n'
        txt += 'Params       :' + self.params.__str__() + '\n'
        return txt
        
    def setup(self):
        Nports = max(self.inp, self.outp)
        self.w = self.dim[0]
        self.h = max(BHmin+PD*(max(Nports-1,0)), self.dim[1])

        p = QPainterPath()
        self.setLabel(p)
        self.setPath(p)

        self.port_in = []
        for n in range(0,self.inp):
            self.add_inPort(n)
        for n in range(0,self.outp):
            self.add_outPort(n)

        self.setFlag(QGraphicsItem.GraphicsItemFlag.ItemIsMovable)
        self.setFlag(QGraphicsItem.GraphicsItemFlag.ItemIsSelectable)
        
        self.setFlip()
        
    def add_inPort(self, n):
        pos = -PD*(self.inp-1)/2
        port = InPort(self, self.scene)
        port.block = self
        xpos = -(self.w)/2
        port.setPos(xpos, pos+n*PD)
        return port

    def add_outPort(self, n):
        pos = -PD*(self.outp-1)/2
        port = OutPort(self, self.scene)
        port.block = self
        xpos = (self.w)/2
        port.setPos(xpos, pos+n*PD)
        return port

    def ports(self):
        ports = []
        for thing in self.childItems():
            if isinstance(thing, Port):
                ports.append(thing)
        return ports

    def paint(self, painter, option, widget):
        pen = QPen()
        pen.setBrush(self.line_color)
        pen.setWidth = LW
        if self.isSelected():
            pen.setStyle(Qt.PenStyle.DotLine)
        
        painter.setPen(pen)

        if self.roundedBlocks:
            painter.drawRoundedRect(self.boundingRect(), 10, 10)
        else:
            painter.drawPath(self.path())
            
        svg_size = self.renderer.defaultSize()

        # the middle of the boundingRect is actually (0,0)
        # so shift only by the svg's dim and height
        new_left: float = -svg_size.width()/2
        new_top: float = -svg_size.height()/2
        where_to: QRectF = QRectF(new_left, new_top, svg_size.width(), svg_size.height())
        self.renderer.render(painter, where_to)

    def itemChange(self, change, value):
        return value

    def remove(self):
        self.scene.blocks.remove(self)
        for thing in self.childItems():
            try:
                thing.remove()
            except:
                pass
        self.scene.removeItem(self)

    def setPos(self, *args):
        if len(args) == 1:
            pt = self.gridPos(args[0])
            super(Block, self).setPos(pt)
        else:
            pt = QPointF(args[0],args[1])
            pt = self.gridPos(pt)
            super(Block, self).setPos(pt)
        
    def clone(self, pt):
        b = Block(None, self.scene, self.name, self.inp, self.outp, 
                      self.insetble, self.outsetble, self.icon, self.params, self.helpTxt, self.dim, self.flip)
        b.setPos(self.scenePos().__add__(pt))

    def setFlip(self, flip=None):
        if flip: 
            self.flip = flip
            
        str_path = respath + 'blocks/Icons/' + self.icon + '.svg'
        if self.flip:
            self.setTransform(QTransform.fromScale(-1, 1))
            # the path
            mirr_path = '/tmp/' + self.icon + '.svg'
            if not os.path.exists(mirr_path):
                cmd = 'inkscape --actions="select-all;object-flip-horizontal" -o ' + mirr_path + ' ' + str_path
                os.system(cmd)
            self.renderer = QtSvg.QSvgRenderer(mirr_path)
        else:
            self.setTransform(QTransform.fromScale(1, 1))
            self.renderer = QtSvg.QSvgRenderer(str_path)
        self.flipLabel()

    def setLabel(self, p):
        labels = set()
        try:
            for b in self.scene.blocks:
                if b != self:
                    labels.add(b.label.toPlainText())
            name = self.name
            if name in labels:
                cnt = 0
                while name and name[-1] in '0123456789':
                    name = name[:-1]
                base = name
                while name in labels:
                    name = base + str(cnt)
                    cnt += 1
            self.name = name
            self.label = QGraphicsTextItem(self)
            self.label.setPlainText(name)
            
        except:
            self.label = QGraphicsTextItem(self)
            self.label.setPlainText(self.name)           
        
        p.addRect(-self.w/2, -self.h/2, self.w, self.h)
        w = self.label.boundingRect().width()
        self.label.setPos(-w/2, self.h/2+5)
 
    def flipLabel(self):
        w = self.label.boundingRect().width()
        if self.flip:
            self.label.setTransform(QTransform.fromScale(-1,1).translate(-w,0))
        else:
            self.label.setTransform(QTransform.fromScale(1,1))          
    
    def save(self):
        pos = (self.pos().x(), self.pos().y())
        vals = [self.name, self.inp, self.outp, self.insetble, self.outsetble, 
                self.icon, self.params, self.helpTxt, self.dim, self.flip, pos]
        keys = ['name', 'inp', 'outp', 'inset', 'outset', 'icon', 'params', 'help', 'dims', 'flip', 'pos']
        return dict(zip(keys, vals))

    def getPorts(self):
        InP = []
        OutP = []
        for item in self.childItems():
            if isinstance(item, InPort):
                InP.append(item)
            elif isinstance(item, OutPort):
                OutP.append(item)
            else:
                pass

        InP.sort(key=lambda p: p.pos().y())
        OutP.sort(key=lambda p: p.pos().y())
        return InP, OutP

    def cloneBlkWithPorts(self):
        # create a perfect copy of a block
        b = Block(None, self.scene, self.name, self.inp, self.outp,
                      self.insetble, self.outsetble, self.icon, self.params,
                      self.helpTxt, self.dim, self.flip)
        b.name = self.name

        inp1, outp1 = self.getPorts()
        inp2, outp2 = b.getPorts()

        for n in range(0, len(inp1)):
            for c in inp1[n].connections:
                newConn = Connection(None, self.scene)
                newConn.port1 = c.port1
                newConn.pos1 = c.pos1
                newConn.port2 = inp2[n]
                newConn.pos2 = c.pos2
                for pt in c.connPoints:
                    newConn.connPoints.append(pt)
                inp2[n].connections.append(newConn)
                if c in c.port1.connections:
                    c.port1.connections.remove(c)
                    c.port1.connections.append(newConn)                    
                self.scene.removeItem(newConn)

        for n in range(0, len(outp1)):
            for c in outp1[n].connections:
                newConn = Connection(None, self.scene)
                newConn.port1 = outp2[n]
                newConn.pos1 = c.pos2
                newConn.port2 = c.port2
                newConn.pos2 = c.pos2
                outp2[n].connections.append(newConn)
                if c in c.port2.connections:
                    c.port2.connections.remove(c)
                    c.port2.connections.append(newConn)                    
                self.scene.removeItem(newConn)

        b.scene.removeItem(b)
        return b

    def gridPos(self, pt):
         gr = GRID
         x = gr * ((pt.x() + gr /2) // gr)
         y = gr * ((pt.y() + gr /2) // gr)
         return QPointF(x,y)

    def setSysPath(self,basepath):
        self.syspath = f"{basepath}/{self.name}"

    def getCodeName(self):
        return self.name + '_' + str(self.ident)
