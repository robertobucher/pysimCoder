from supsisim.qtvers import *

import numpy as np
from supsisim.const import LW, DB, GRID
from supsisim.port import InPort, OutPort

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
        self.params = None

        self.connPoints = []
        self.draw_color =Qt.GlobalColor.black
        self.setup()
        
        self.selected = False

    def __str__(self):
        txt  = 'Connection\n'
        txt += 'Position 1 : ' + self.pos1.__str__() + '\n'
        txt += 'Position 2 : ' + self.pos2.__str__() + '\n'
        txt += 'Port1 :   '
        try:
            txt += self.port1.parent.name + '\n'
        except:
            txt +='\n'
        txt += 'Port2 :   '
        try:
            txt += self.port2.parent.name + '\n'
        except:
            txt +='\n'
        for el in self.connPoints:
            txt += el.__str__() + '\n'
        return txt
        
    def setup(self):
        pen = QPen(self.draw_color)
        pen.setWidth(LW)
        self.setPen(pen)
        self.setFlag(QGraphicsItem.GraphicsItemFlag.ItemIsSelectable)

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

    def insPoint(self, pos):
        if len(self.connPoints) == 0:
            y = self.pos2.y()
            pt =QPointF(pos.x(),y)
        else:
            pt_prev = self.connPoints[0]
            dx = np.abs(pos.x()-pt_prev.x())
            dy = np.abs(pos.y()-pt_prev.y())
            if dx > dy:
                pt = QPointF(pos.x(),pt_prev.y())
            else:
                pt = QPointF(pt_prev.x(),pos.y())
        self.connPoints.insert(0, pt)

    def clean(self):
        N = len(self.connPoints)
        if N>2:
            connPoints = self.connPoints.copy()
            # Clean identic points
            self.connPoints = []
            [self.connPoints.append(x) for x in connPoints[0:-1] \
             if x not in self.connPoints]
            self.connPoints.append(connPoints[-1])
            self.cleanXY()
            
    def cleanXY(self):
        #  Clean wrong aligned points in x and y
        N = len(self.connPoints)
        if N>2:
            connPoints = self.connPoints.copy()
            for n in range(1,N-1):
                prev_ptx, prev_pty = self.connPoints[n-1].x(), self.connPoints[n-1].y()
                ptx, pty = self.connPoints[n].x(), self.connPoints[n].y()
                next_ptx, next_pty = self.connPoints[n+1].x(), self.connPoints[n+1].y()
                if prev_ptx==ptx==next_ptx:
                    if pty > prev_pty and pty > next_pty or \
                       pty < prev_pty and pty < next_pty:
                          connPoints.remove(self.connPoints[n])
                if prev_pty==pty==next_pty:
                    if ptx > prev_ptx and ptx > next_ptx or \
                       ptx < prev_ptx and pty < next_ptx:
                          connPoints.remove(self.connPoints[n])                          
            self.connPoints = []
            for el in connPoints:
                self.connPoints.append(el)
                                                          
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
#        N = len(self.connPoints)
#        for n in range(0,N):
#            self.connPoints[n] = self.connPoints[n]+delta
        self.pos2 = self.port2.scenePos()
        if len(self.connPoints)!=0:
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

    def update_path(self):
        p = QPainterPath()
        p.moveTo(self.pos1)
        for el in self.connPoints:
            p.lineTo(el)
        p.lineTo(self.pos2)
        self.setPath(p)

    def find_exact_pos(self, pos):
        # Find exact point "pos" on the connection
        points = [self.pos1]
        for el in self.connPoints:
            points.append(el)
        points.append(self.pos2)
        N = len(points)
        for n in range(0,N-1):
            p1 = points[n]
            p2 = points[n+1]
            rect = QRectF(p1 - QPointF(DB, DB) ,p2 + QPointF(DB, DB))
            if rect.contains(pos):
                if p1.x() == p2.x():
                    pos.setX(p1.x())
                if p1.y() == p2.y():
                    pos.setY(p1.y())
                return n, pos

    def get_direction(self, pos):
        try:
            npos, pos = self.find_exact_pos(self.gridPos(pos))
        except:
            return
        N = len(self.connPoints)
        if npos == 0 or npos == (N):
            return 'h'
        else:
            pt1 = self.connPoints[npos-1]
            pt2 = self.connPoints[npos]
            if pt1.x()==pt2.x():
                return 'v'
            else:
                return 'h'

    def update_path_draw2InPort(self):
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

    def update_path_draw2Pt_fw(self):
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

    def update_path_draw2OutPort(self):
        p = QPainterPath()
        p.moveTo(self.pos1)
        if len(self.connPoints) == 0:
            pt1 = QPointF((self.pos1.x()+self.pos2.x())/2, self.pos1.y())
            pt2 = QPointF((self.pos1.x()+self.pos2.x())/2, self.pos2.y())
            p.lineTo(pt1)
            p.lineTo(pt2)
        
        else:
            pt=  self.connPoints[0]
            pt1 = QPointF((self.pos1.x()+pt.x())/2, self.pos1.y())
            pt2 = QPointF((self.pos1.x()+pt.x())/2, pt.y())
            p.lineTo(pt1)
            p.lineTo(pt2)
            
            if len(self.connPoints)>=2:
                pt_2next = self.connPoints[1]
                if pt_2next.x() == pt1.x() == pt2.x() or \
                   pt_2next.y() == pt1.y() == pt2.y():
                    self.connPoints.remove(pt_2next)
        
            for el in self.connPoints:
                p.lineTo(el)
                
        p.lineTo(self.pos2)
        self.setPath(p)        
        
    def update_path_draw2Pt_bk(self):
        p = QPainterPath()
        p.moveTo(self.pos1)
        if len(self.connPoints) == 0:
            y = self.pos2.y()
            pt =QPointF(self.pos1.x(),y)
        else:
            pt_next = self.connPoints[0]
            dx = np.abs(self.pos1.x()-pt_next.x())
            dy = np.abs(self.pos1.y()-pt_next.y())
            if dx > dy:
                pt = QPointF(self.pos1.x(),pt_next.y())
            else:
                pt = QPointF(pt_next.x(),self.pos1.y())
            if len(self.connPoints)==1:
                pt_1next = self.pos1;
            else:
                pt_1next = self.connPoints[1]
            if pt.x() == pt_next.x() == pt_1next.x() and \
               (pt_next.y() < pt.y() < pt_1next.y() or pt_1next.y() < pt.y() < pt_next.y()):
                self.connPoints.remove(pt_next)
                        
            elif pt.y() == pt_next.y() == pt_1next.y() and \
                 (pt_next.x() < pt.x() < pt_1next.x() or pt_1next.x() < pt.x() < pt_next.x()):
                self.connPoints.remove(pt_next)
            
        p.lineTo(pt)
        for el in self.connPoints:
            p.lineTo(el)
        p.lineTo(self.pos2)
        self.setPath(p)

    def update_path_draw2Conn(self, item):
        p = QPainterPath()
        d = item.get_direction(self.pos1)
        p.moveTo(self.pos1)

        if len(self.connPoints) == 0:
            pt_end = self.pos2
        else:
            pt_end = self.connPoints[0]
            
        if d == 'h':
            pt =  QPointF(self.pos1.x(), pt_end.y())
            p.lineTo(pt)
        else:
            pt1 = QPointF((self.pos1.x()+pt_end.x())/2, self.pos1.y())
            pt2 = QPointF((self.pos1.x()+pt_end.x())/2, pt_end.y())                        
            p.lineTo(pt1)
            p.lineTo(pt2)
           
        for el in self.connPoints:
            p.lineTo(el)
        p.lineTo(self.pos2)
        self.setPath(p)        
        
    def paint(self, painter, option, widget):
        pen = QPen(self.draw_color)
        pen.setWidth(LW)
        if self.isSelected():
            pen.setStyle(Qt.PenStyle.DotLine)
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
    
    def cleanPts(self):        
        while True:
            N = len(self.connPoints)
            if N==2:
                return
            errPos = []
            for n in range(1, N-1):
                if self.connPoints[n-1].x() == self.connPoints[n].x() == self.connPoints[n+1].x() or \
                self.connPoints[n-1].y() == self.connPoints[n].y() == self.connPoints[n+1].y():
                    errPos.append(self.connPoints[n])
            if len(errPos)==0:
                try:
                    x = self.connPoints[-2].x()
                    y = self.pos2.y()
                    self.connPoints[-1] = QPointF(x,y)
                    x = self.connPoints[1].x()
                    y = self.pos1.y()
                    self.connPoints[0] = QPointF(x,y)
                except:
                    pass
                return
            else:
                self.connPoints.remove(errPos[0])
        
    def save(self):
        self.cleanPts()
        try:
            pos1 = (self.pos1.x(), self.pos1.y())
            pos2 = (self.pos2.x(), self.pos2.y())
            points = []
            for el in self.connPoints:
                points.append((el.x(), el.y()))
                
            keys = ['pos1', 'pos2', 'points']
            vals = [pos1, pos2, points]
            
            return dict(zip(keys, vals))
        except:
            pass

    def load(self, item, dx = 0.0, dy = 0.0):
        try:
            pt1 = QPointF(item['pos1'][0], item['pos1'][1])
            pt2 = QPointF(item['pos2'][0], item['pos2'][1])
            dpt = QPointF(dx, dy)
            self.pos1 = self.gridPos(pt1+dpt)
            self.pos2 = self.gridPos(pt2+dpt)
            self.connPoints = []
            for el in item['points']:
                pt = QPointF(el[0], el[1])+dpt
                self.connPoints.append(pt)
            self.cleanPts()
            self.update_ports_from_pos()
        except:
            pass
            
    def gridPos(self, pt):
         gr = GRID
         x = gr * ((pt.x() + gr /2) // gr)
         y = gr * ((pt.y() + gr /2) // gr)
         return QPointF(x,y)
         
    def connSelected(self, pos):
        points = [self.pos1]
        for el in self.connPoints:
            points.append(el)
        points.append(self.pos2)
        N = len(points)
        for n in range(0,N-1):
            p1 = points[n]
            p2 = points[n+1]
            rect = QRectF(p1 - QPointF(DB,DB) ,p2 + QPointF(DB,DB))
            if rect.contains(pos):
                return True
        return False
    
    def connInSelection(self, rect):
        if self.selected:
            self.selected = False
            return True
        else:
            points = [self.pos1]
            for el in self.connPoints:
                points.append(el)
            points.append(self.pos2)
            for pt in points:
                if not rect.contains(pt):
                    return False
            return True
            
        
    def mousePressEvent(self, event):
        if self.connSelected(event.scenePos()):
            self.setSelected(True)
            self.selected = True
            event.accept()
        else:
            event.ignore()
                    
