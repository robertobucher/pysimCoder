from PyQt5.QtWidgets import QGraphicsPathItem
from PyQt5.QtGui import QPainterPath, QPen
from PyQt5.QtCore import Qt, QPointF

from supsisim.const import GRID, NW

class Node(QGraphicsPathItem):
    """A block holds ports that can be connected to."""
    def __init__(self, parent, scene):
        super(Node, self).__init__(parent)
        self.scene = scene
        self.scene.addItem(self)
        self.line_color = Qt.black
        self.fill_color = Qt.black
        self.setup()

    def __str__(self):
        txt = 'Node\n'
        return txt
        
    def setup(self):
        p = QPainterPath()
        p.addEllipse(-NW/2, -NW/2, NW, NW)
        self.setPath(p)
        #self.setFlag(self.ItemIsMovable)
        #self.setFlag(self.ItemIsSelectable)

    def paint(self, painter, option, widget):
        painter.setPen(QPen(self.line_color))
        if self.isSelected():
            painter.setPen(QPen(Qt.red))
            painter.setBrush(Qt.red)
        else:
            painter.setPen(QPen(self.line_color))
        painter.setBrush(self.fill_color)
        painter.drawPath(self.path())

    def remove(self):
        self.scene.removeItem(self)

    def setPos(self, *args):
        if len(args) == 1:
            pt = self.gridPos(args[0])
            super(Node, self).setPos(pt)
        else:
            pt = QPointF(args[0],args[1])
            pt = self.gridPos(pt)
            super(Node, self).setPos(pt)
        
    def gridPos(self, pt):
         gr = GRID
         x = gr * ((pt.x() + gr /2) // gr)
         y = gr * ((pt.y() + gr /2) // gr)
         return QPointF(x,y)
     

        

