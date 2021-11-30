from PyQt5.QtWidgets import QMenu, QGraphicsItem, QApplication, QGraphicsView
from PyQt5.QtGui import QTransform
from PyQt5.QtCore import Qt, QEvent, QObject, QPointF, QRectF, QSizeF, QMimeData

from supsisim.port import Port, InPort, OutPort
from supsisim.connection import Connection
from supsisim.block import Block
from supsisim.dialg import BlockName_Dialog
import supsisim.RCPDlg as pDlg
from supsisim.const import GRID, DB, DP
from supsisim.node import Node
import numpy as np
from lxml import etree

# States
IDLE = 0
LEFTMOUSEPRESSED = 1
ITEMSELECTED = 2
DRAWFROMOUTPORT = 3
DRAWFROMINPORT = 4
DRAWFROMCONNECTION = 5
MOVECONN = 6

MOUSEMOVE = 0
LEFTMOUSEPRESSED = 1
RIGHTMOUSEPRESSED = 2
MOUSERELEASED = 3
MOUSEDOUBLECLICK = 4
KEY_DEL = 5
KEY_ESC = 6
    
class Editor(QObject):
    """ Editor to handles events"""
    def __init__(self, parent):
        super(Editor, self).__init__(parent)
        self.mainw = parent
        self.conn = None
        self.scene = None
        self.movBlk = False
        self.event = None
        self.state = IDLE

        self.menuIOBlk = QMenu()
        parBlkAction = self.menuIOBlk.addAction('Block I/Os')
        paramsBlkAction = self.menuIOBlk.addAction('Block Parameters')
        flpBlkAction = self.menuIOBlk.addAction('Flip Block')
        nameBlkAction = self.menuIOBlk.addAction('Change Name')
        cloneBlkAction = self.menuIOBlk.addAction('Clone Block')
        copyBlkAction = self.menuIOBlk.addAction('Copy Block')
        deleteBlkAction = self.menuIOBlk.addAction('Delete Block')
        
        parBlkAction.triggered.connect(self.parBlock)
        flpBlkAction.triggered.connect(self.flipBlock)
        nameBlkAction.triggered.connect(self.nameBlock)
        paramsBlkAction.triggered.connect(self.paramsBlock)
        cloneBlkAction.triggered.connect(self.cloneBlock)
        copyBlkAction.triggered.connect(self.copyBlock)
        deleteBlkAction.triggered.connect(self.deleteBlock)

        self.subMenuConn = QMenu()
        connAddAction = self.subMenuConn.addAction('Add connection')
        connDelAction = self.subMenuConn.addAction('Delete connection')
        connAddAction.triggered.connect(self.addConn)
        connDelAction.triggered.connect(self.deleteConn)

        self.subMenuEditor = QMenu()
        pasteAction = self.subMenuEditor.addAction('Paste')
        pasteAction.triggered.connect(self.pasteBlock)

        # Matrix has two index [state, event]
        # States:
        # IDLE                                    0
        # LEFTMOUSEPRESSED         1
        # ITEMSELECTED                   2
         #DRAWFROMOUTPORT        3
        # DRAWFROMINPORT            4
        # DRAWFROMCONNECTION  5
        # MOVECONN                        6
        
        # Events
        # MOUSEMOVE                      0
        # LEFTMOUSEPRESSED          1
        # RIGHTMOUSEPRESSED       2
        # MOUSERELEASED               3
        # MOUSEDOUBLECLICK         4
        # KEY_DEL                              5
        # KEY_ESC                              6

        self.Fun = [[self.P00, self.P01, self.P02, self.PDM, self.P03, self.P04, self.P05],
                        [self.P06, self.PDM, self.PDM, self.P07, self.PDM, self.PDM, self.PDM],
                        [self.P11, self.P01, self.P02, self.PDM, self.P03, self.P04, self.P05],
                        [self.P10, self.P08, self.P09, self.P08, self.PDM, self.PDM, self.P09],
                        [self.P15, self.P14, self.P09, self.P14, self.PDM, self.PDM, self.P09],
                        [self.P10, self.P16, self.P09, self.P08, self.PDM, self.PDM, self.P09],
                        [self.P12, self.PDM, self.PDM, self.P13, self.PDM, self.PDM, self.PDM]]
              
    def install(self, scene):
        scene.installEventFilter(self)
        self.scene = scene
    
    def parBlock(self):
        self.scene.DgmToUndo()
        ok = self.scene.mainw.parBlock()
        if not ok:
            self.scene.clearLastUndo()
    
    def flipBlock(self):
        self.scene.DgmToUndo()
        item = self.scene.item
        item.flip = not item.flip
        item.setFlip()
        
    def nameBlock(self):
        self.scene.DgmToUndo()
        item = self.scene.item
        dialog = BlockName_Dialog(self.scene.mainw)
        dialog.name.setText(item.name)
        res = dialog.exec_()
        if res == 1:
            item.name = str(dialog.name.text())
            item.label.setPlainText(item.name)
            w = item.label.boundingRect().width()
            item.label.setPos(-w/2, item.h/2+5)
        else:
            self.scene.clearLastUndo()
        
    def paramsBlock(self):
        item = self.scene.item
        params = item.params.split('|')
        blk = params[0]

        blk = blk.replace('Blk','Dlg')

        streV = 'import dialogs.' + blk +  ' as dlg'
        try:
            exec(streV)
            name =  item.name.replace(' ','_')
            cmd = 'dlg.' + blk + '(' + str(item.inp) + ',' + str(item.outp) + ',"' + item.params + '"' +  ',"' +  name + '")'
            pars = exec(cmd)

        except:
            self.scene.DgmToUndo()
            pars = pDlg.parsDialog(item.params, item.helpTxt)

            if pars != item.params:
                item.params = pars
            else:
                self.scene.clearLastUndo()
            
    def cloneBlock(self):
        self.scene.DgmToUndo()
        item = self.scene.item
        item.clone(QPointF(DP, DP))

    def copyBlock(self):
        root = etree.Element('root')
        item = self.scene.item
        item.save(root)
        msg = etree.tostring(root, pretty_print=True)
        clipboard = QApplication.clipboard()
        mimeData = QMimeData()
        mimeData.setText(msg.decode())
        clipboard.setMimeData(mimeData)

    def pasteBlock(self):
        self.scene.DgmToUndo()
        try:
            msg = QApplication.clipboard().text()
            root = etree.fromstring(msg)
            blocks = root.findall('block')
            meanPosX = 0.0
            meanPosY = 0.0
            numBlk = 0
            for item in blocks:
                pBlkX = float(item.findtext('posX'))
                pBlkY = float(item.findtext('posY'))
                meanPosX = meanPosX + pBlkX
                meanPosY = meanPosY + pBlkY
                numBlk += 1
            deltaPosX = self.scene.evpos.x()-meanPosX/numBlk
            deltaPosY = self.scene.evpos.y()-meanPosY/numBlk
            blocks = root.findall('block')            
            for item in blocks:
                self.scene.loadBlock(item, deltaPosX, deltaPosY)
            connections = root.findall('connection')
            for item in connections:
                self.scene.loadConn(item, deltaPosX, deltaPosY)
            try:
                self.redrawNodes()
            except:
                pass
        except:
            pass
       
    def deleteBlock(self):
        self.scene.DgmToUndo()
        item = self.scene.item
        item.remove()
        self.removeNodes()
        self.redrawNodes()

    def connectInPort(self, item):
        if len(item.connections)==0:
            self.conn.port2 = item
            self.conn.pos2 = item.scenePos()
            self.conn.port1.connections.append(self.conn)
            self.conn.port2.connections.append(self.conn)
            if len(self.conn.connPoints) == 0:
                pos1 = QPointF((self.conn.pos2.x()+self.conn.pos1.x())/2, self.conn.pos1.y())
                pos2 = QPointF((self.conn.pos2.x()+self.conn.pos1.x())/2, self.conn.pos2.y())
                self.conn.connPoints.append(self.gridPos(pos1))
                self.conn.connPoints.append(self.gridPos(pos2))
            else:
                pt = self.conn.connPoints[-1]
                pos1 = QPointF(pt.x(), self.conn.pos2.y())
                self.conn.connPoints.append(self.gridPos(pos1))
        self.conn.clean()
        self.conn.update_path()
        self.conn = None
 
    def connectOutPort(self, item):        
        self.conn.port1 = item
        self.conn.pos1 = item.scenePos()
        self.conn.port1.connections.append(self.conn)
        self.conn.port2.connections.append(self.conn)    

        if len(self.conn.connPoints) == 0:
            pos1 = QPointF((self.conn.pos2.x()+self.conn.pos1.x())/2, self.conn.pos1.y())
            pos2 = QPointF((self.conn.pos2.x()+self.conn.pos1.x())/2, self.conn.pos2.y())
            self.conn.connPoints.append(self.gridPos(pos1))
            self.conn.connPoints.append(self.gridPos(pos2))
        else:
            pt = self.conn.connPoints[0]
            pos1 = QPointF(pt.x(), self.conn.pos1.y())
            self.conn.connPoints.insert(0,self.gridPos(pos1))
                
        self.conn.clean()
        self.conn.update_path()
        self.conn = None
 
    def deleteConn(self):
        self.scene.DgmToUndo()
        self.scene.item.remove()
        self.removeNodes()
        self.redrawNodes()
    
    def addConn(self):
        self.scene.DgmToUndo()
        c = self.scene.item
        posMouse = self.gridPos(self.scene.evpos)
        self.conn = Connection(None, self.scene)
        self.conn.port1 = c.port1
        self.conn.pos1 = c.pos1

        try:
            npos, pos = c.find_exact_pos(posMouse)
        except:
            return
        N = len(c.connPoints)
        
        if npos == 0:
            pos = posMouse
            npt = 0
        elif npos == N:
            pos = posMouse
            npt = N
        else:            
            npt = npos
       
        for n in range(0, npt):
            self.conn.connPoints.append(c.connPoints[n])
        self.conn.connPoints.append(pos)
        self.state = DRAWFROMCONNECTION
        self.firstTime = True

    def link2Connection(self, c):
        posMouse = self.gridPos(self.conn.pos1)
        
        try:
            npos, pos = c.find_exact_pos(posMouse)
        except:
            return

        connPoints = self.conn.connPoints.copy()
            
        self.conn.pos1 = c.pos1
        self.conn.port1 = c.port1

        self.conn.connPoints.clear()
        
        for n in range(0,npos):
            self.conn.connPoints.append(c.connPoints[n])
        self.conn.connPoints.append(posMouse)

        if len(connPoints) == 0:
            pt_end = self.conn.pos2
        else:
            pt_end = connPoints[0]
            
        d = c.get_direction(posMouse)

        if d == 'h':
            pt =  QPointF(posMouse.x(), pt_end.y())
            self.conn.connPoints.append(pt)
        else:
            pt1 = QPointF((posMouse.x()+pt_end.x())/2, posMouse.y())
            pt2 = QPointF((posMouse.x()+pt_end.x())/2, pt_end.y())                        
            self.conn.connPoints.append(pt1)
            self.conn.connPoints.append(pt2)

        for el in connPoints:
            self.conn.connPoints.append(el)
             
        self.conn.port1.connections.append(self.conn)
        self.conn.port2.connections.append(self.conn)
        self.conn.clean()
        self.conn.update_path()
        self.conn = None
    
    def redrawSelectedItems(self):
        for item in self.scene.selectedItems():
            if isinstance(item, Block):
                item.setPos(item.scenePos())
                for el in item.childItems():
                    try:
                        for conn in el.connections:
                            conn.update_pos_from_ports()
                    except:
                        pass

    def clean_points(self, pts, m):
        N = len(pts)
        remPt = []
        for n in range(1,N-1):
            if m=='x':
                if pts[n-1].x() == pts[n].x() == pts[n+1].x():
                    remPt.append(pts[n])
            elif m=='y':
                if pts[n-1].y() == pts[n].y() == pts[n+1].y():
                    remPt.append(pts[n])
                
        for el in remPt:
            pts.remove(el)
        return pts
        
    def ptInLine(self, pt, p1, p2):
        rect = QRectF(p1-QPointF(0.5,0.5), p2+QPointF(0.5,0.5))
        if rect.contains(pt):
            return True
        else:
            return False
        
    def setNode(self, pts1, pts2):
        pts1 = self.clean_points(pts1, 'x')
        pts1 = self.clean_points(pts1, 'y')
        pts2 = self.clean_points(pts2, 'x')
        pts2 = self.clean_points(pts2, 'y')
        
        n = 0
        N = min(len(pts1), len(pts2))
        try:
            while pts1[n] == pts2[n] and n<N:
                n +=1
            p1_prev = pts1[n-1]
            p1 = pts1[n]
            p2_prev = pts2[n-1]
            p2 = pts2[n]
 
            if self.ptInLine(p1, p2_prev, p2):
                pos = p1
            elif self.ptInLine(p2, p1_prev, p1):
                pos = p2
            else:
                pos = p1_prev
            
            node = Node(None, self.scene)
            node.setPos(pos)
        except:
            pass

    def redrawNodesFromPort(self, p):
        N = len(p.connections)
        for n in range(0,N):
            pts1 = [p.connections[n].pos1]
            for el in p.connections[n].connPoints:
                pts1.append(el)
            pts1.append(p.connections[n].pos2)
            for m in range(n+1,N):
                pts2 = [p.connections[m].pos1]
                for el in p.connections[m].connPoints:
                    pts2.append(el)
                pts2.append(p.connections[m].pos2)

                self.setNode(pts1, pts2)
               
    def redrawNodes(self):
        for el in self.scene.items():
            if isinstance(el, Node):
                el.remove()
        for item in self.scene.items():
            if isinstance(item, Block):
                for p in item.childItems():
                    if isinstance(p, OutPort):
                        if len(p.connections) > 1:
                            self.redrawNodesFromPort(p)
                                           
    def removeNodes(self):
        for el in self.scene.items():
            if isinstance(el, Node):
                el.remove()
                
    def itemAt(self, pos):
        rect = QRectF(pos+QPointF(-DB,-DB), QSizeF(2*DB,2*DB))
        items =  self.scene.items(rect)

        for item in items:
            if isinstance(self.findBlockAt(pos), Block):
                return item
        for item in items:
            if isinstance(self.findOutPortAt(pos), OutPort):
                return item
        for item in items:
            if isinstance(self.findInPortAt(pos), InPort):
                return(item)
        for item in items:
            if isinstance(self.findConnectionAt(pos), Connection):
                return(item)
        return None

    def itemByDraw(self, pos):
        rect = QRectF(pos-QPointF(DB,DB), QSizeF(2*DB,2*DB))
        items =  self.scene.items(QRectF(pos-QPointF(DB,DB), QSizeF(2*DB,2*DB)))
        for item in items:
            if isinstance(item, Port):
                return(item)
            elif isinstance(item, Connection) and item != self.conn:
                return item
        return None
    
    def findInPortAt(self, pos):
        items =  self.scene.items(QRectF(pos-QPointF(DB,DB), QSizeF(2*DB,2*DB)))
        for el in items:
            if isinstance(el, InPort):
                return el
        return None

    def findOutPortAt(self, pos):
        items =  self.scene.items(QRectF(pos-QPointF(DB,DB), QSizeF(2*DB,2*DB)))
        for el in items:
            if isinstance(el, OutPort):
                return el
        return None

    def findBlockAt(self, pos):
        items =  self.scene.items(QRectF(pos-QPointF(DB,DB), QSizeF(2*DB,2*DB)))
        for el in items:
            if isinstance(el, Block):
                return el
        return None

    def findConnectionAt(self, pos):
        items =  self.scene.items(QRectF(pos-QPointF(DB,DB), QSizeF(2*DB,2*DB)))
        for c in items:
            if isinstance(c, Connection):
                points = [c.pos1]
                for el in c.connPoints:
                    points.append(el)
                points.append(c.pos2)
                N = len(points)
                for n in range(0,N-1):
                    p1 = points[n]
                    p2 = points[n+1]
                    rect = QRectF(p1 - QPointF(DB,DB) ,p2 + QPointF(DB,DB))
                    if rect.contains(pos):
                        return c
        return None
    
    def findOtherConnectionAt(self, pos, orig_c):
        items =  self.scene.items(QRectF(pos-QPointF(DB,DB), QSizeF(2*DB,2*DB)))
        for c in items:
            if isinstance(c, Connection) and isinstance(c.port1, OutPort):
                return(c)
        return None

    def deleteSelected(self):
        items = self.scene.selectedItems()
        self.scene.DgmToUndo()
        for item in items:
            try:
                item.remove()
                self.editor.removeNodes()
                self.editor.redrawNodes()
            except:
                pass
 
    def deselect_all(self):
        for el in self.scene.items():
            el.setSelected(False)

    def setMouseInitDraw(self, pos):
        pointer = Qt.ArrowCursor
        itemB = self.findBlockAt(pos)
        itemOP = self.findOutPortAt(pos)
        itemIP = self.findInPortAt(pos)
        itemC = self.findConnectionAt(pos)
        if isinstance(itemB, Block):
            pointer = Qt.ArrowCursor
        elif isinstance(itemOP, OutPort) and len(itemOP.connections)==0:
            pointer = Qt.CrossCursor
        elif isinstance(itemIP, InPort) and len(itemIP.connections)==0:
            pointer = Qt.CrossCursor
        elif isinstance(itemC, Connection):
            pointer = Qt.PointingHandCursor
        else:
            pointer = Qt.ArrowCursor
        self.scene.mainw.view.setCursor(pointer)        
            
    def setMouseByDraw(self, item):
        if isinstance(item, InPort) and len(item.connections)==0:
            pointer = Qt.CrossCursor
        elif isinstance(item, OutPort):
            pointer = Qt.CrossCursor
        elif isinstance(item, Connection) and self.state==DRAWFROMINPORT:
            pointer = Qt.CrossCursor            
        else:
            pointer = Qt.DragLinkCursor
        self.scene.mainw.view.setCursor(pointer)        
            
    def PDM(self, obj, event):    # Dummy function - No action
        pass

    def P00(self, obj, event):                                   # IDLE + MOUSEMOVE
        self.setMouseInitDraw(event.scenePos())
        
        item = self.itemAt(event.scenePos())
        if item == None:
            self.deselect_all()
        else:
            try:
                item.setSelected(True)
            except:
                pass
        self.scene.updateDgm()
 
    def P01(self, obj, event):                                     # IDLE, ITEMSELECTED + LEFTMOUSEPRESSED
        item = self.findConnectionAt(event.scenePos())
        if item != None:
            self.scene.currentItem = item
            self.currentPos = event.scenePos()
            self.deselect_all()
            self.scene.DgmToUndo()
            self.state = MOVECONN

        elif self.findOutPortAt(event.scenePos()) != None:
            item = self.findOutPortAt(event.scenePos())
            self.scene.DgmToUndo()
            self.state = DRAWFROMOUTPORT
            self.conn = Connection(None, self.scene)
            self.mainw.view.setDragMode(QGraphicsView.NoDrag)
            self.conn.port1 = item
            self.conn.pos1 = item.scenePos()
            self.conn.pos2 = item.scenePos()
            self.firstTime = True
            
        elif self.findInPortAt(event.scenePos()) != None:
            item = self.findInPortAt(event.scenePos())
            self.scene.DgmToUndo()
            self.state = DRAWFROMINPORT
            self.conn = Connection(None, self.scene)
            self.mainw.view.setDragMode(QGraphicsView.NoDrag)
            self.conn.port2 = item
            self.conn.pos1 = item.scenePos()
            self.conn.pos2 = item.scenePos()
            self.firstTime = True
            
        else:
            self.state = LEFTMOUSEPRESSED
            
    def P02(self, obj, event):                                     # IDLE, ITEMSELECTED + RIGHTMOUSEPRESSED
        item = self.findBlockAt(event.scenePos())
        self.deselect_all()
        if isinstance(item, Block):
            item.setSelected(True)
            self.scene.item = item
            self.scene.evpos = event.scenePos()
            try:
                self.menuIOBlk.exec_(event.screenPos())
            except:
                pass
        else:                
            item = self.findConnectionAt(event.scenePos())
            if isinstance(item,Connection):
                self.scene.item = item
                self.scene.evpos = event.scenePos()
                try:
                    self.subMenuConn.exec_(event.screenPos())
                except:
                    pass

        if item == None:      
            self.scene.evpos = event.scenePos()
            self.subMenuEditor.exec_(event.screenPos())
            

    def P03(self, obj, event):                                     # IDLE, ITEMSELECTED + MOUSEDOUBLECLICK
        item = self.findBlockAt(event.scenePos())
        self.deselect_all()
        if isinstance(item, Block):
            item.setSelected(True)
            self.scene.item = item
            self.scene.evpos = event.scenePos()
            self.paramsBlock()

    def P04(self, obj, event):                                     # ITEMSELECTED + KEY_DEL
        self.deleteSelected()
        self.state = IDLE
        self.redrawNodes()

    def P05(self, obj, event):                                     # ITEMSELECTED + KEY_ESC
        self.state = IDLE

    def P06(self, obj, event):                                     # LEFTMOUSEPRESSED + MOUSEMOVE
        self.redrawSelectedItems()
        self.removeNodes()
        item = self.itemAt(event.scenePos())
                        
    def P07(self, obj, event):                                      # LEFTMOUSEPRESSED + MOUSERELEASED
        self.redrawSelectedItems()
        self.redrawNodes()
        
        item = self.itemAt(event.scenePos())
        if self.scene.currentItem != None:
            self.scene.currentItem = None
            self.deselect_all()
            try:
                item.setSelected(True)
                self.scene.currentItem = item
            except:
                pass        

        if self.scene.selectedItems():
            self.state = ITEMSELECTED
        else:
            self.state = IDLE
        
    def P08(self, obj, event):                                      # DRAWFROMOUTPORT + LEFTMOUSEPRESSED
        item = self.findInPortAt(event.scenePos())
        if isinstance(item,InPort):
            self.connectInPort(item)
            self.redrawNodes()
            self.state = IDLE
            self.mainw.view.setDragMode(QGraphicsView.RubberBandDrag)
        else:
            pt = self.gridPos(event.scenePos())
            if self.firstTime:
                self.firstTime = False
            else:
                self.conn.addPoint(pt)
            if len(self.conn.connPoints)!=0:
                self.conn.pos2 = pt            
            self.conn.update_path()
                
    def P09(self, obj, event):                                      # DRAWFROMOUTPORT + RIGHTMOUSEPRESSED, KEY_ESC
        try:
            self.conn.remove()
            self.scene.undoDgm()
        except:
            pass
        self.conn = None
        self.state = IDLE
       
    def P10(self, obj, event):                                      # DRAWFROMOUTPORT + MOUSEMOVE
        item = self.itemByDraw(event.scenePos())
        self.setMouseByDraw(item)
        self.conn.pos2 = event.scenePos()
        if isinstance(item, InPort):
            self.conn.update_path_draw2InPort()
        else:
            self.conn.update_path_draw2Pt_fw()
                    
    def P11(self, obj, event):                                      # ITEMSELECTED + MOUSEMOVE
        self.setMouseInitDraw(event.scenePos())

    def P12(self, obj, event):                                      # MOVECONN + MOUSEMOVE
        item = self.scene.currentItem
        N = len(item.connPoints)
        oldPos = self.currentPos
        newPos = self.gridPos(event.scenePos())
        try:
            npos, pos = item.find_exact_pos(oldPos)
        except:
            return
            
        if npos != 0 and npos != N:
            ok = item.move(npos, newPos)
            if ok:
                self.currentPos = newPos

    def P13(self, obj, event):                                         # MOVECONN + MOUSERELEASE
        item = self.scene.currentItem
        N = len(item.connPoints)
        oldPos = self.currentPos
        newPos = self.gridPos(event.scenePos())
        try:
            npos, pos = item.find_exact_pos(oldPos)
        except:
            return
            
        if npos != 0 and npos != N:
            ok = item.move(npos, newPos)
            if ok:
                self.currentPos = newPos

        self.redrawNodes()
        self.scene.currentItem = None
        self.state = IDLE

    def P14(self, obj, event):                          # DRAWFROMINPORT + LEFTMOUSEPRESSED  OR MOUSERELEASED
        item1 = self.findOutPortAt(event.scenePos())
        item2 = None
        try:
            item2 = self.findOtherConnectionAt(event.scenePos(), self.conn)
        except:
            pass
        if isinstance(item1,OutPort):
            self.connectOutPort(item1)  
            self.redrawNodes()
            self.state = IDLE
            self.mainw.view.setDragMode(QGraphicsView.RubberBandDrag)
        elif isinstance(item2, Connection):
            self.link2Connection(item2)
            self.redrawNodes()
            self.state = IDLE
            self.mainw.view.setDragMode(QGraphicsView.RubberBandDrag)
        else:
            pt = self.gridPos(event.scenePos())
            if self.firstTime:
                self.firstTime = False
            else:
                self.conn.insPoint(pt)
            if len(self.conn.connPoints)!=0:
                self.conn.pos1 = pt            
            self.conn.update_path()

    def P15(self, obj, event):                              # DRAWFROMINPORT + MOUSEMOVE  
        item = self.itemByDraw(event.scenePos())
        self.setMouseByDraw(item)
        self.conn.pos1 = event.scenePos()
        if isinstance(item, OutPort):
            self.conn.update_path_draw2OutPort()
        elif isinstance(item, Connection):
            self.conn.update_path_draw2Conn(item)
        else:
            self.conn.update_path_draw2Pt_bk()
        pass
        
    def P16(self, obj, event):                                      # DRAWFROMCONNECTION + LEFTMOUSEPRESSED
        item = self.findInPortAt(event.scenePos())
        if isinstance(item,InPort):
            self.connectInPort(item)
            self.redrawNodes()
            self.state = IDLE
            self.mainw.view.setDragMode(QGraphicsView.RubberBandDrag)
        else:
            pt = self.gridPos(event.scenePos())
            """
            if self.firstTime:
                self.firstTime = False
            else:
                self.conn.addPoint(pt)
            """

            self.conn.addPoint(pt)
           
            if len(self.conn.connPoints)!=0:
                self.conn.pos2 = pt            
            self.conn.update_path()
            if self.firstTime:
                self.firstTime = False
                self.conn.connPoints.remove(self.conn.connPoints[-1])
                
    def eventFilter(self, obj, event):
        ev = -1
        if event.type() ==  QEvent.GraphicsSceneMouseMove:
            ev = MOUSEMOVE
             
        if event.type() ==  QEvent.GraphicsSceneMousePress:
            self.mainw.statusLabel.setText('')
            if event.button() == Qt.LeftButton:
                ev = LEFTMOUSEPRESSED
            if event.button() == Qt.RightButton:
                ev = RIGHTMOUSEPRESSED
        
        if event.type() == QEvent.GraphicsSceneMouseRelease:
            ev = MOUSERELEASED
                
        if event.type() == QEvent.GraphicsSceneMouseDoubleClick:
            self.mainw.statusLabel.setText('')
            ev = MOUSEDOUBLECLICK

        if event.type() == QEvent.KeyPress:
            self.mainw.statusLabel.setText('')
            if event.key() == Qt.Key_Delete:
                ev = KEY_DEL
            if event.key() == Qt.Key_Escape:
                ev = KEY_ESC

        if ev != -1:
            fun = self.Fun[self.state][ ev]
            fun(obj, event)
                 
        return False

    def gridPos(self, pt):
         gr = GRID
         x = gr * ((pt.x() + gr /2) // gr)
         y = gr * ((pt.y() + gr /2) // gr)
         return QPointF(x,y)

