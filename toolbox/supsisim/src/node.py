from pyqt45 import QGraphicsPathItem, QPainterPath, QPen, QtCore

from supsisim.port import InNodePort, OutNodePort
from supsisim.const import GRID, NW

from lxml import etree

class Node(QGraphicsPathItem):
    """A block holds ports that can be connected to."""
    def __init__(self, parent, scene):
        super(Node, self).__init__(parent, scene)
        self.scene = scene
        self.line_color = QtCore.Qt.black
        self.fill_color = QtCore.Qt.black
        self.setup()

    def __str__(self):
        txt = 'Node\n'
        for thing in self.childItems():
            print(thing)
        return txt
        
    def setup(self):
        p = QPainterPath()
        #p.addRect(-NW/2, -NW/2, NW, NW)
        p.addEllipse(-NW/2, -NW/2, NW, NW)
        self.setPath(p)

        self.add_inPort()
        self.add_outPort()
        self.setFlag(self.ItemIsMovable)
        self.setFlag(self.ItemIsSelectable)
        
    def add_inPort(self):
        port = InNodePort(self, self.scene)
        self.port_in = port
        return port

    def add_outPort(self):
        port = OutNodePort(self, self.scene)
        self.port_out = port
        return port

    def ports(self):
        return self.port

    def paint(self, painter, option, widget):
        painter.setPen(QPen(self.line_color))
        if self.isSelected():
            painter.setPen(QPen(QtCore.Qt.red))
            painter.setBrush(QtCore.Qt.red)
        else:
            painter.setPen(QPen(self.line_color))
        painter.setBrush(self.fill_color)
        painter.drawPath(self.path())

    def remove(self):
      for thing in self.childItems():
        try:
          thing.remove()
        except:
          pass
      self.scene.removeItem(self)

    def setPos(self, *args):
        if len(args) == 1:
            pt = self.gridPos(args[0])
            super(Node, self).setPos(pt)
        else:
            pt = QtCore.QPointF(args[0],args[1])
            pt = self.gridPos(pt)
            super(Node, self).setPos(pt)
        
    def gridPos(self, pt):
         gr = GRID
         x = gr * ((pt.x() + gr /2) // gr)
         y = gr * ((pt.y() + gr /2) // gr)
         return QtCore.QPointF(x,y)
       
    def clone(self, pt):
        n = Node(None, self.scene)
        n.setPos(self.scenePos().__add__(pt))
       
    def save(self, root):
        node = etree.SubElement(root,'node')
        etree.SubElement(node,'posX').text = self.pos().x().__str__()
        etree.SubElement(node,'posY').text = self.pos().y().__str__()


        

