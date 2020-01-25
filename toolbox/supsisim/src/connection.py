from PyQt5.QtWidgets import QGraphicsPathItem
from PyQt5.QtGui import QPainterPath, QPen
from PyQt5.QtCore import Qt, QPointF

import numpy as np
from supsisim.const import LW, DB, GRID
from supsisim.port import InPort, OutPort
from lxml import etree

class Connection(QGraphicsPathItem):
    """Connects one port to another."""

    def __init__(self, parent, scene):
        super(Connection, self).__init__(None)
        self.scene = scene
        self.scene.addItem(self)

        self.pos1 = None
        self.pos2 = None

        self.port1 = None
        self.port2 = None

        self.connPoints = []
        self.draw_color = Qt.black
        
        self.setup()

    def __str__(self):
        txt  = 'Connection\n'
        txt += 'Position 1 : ' + self.pos1.__str__() + '\n'
        txt += 'Position 2 : ' + self.pos2.__str__() + '\n'
        txt += 'Port1 :\n'
        txt += self.port1.parent.name + '\n'
        txt += 'Port2 :\n'
        txt += self.port2.parent.name + '\n'
        for el in self.connPoints:
            txt += el.__str__() + '\n'
        return txt
        
    def setup(self):
        pen = QPen(self.draw_color)
        pen.setWidth(LW)
        self.setPen(pen)
        #self.setFlag(self.ItemIsSelectable)

    def addPoint(self, pos):
        if len(self.connPoints) == 0:
            y = self.pos1.y()
            pt =QPointF(pos.x(),y)
        else:
            pt_prev = self.connPoints[-1]
            dx = np.abs(pos.x()-pt_prev.x())
            dy = np.abs(pos.y()-pt_prev.y())
            if dx > dy:
                pt = QPointF(pos.x(),pt_prev.y())
            else:
                pt = QPointF(pt_prev.x(),pos.y())
        self.connPoints.append(pt)

    def clean(self):
        N = len(self.connPoints)
        if N> 2:
            remPt = []
            for n in range(1, N-2):
                if self.connPoints[n-1] == self.connPoints[n]:
                    remPt.append(self.connPoints[n])
            for el in remPt:
                self.connPoints.remove(el)

    def move(self, npos, destPos):
        N = len(self.connPoints)
        initIndex = npos -1
        pt =  self.connPoints[initIndex]
        actPosX = pt.x()
        actPosY = pt.y()
        
        if self.connPoints[initIndex].x() == self.connPoints[initIndex+1].x():
            # Vertical segment
            try:
                while initIndex<=N:
                    if actPosX - DB < self.connPoints[initIndex].x() < actPosX+DB:
                        self.connPoints[initIndex].setX(destPos.x())
                        initIndex  += 1
                    else:
                        break
            except:
                pass
            initIndex = npos -2
            try:
                while initIndex>=0:
                    if actPosX - DB < self.connPoints[initIndex].x() < actPosX+DB:
                        self.connPoints[initIndex].setX(destPos.x())
                        initIndex -= 1
                    else:
                        break
            except:
                pass
                    
            self.update_path()
            self.update_pos_from_ports()
            return True
            
        elif self.connPoints[initIndex].y() == self.connPoints[initIndex+1].y():
            # Horizontal segment
            try:
                while initIndex<N:
                    if actPosY - DB < self.connPoints[initIndex].y() < actPosY+DB:
                        self.connPoints[initIndex].setY(destPos.y())
                        initIndex += 1
                    else:
                        break
            except:
                pass
            initIndex = npos
            try:
                while initIndex>0:
                    if actPos.contains(self.connPoints):
                        self.connPoints[initIndex].setY(destPos.y())
                        initIndex -= 1
                    else:
                        break
            except:
                pass
            
            self.update_path()
            self.update_pos_from_ports()
            return True
        
        else:          
            return False

    def update_pos_from_ports(self):
        pos = self.port1.scenePos()
        delta = pos - self.pos1
        self.pos1 = pos
        N = len(self.connPoints)
        for n in range(0,N):
            self.connPoints[n] = self.connPoints[n]+delta
        self.pos2 = self.port2.scenePos()
        self.connPoints[0].setY(self.pos1.y())
        self.connPoints[-1].setY(self.pos2.y())
        self.update_path()

    def update_ports_from_pos(self):
        item = self.scene.find_itemAt(self.pos1)
        if isinstance(item, OutPort):
            self.port1 = item
        item = self.scene.find_itemAt(self.pos2)
        if isinstance(item, InPort):
            self.port2 = item
        try:
            self.port1.connections.append(self)
        except:
            pass
        try:
            self.port2.connections.append(self)
        except:
            pass        
        self.update_path()

    def redrawConnection(self):
        N = len(self.connPoints)
        if N != 0:
            for n in range(0,N-1):
                if (n % 2) == 0:
                    self.connPoints[n].setX(self.connPoints[n+1].x())
                else:
                    self.connPoints[n].setY(self.connPoints[n+1].y())
        self.connPoints[N-1].setY(self.pos2.y())             
        self.update_path()      
        
    def update_path(self):
        p = QPainterPath()
        p.moveTo(self.pos1)
        for el in self.connPoints:
            p.lineTo(el)
        p.lineTo(self.pos2)
        self.setPath(p)

    def update_path_draw2Port(self):
        p = QPainterPath()
        p.moveTo(self.pos1)
        for el in self.connPoints:
            p.lineTo(el)
        if len(self.connPoints) == 0:
            pt_prev = self.pos1
            pt1 = QPointF((self.pos2.x()+pt_prev.x())/2, pt_prev.y())
            pt2 = QPointF((self.pos2.x()+pt_prev.x())/2, self.pos2.y())
            p.lineTo(pt1)
            p.lineTo(pt2)
        else:
            pt_prev = self.connPoints[-1]
            pt = QPointF(pt_prev.x(), self.pos2.y())
            if len(self.connPoints)>=2:
                pt_2prev = self.connPoints[-2]
                if pt_2prev.x() == pt_prev.x() == pt.x() or \
                   pt_2prev.y() == pt_prev.y() == pt.y():
                    self.connPoints.remove(pt_prev)
            
            p.lineTo(pt)
        p.lineTo(self.pos2)
        self.setPath(p)

    def update_path_draw2Pt(self):
        p = QPainterPath()
        p.moveTo(self.pos1)
        for el in self.connPoints:
            p.lineTo(el)
        if len(self.connPoints) == 0:
            y = self.pos1.y()
            pt =QPointF(self.pos2.x(),y)
        else:
            pt_prev = self.connPoints[-1]
            dx = np.abs(self.pos2.x()-pt_prev.x())
            dy = np.abs(self.pos2.y()-pt_prev.y())
            if dx > dy:
                pt = QPointF(self.pos2.x(),pt_prev.y())
            else:
                pt = QPointF(pt_prev.x(),self.pos2.y())
            if len(self.connPoints)==1:
                pt_2prev = self.pos1;
            else:
                pt_2prev = self.connPoints[-2]
            if pt.x() == pt_prev.x() == pt_2prev.x() and \
               (pt_prev.y() < pt.y() < pt_2prev.y() or pt_2prev.y() < pt.y() < pt_prev.y()):
                self.connPoints.remove(pt_prev)
                        
            elif pt.y() == pt_prev.y() == pt_2prev.y() and \
                 (pt_prev.x() < pt.x() < pt_2prev.x() or pt_2prev.x() < pt.x() < pt_prev.x()):
                self.connPoints.remove(pt_prev)
            
        p.lineTo(pt)
        p.lineTo(self.pos2)
        self.setPath(p)

    def paint(self, painter, option, widget):
        pen = QPen(self.draw_color)
        pen.setWidth(LW)
        if self.isSelected():
            pen.setStyle(Qt.DotLine)
        painter.setPen(pen)
        painter.drawPath(self.path())

    def setPos(self, *args):
        if len(args) == 1:
            pt = self.gridPos(args[0])
            super(Node, self).setPos(pt)
        else:
            pt = QPointF(args[0],args[1])
            pt = self.gridPos(pt)
            super(Node, self).setPos(pt)
            
    def remove(self):
        try:
            self.port1.connections.remove(self)
        except:
            pass
        try:
            self.port2.connections.remove(self)
        except:
            pass
        self.scene.removeItem(self)

    def save(self,root):
        try:
            conn = etree.SubElement(root,'connection')
            etree.SubElement(conn,'pos1X').text = self.pos1.x().__str__()
            etree.SubElement(conn,'pos1Y').text = self.pos1.y().__str__()
            etree.SubElement(conn,'pos2X').text = self.pos2.x().__str__()
            etree.SubElement(conn,'pos2Y').text = self.pos2.y().__str__()
            for el in self.connPoints:
                etree.SubElement(conn, 'pt').text = el.x().__str__() + ',' + el.y().__str__()
        except:
            pass

    def load(self, item, dx = 0.0, dy = 0.0):
        try:
            pt1 = QPointF(float(item.findtext('pos1X')), float(item.findtext('pos1Y')))
            pt2 = QPointF(float(item.findtext('pos2X')), float(item.findtext('pos2Y')))
            dpt = QPointF(dx, dy)
            self.pos1 = self.gridPos(pt1+dpt)
            self.pos2 = self.gridPos(pt2+dpt)
            points = item.findall('pt')
            for el in points:
                pt = el.text.split(',')
                x = float(pt[0])
                y = float(pt[1])
                pt = self.gridPos(QPointF(float(pt[0]), float(pt[1]))+dpt)
                self.connPoints.append(pt)
            self.update_ports_from_pos()
        except:
            pass
            
    def gridPos(self, pt):
         gr = GRID
         x = gr * ((pt.x() + gr /2) // gr)
         y = gr * ((pt.y() + gr /2) // gr)
         return QPointF(x,y)


