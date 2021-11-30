from PyQt5.QtWidgets import QGraphicsPathItem, QGraphicsTextItem
from PyQt5.QtGui import QPainterPath, QPen, QImage, QTransform
from PyQt5.QtCore import Qt, QPointF

from supsisim.port import Port, InPort, OutPort
from supsisim.const import GRID, PW, LW, BWmin, BHmin, PD, respath

from lxml import etree

class Block(QGraphicsPathItem):
    """A block holds ports that can be connected to."""
    def __init__(self, *args):
        self.scene = args[1]
        parent = args[0]
        super(Block, self).__init__(parent)
        self.scene.addItem(self)
 
        if len(args) == 12:
            parent, self.scene, self.name, self.inp, self.outp, self.insetble, self.outsetble, self.icon, self.params, self.helpTxt, self.width, self.flip = args
        elif len(args) == 3:
            parent, self.scene, strBlk = args
            ln = strBlk.split('@')
            self.name = str(ln[0])
            self.inp = int(ln[1])
            self.outp = int(ln[2])
            self.icon = ln[5]
            self.params = ln[6]
            self.helpTxt = ln[7]
            self.width = int(ln[8])
            self.flip = False
            stbin = int(ln[3])
            stbout = int(ln[4])
            self.insetble = (stbin==1)
            self.outsetble = (stbout==1)
        else:
            raise ValueError('Needs 12 or 3 arguments; received %i.' % len(args))

        self.line_color = Qt.black
        self.fill_color = Qt.black
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
        txt += 'Help            :' + self.helpTxt.__str__() + '\n'        
        return txt
        
    def setup(self):
        Nports = max(self.inp, self.outp)
        self.w = self.width
        self.h = BHmin+PD*(max(Nports-1,0))

        p = QPainterPath()
        self.setLabel(p)
        self.setPath(p)

        self.port_in = []
        for n in range(0,self.inp):
            self.add_inPort(n)
        for n in range(0,self.outp):
            self.add_outPort(n)

        self.setFlag(self.ItemIsMovable)
        self.setFlag(self.ItemIsSelectable)
        
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
            pen.setStyle(Qt.DotLine)
        painter.setPen(pen)
        painter.drawPath(self.path())
        img = QImage(respath + 'blocks/Icons/' + self.icon + '.svg')
        if self.flip:
            img = img.mirrored(True, False)
        rect = img.rect()
        painter.drawImage(-rect.width()/2,-rect.height()/2,img)

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
        
    def gridPos(self, pt):
         gr = GRID
         x = gr * ((pt.x() + gr /2) // gr)
         y = gr * ((pt.y() + gr /2) // gr)
         return QPointF(x,y)

    def clone(self, pt):
        b = Block(None, self.scene, self.name, self.inp, self.outp,
                      self.insetble, self.outsetble, self.icon, self.params, self.helpTxt, self.width, self.flip)
        b.setPos(self.scenePos().__add__(pt))

    def setFlip(self, flip=None):
        if flip: 
            self.flip = flip
        if self.flip:
            self.setTransform(QTransform.fromScale(-1, 1))
        else:
            self.setTransform(QTransform.fromScale(1, 1))
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
    
    def save(self, root):
        blk = etree.SubElement(root,'block')
        etree.SubElement(blk,'name').text = self.name
        etree.SubElement(blk,'inp').text = self.inp.__str__()
        etree.SubElement(blk,'outp').text = self.outp.__str__()
        
        if self.insetble:
            etree.SubElement(blk,'inset').text = '1'
        else:
            etree.SubElement(blk,'inset').text = '0'
        if self.outsetble:
            etree.SubElement(blk,'outset').text = '1'
        else:
            etree.SubElement(blk,'outset').text = '0'

            
        etree.SubElement(blk,'icon').text = self.icon
        etree.SubElement(blk,'params').text = self.params
        etree.SubElement(blk,'help').text = self.helpTxt
        etree.SubElement(blk,'width').text = self.width.__str__()
        if self.flip:
            etree.SubElement(blk,'flip').text = '1'
        else:
            etree.SubElement(blk,'flip').text = '0'
        etree.SubElement(blk,'posX').text = self.pos().x().__str__()
        etree.SubElement(blk,'posY').text = self.pos().y().__str__()
