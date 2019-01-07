from pyqt45 import QGraphicsPathItem, QPainterPath, QPen, QtCore

import numpy as np
from supsisim.const import LW
from supsisim.port import InPort, OutPort
from supsisim.node import Node
from lxml import etree

class Connection(QGraphicsPathItem):
    """Connects one port to another."""

    def __init__(self, parent, scene):
        super(Connection, self).__init__(parent, scene)
        self.scene = scene

        self.pos1 = None
        self.pos2 = None

        self.port1 = None
        self.port2 = None

        self.line_color = QtCore.Qt.black

        self.setup()

    def __str__(self):
        txt  = 'Connection\n'
        txt += 'Position 1 : ' + self.pos1.__str__() + '\n'
        txt += 'Position 2 : ' + self.pos2.__str__() + '\n'
        txt += 'Port1 :\n'
        txt += self.port1.__str__() + '\n'
        txt += 'Port2 :\n'
        txt += self.port2.__str__() + '\n'
        return txt
        
    def setup(self):
        pen = QPen(self.line_color)
        pen.setWidth(LW)
        self.setPen(pen)

    def update_pos_from_ports(self):
        self.pos1 = self.port1.scenePos()
        self.pos2 = self.port2.scenePos()

    def update_ports_from_pos(self):
        item = self.scene.find_itemAt(self.pos1)
        if isinstance(item, OutPort):
            self.port1 = item
        elif isinstance(item, Node):
            self.port1 = item.port_out
        item = self.scene.find_itemAt(self.pos2)
        if isinstance(item, InPort):
            self.port2 = item
        elif isinstance(item, Node):
            self.port2 = item.port_in
        # Bug TU Berlin - Berghoff (is now a double check?)
        if self.port1 is not None:
            if self.port1.connections is not None:
                self.port1.connections.append(self)
        if self.port2 is not None:
            if self.port2.connections is not None:
                self.port2.connections.append(self)
        #self.port1.connections.append(self)
        #self.port2.connections.append(self)
        self.update_path()
        
    def update_path(self):
        p = QPainterPath()
        item = None
        if self.port2 == None:
            item = self.scene.find_itemAt(self.pos2)
        if isinstance(self.port1, OutPort):
            pt = QtCore.QPointF(self.pos2.x(),self.pos1.y())
        elif isinstance(self.port2, InPort) or isinstance(item, InPort):
            pt = QtCore.QPointF(self.pos1.x(),self.pos2.y())
        else:
            dx = np.abs(self.pos2.x()-self.pos1.x())
            dy = np.abs(self.pos2.y()-self.pos1.y())
            if dx > dy:
                pt = QtCore.QPointF(self.pos2.x(),self.pos1.y())
            else:
                pt = QtCore.QPointF(self.pos1.x(),self.pos2.y())

        p.moveTo(self.pos1)
        p.lineTo(pt)
        p.lineTo(self.pos2)
        self.setPath(p)

    '''
    def paint(self, painter, option, widget):
        pen = QPen()
        pen.setBrush(self.line_color)
        pen.setWidth(LW)
        if self.isSelected():
            pen.setStyle(QtCore.Qt.DotLine)
        painter.setPen(pen)
    '''

    def remove(self):
        try:
            self.port1.connections.remove(self)
        except (AttributeError, ValueError):
            pass
        try:
            self.port2.connections.remove(self)
        except (AttributeError, ValueError):
            pass
        try:
            self.scene.removeItem(self)
        except AttributeError:
            pass

    def clone(self, pt):
        c = Connection(None, self.scene)
        c.pos1 = self.pos1.__add__(pt)
        c.pos2 = self.pos2.__add__(pt)
        return c
    
    def save(self,root):
        conn = etree.SubElement(root,'connection')
        etree.SubElement(conn,'pos1X').text = self.pos1.x().__str__()
        etree.SubElement(conn,'pos1Y').text = self.pos1.y().__str__()
        etree.SubElement(conn,'pos2X').text = self.pos2.x().__str__()
        etree.SubElement(conn,'pos2Y').text = self.pos2.y().__str__()

