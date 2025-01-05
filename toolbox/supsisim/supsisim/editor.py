from supsisim.qtvers import *

from supsisim.port import Port, InPort, OutPort
from supsisim.connection import Connection
from supsisim.subsblock import subsBlock
from supsisim.block import Block
from supsisim.dialg import BlockName_Dialog
import supsisim.RCPDlg as pDlg
from supsisim.const import GRID, DB, DP
from supsisim.node import Node
import json
from shv import SHVDecimal
from decimal import Decimal, ROUND_DOWN


# States
IDLE = 0
LEFTMOUSEPRESSED    = 1
ITEMSELECTED        = 2
DRAWFROMOUTPORT     = 3
DRAWFROMINPORT      = 4
DRAWFROMCONNECTION  = 5
MOVECONN            = 6

MOUSEMOVE = 0
LEFTMOUSEPRESSED    = 1
RIGHTMOUSEPRESSED   = 2
MOUSERELEASED       = 3
MOUSEDOUBLECLICK    = 4
KEY_DEL             = 5
KEY_ESC             = 6
    
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
        self.active = True

        self.menuIOBlk = QMenu()
        parBlkAction = self.menuIOBlk.addAction('Block I/Os')
        paramsBlkAction = self.menuIOBlk.addAction('Block Parameters')
        flpBlkAction = self.menuIOBlk.addAction('Flip Block')
        nameBlkAction = self.menuIOBlk.addAction('Change Name')
        cloneBlkAction = self.menuIOBlk.addAction('Clone Block')
        copyBlkAction = self.menuIOBlk.addAction('Copy Block')
        deleteBlkAction = self.menuIOBlk.addAction('Delete Block')
        shvBlkAction = self.menuIOBlk.addAction('Tune parameters')
        
        self.menuSubsBlk = QMenu()
        opensubsBlkAction = self.menuSubsBlk.addAction('Open subsystem')
        flpsubBlkAction = self.menuSubsBlk.addAction('Flip Block')
        namesubBlkAction = self.menuSubsBlk.addAction('Change Name')
        copysubBlkAction = self.menuSubsBlk.addAction('Copy Block')
        deletesubBlkAction = self.menuSubsBlk.addAction('Delete Block')
        # shvBlkAction = self.menuSubsBlk.addAction('Tune parameters')
          
        parBlkAction.triggered.connect(self.parBlock)
        flpBlkAction.triggered.connect(self.flipBlock)
        nameBlkAction.triggered.connect(self.nameBlock)
        paramsBlkAction.triggered.connect(self.paramsBlock)
        cloneBlkAction.triggered.connect(self.cloneBlock)
        copyBlkAction.triggered.connect(self.copyBlock)
        deleteBlkAction.triggered.connect(self.deleteBlock)
        opensubsBlkAction.triggered.connect(self.openSubsystem)
        flpsubBlkAction.triggered.connect(self.flipBlock)
        copysubBlkAction.triggered.connect(self.copyBlock)
        namesubBlkAction.triggered.connect(self.nameBlock)
        deletesubBlkAction.triggered.connect(self.deleteBlock)
        shvBlkAction.triggered.connect(self.shvAction)
        
        self.subMenuConn = QMenu()
        connAddAction = self.subMenuConn.addAction('Add connection')
        connDelAction = self.subMenuConn.addAction('Delete connection')
        connAddAction.triggered.connect(self.addConn)
        connDelAction.triggered.connect(self.deleteConn)

        self.subMenuEditor = QMenu()
        pasteAction = self.subMenuEditor.addAction('Paste')
        pasteAction.triggered.connect(self.pasteBlock)

        self.subMenuSubsystem = QMenu()
        subcopyAction = self.subMenuSubsystem.addAction('Copy')
        subsystemAction = self.subMenuSubsystem.addAction('Create subsystem')
        subcopyAction.triggered.connect(self.copyBlock)
        subsystemAction.triggered.connect(self.createSubsystem)

        # Matrix has two index [state, event]
        # States:
        # IDLE                0
        # LEFTMOUSEPRESSED    1
        # ITEMSELECTED        2
        # DRAWFROMOUTPORT     3
        # DRAWFROMINPORT      4
        # DRAWFROMCONNECTION  5
        # MOVECONN            6
        
        # Events
        # MOUSEMOVE           0
        # LEFTMOUSEPRESSED    1
        # RIGHTMOUSEPRESSED   2
        # MOUSERELEASED       3
        # MOUSEDOUBLECLICK    4
        # KEY_DEL             5
        # KEY_ESC             6

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
        
    # Menu actions for blocks and subblocks
    
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
        res = dialog.exec()
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
            name =  item.name.replace(' ','_') + '_' + str(item.ident)
            cmd = 'dlg.' + blk + '(' + str(item.inp) + ',' + str(item.outp) + \
                  ',  "' + item.params + '"' +  ',"' +  name + '")'
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
        self.mainw.copyAct()

    def copysubBlock(self):
        self.mainw.copyAct()

    def pasteBlock(self):
        self.scene.DgmToUndo()
        try:
            msg = QApplication.clipboard().text()
            data = json.loads(msg)
            n = 0
            px = 0
            py = 0
            for item in data['blocks']:
                n += 1
                px += item['pos'][0]
                py += item['pos'][1]
            for item in data['subsystems']:
                n += 1
                px += item['block']['pos'][0]
                py += item['block']['pos'][1]
            px = px/n
            py = py/n
            DPx = self.scene.evpos.x()-px
            DPy = self.scene.evpos.y()-py
            gr = GRID
            DPx = gr * ((DPx + gr /2) // gr)
            DPy = gr * ((DPy + gr /2) // gr)            
            self.scene.DictToDgm(data, DPx, DPy)
            self.deselect_all()
        except:
            pass
       
    def deleteBlock(self):
        self.scene.DgmToUndo()
        item = self.scene.item
        item.remove()
        self.removeNodes()
        self.redrawNodes()

    def shvAction(self):
        if not self.scene.SHV.tuned:
            dlg = QMessageBox()
            dlg.setWindowTitle("Warning!")
            dlg.setText("Enable tuning option")
            dlg.setStandardButtons(QMessageBox.StandardButton.Ok)
            dlg.setIcon(QMessageBox.Icon.Warning)
            dlg.exec()
            return

        connection = self.scene.getBrokerConnection()

        if not connection.is_connected():
            dlg = QMessageBox()
            dlg.setWindowTitle("Warning!")
            dlg.setText("No connection to broker")
            dlg.setStandardButtons(QMessageBox.StandardButton.Ok)
            dlg.setIcon(QMessageBox.Icon.Warning)
            dlg.exec()
            return

        # get corret name.ident values for the SHV paths to blocks
        self.scene.findAllItems(self.scene)

        item = self.scene.item
        params = item.params.split('|')
        blk = params[0]
        name =  item.name.replace(' ','_') + '_' + str(item.ident)
        items = item.params.split('|')
        parr = ""
        try:
            for i in range(1,len(items)):
                par = items[i].split(':')
                if par[2].replace(" ", "") == "double":
                    res = connection.get_parameter_value(par[0], name)
                    par[1] = str(float(res))
                    items[i] = ':'.join(par)
            name =  item.name.replace(' ','_') + '_' + str(item.ident)
            parr = '|'.join(items)
            if parr != item.params:
                params = parr
        except:
            print("Error connecting to the broker!")


        # params = it.em.params.split('|')
        blk = params[0]

        blk = blk.replace('Blk','Dlg')

        streV = 'import dialogs.' + blk +  ' as dlg'
        try:
            exec(streV)
            name =  item.name.replace(' ','_') + '_' + str(item.ident)
            cmd = 'dlg.' + blk + '(' + str(item.inp) + ',' + str(item.outp) + \
                ',  "' + params + '"' +  ',"' +  name + '")'
            pars = exec(cmd)

        except:
            items = parr.split('|')
            show = items[0]
            for i in range(1,len(items)):
                par = items[i].split(':')
                if par[2].replace(" ", "") == "double":
                    show += '|' + items[i]

            pars = pDlg.parsDialog(show, item.helpTxt)

            if pars != params and pars != "":
                params = pars
                items = params.split('|')

                for i in range(1,len(items)):
                    par = items[i].split(':')
                    parameter = par[1]
                    if par[2].replace(" ", "") == "double":
                        # parameter = SHVFloat(par[1])
                        try:
                            parameter = SHVDecimal(par[1]).quantize(Decimal('1.000000000'), rounding=ROUND_DOWN)
                        except:
                            print("Wrong data type")
                    else:
                        continue
                    connection.set_parameter_value(par[0], name, parameter)
            else:
                self.scene.clearLastUndo()
    
    # Subsystems

    def createSubsystem(self):
        self.scene.DgmToUndo()
        # Create subsystem Block        
                
        blocks = []
                
        for item in self.scene.selectedItems():
            blocks.append(item)
        b = subsBlock(None, self.scene, blocks)
        
        self.redrawNodes()
                                    
    def openSubsystem(self):
        item = self.scene.item
        item.openSubsystem()
        
    # Connections and ports
    
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
        self.conn.cleanPts()
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
        self.conn.cleanPts()
        self.conn.update_path()
        self.conn = None
 
    def deleteConn(self):
        try:
            self.scene.DgmToUndo()
            self.scene.item.remove()
            self.removeNodes()
            self.redrawNodes()
        except:
            pass
        
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
            self.conn.connPoints.append(QPointF(c.connPoints[n]))
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
        self.conn = None
        
    # Functions on diagram items
    
    def redrawSelectedItems(self):
        if len(self.scene.selectedItems())==1:
            mvFlag = False
        else:
            mvFlag = True
        
        for item in self.scene.selectedItems():
            if isinstance(item, Block):
                item.setPos(item.scenePos())
                for el in item.childItems():
                    try:
                        for conn in el.connections:
                            conn.clean()
                            conn.update_pos_from_ports(mvFlag)
                    except:
                        pass

    def getNumOfItems(self):
        tot = 0
        for item in self.scene.selectedItems():
            if isinstance(item, Block):
                tot += 1
        return tot
        
    def itemAt(self, pos):
        rect = QRectF(pos+QPointF(-DB,-DB), QSizeF(2*DB,2*DB))
        items =  self.scene.items(rect)
        return items

    def itemByDraw(self, pos):
        rect = QRectF(pos-QPointF(DB,DB), QSizeF(2*DB,2*DB))
        items =  self.scene.items(QRectF(pos-QPointF(DB,DB), QSizeF(2*DB,2*DB)))
        for item in items:
            if isinstance(item, Port):
                return(item)
#             elif isinstance(item, Connection) and item != self.conn:
#                 return item
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
        
    def setRect(self, p1, p2, delta):
        pt1X = min(p1.x(), p2.x())-delta
        pt2X = max(p1.x(), p2.x())+delta
        pt1Y = min(p1.y(), p2.y())-delta
        pt2Y = max(p1.y(), p2.y())+delta
        return QRectF(QPointF(pt1X,pt1Y), QPointF(pt2X, pt2Y))
            
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
                    rect = self.setRect(p1, p2, DB)
                    if rect.contains(pos):
                        return c
        return None
    
    def findOtherConnectionAt(self, pos, orig_c):
        items =  self.scene.items(QRectF(pos-QPointF(DB,DB), QSizeF(2*DB,2*DB)))
        for c in items:
            if isinstance(c, Connection) and isinstance(c.port1, OutPort):
                points = [c.pos1]
                for el in c.connPoints:
                    points.append(el)
                points.append(c.pos2)
                N = len(points)
                for n in range(0,N-1):
                    p1 = points[n]
                    p2 = points[n+1]
                    rect = self.setRect(p1, p2, DB)
                    if rect.contains(pos):
                        return c               
        return None

    def deleteSelected(self):
        self.removeNodes()
        
        dgmBlocks = []
        dgmSubsystems = []                
        
        items = self.scene.selectedItems()
        for item in items:
            if isinstance(item, subsBlock):
                dgmSubsystems.append(item)
            elif isinstance(item, Block):
                dgmBlocks.append(item)
            elif isinstance(item, Connection):
                self.scene.item = item
                self.deleteConn()
            else:
                pass
        for item in dgmBlocks:
            item.remove()
        for item in dgmSubsystems:
            item.remove()

        self.redrawNodes()
        
    def deselect_all(self):
        for el in self.scene.items():
            el.setSelected(False)
            
    # Connections points

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
        rect = self.setRect(p1, p2, 0.5)
        if rect.contains(pt):
            return True
        else:
            return False
        
    # Functions for nodes
    
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
            if p.connections[n].port2.parent in self.scene.items():
                pts1 = [p.connections[n].pos1]
                for el in p.connections[n].connPoints:
                    pts1.append(el)
                pts1.append(p.connections[n].pos2)
                for m in range(n+1,N):
                    pts2 = [p.connections[m].pos1]
                    for el in p.connections[m].connPoints:
                        pts2.append(el)
                    pts2.append(p.connections[m].pos2)
                try:
                    self.setNode(pts1, pts2)
                except:
                    pass
               
    def redrawNodes(self):
        self.removeNodes()
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
                
    # Positions functions
                
    def gridPos(self, pt):
         gr = GRID
         x = gr * ((pt.x() + gr /2) // gr)
         y = gr * ((pt.y() + gr /2) // gr)
         return QPointF(x,y)
                                
    # Mouse icons
                
    def setMouseInitDraw(self, pos):
        pointer = Qt.CursorShape.ArrowCursor
        itemB = self.findBlockAt(pos)
        itemOP = self.findOutPortAt(pos)
        itemIP = self.findInPortAt(pos)
        itemC = self.findConnectionAt(pos)
        if isinstance(itemB, Block):
            pointer = Qt.CursorShape.ArrowCursor
        elif isinstance(itemOP, OutPort) and len(itemOP.connections)==0:
            pointer = Qt.CursorShape.CrossCursor
        elif isinstance(itemIP, InPort) and len(itemIP.connections)==0:
            pointer = Qt.CursorShape.CrossCursor
        elif isinstance(itemC, Connection):
            pointer = Qt.CursorShape.PointingHandCursor
        else:
            pointer = Qt.CursorShape.ArrowCursor
        self.scene.mainw.view.setCursor(pointer)        
            
    def setMouseByDraw(self, item):
        if isinstance(item, InPort) and len(item.connections)==0:
            pointer = Qt.CursorShape.CrossCursor
        elif isinstance(item, OutPort):
            pointer = Qt.CursorShape.CrossCursor
        elif isinstance(item, Connection) and self.state==DRAWFROMINPORT:
            pointer = Qt.CursorShape.CrossCursor            
        else:
            pointer = Qt.CursorShape.DragLinkCursor
        self.scene.mainw.view.setCursor(pointer) 
        
    # =================  State events functions  ========================
            
    def PDM(self, obj, event):    
        # Dummy function - No action
        pass

    def P00(self, obj, event):                                   
        # IDLE + MOUSEMOVE
        # Mouse is moving and blocks are selected if mouse over them
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
 
    def P01(self, obj, event):                                     
        # IDLE, ITEMSELECTED + LEFTMOUSEPRESSED
        # Look for connections and ports and begin drawing from them
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
            self.mainw.view.setDragMode(QGraphicsView.DragMode.NoDrag)
            self.conn.port1 = item
            self.conn.pos1 = self.gridPos(item.scenePos())
            self.conn.pos2 = self.gridPos(item.scenePos())
            self.firstTime = True
            
        elif self.findInPortAt(event.scenePos()) != None:
            item = self.findInPortAt(event.scenePos())
            self.scene.DgmToUndo()
            self.state = DRAWFROMINPORT
            self.conn = Connection(None, self.scene)
            self.mainw.view.setDragMode(QGraphicsView.DragMode.NoDrag)
            self.conn.port2 = item
            self.conn.pos1 = self.gridPos(item.scenePos())
            self.conn.pos2 = self.gridPos(item.scenePos())
            self.firstTime = True
            
        else:
            self.state = LEFTMOUSEPRESSED
            
    def P02(self, obj, event):                                     
        # IDLE, ITEMSELECTED + RIGHTMOUSEPRESSED
        # Check if more blocks are selected
        NumOfItems = self.getNumOfItems()
        if NumOfItems>1:
            self.scene.evpos = event.scenePos()
            self.subMenuSubsystem.exec(event.screenPos())            
            self.deselect_all()
            return
        
        items = self.itemAt(event.scenePos())
        blks = [b for b in items if isinstance(b, Block)]
        conn = [c for c in items if isinstance(c, Connection)]
        if blks:
            item = blks[0]
            item.setSelected(True)
            self.scene.item = item
            self.scene.evpos = event.scenePos()           
            if isinstance(item, subsBlock):
                self.menuSubsBlk.exec(event.screenPos())
            else:
                self.menuIOBlk.exec(event.screenPos())
                
        elif conn:                
#             self.scene.item = conn[0]
            self.scene.item = self.findConnectionAt(event.scenePos())  # New
            self.scene.evpos = event.scenePos()
            try:
                self.subMenuConn.exec(event.screenPos())
            except:
                pass
                    
        else:      
            self.scene.evpos = event.scenePos()
            self.subMenuEditor.exec(event.screenPos())
        self.deselect_all()
            
    def P03(self, obj, event):                                     
        # IDLE, ITEMSELECTED + MOUSEDOUBLECLICK
        item = self.findBlockAt(event.scenePos())
        self.deselect_all()
        if isinstance(item,subsBlock):
            self.scene.item = item
            self.openSubsystem()        
        
        elif isinstance(item, Block):
            item.setSelected(True)
            self.scene.item = item
            self.scene.evpos = event.scenePos()
            self.paramsBlock()
            
        else:
            pass
            
    def P04(self, obj, event):                                     
        # ITEMSELECTED + KEY_DEL
        self.deleteSelected()
        self.state = IDLE
        self.redrawNodes()

    def P05(self, obj, event):                                     
        # ITEMSELECTED + KEY_ESC
        self.state = IDLE

    def P06(self, obj, event):                                     
        # LEFTMOUSEPRESSED + MOUSEMOVE
        self.redrawSelectedItems()
        self.removeNodes()
                        
    def P07(self, obj, event):                                      
        # LEFTMOUSEPRESSED + MOUSERELEASED        
        self.redrawSelectedItems()
        self.redrawNodes()
        if self.scene.selectedItems():
            self.state = ITEMSELECTED
        else:
            self.state = IDLE
        
    def P08(self, obj, event):                                      
        # DRAWFROMOUTPORT + LEFTMOUSEPRESSED
        item = self.findInPortAt(event.scenePos())
        if isinstance(item,InPort):
            self.connectInPort(item)
            self.redrawNodes()
            self.state = IDLE
            self.mainw.view.setDragMode(QGraphicsView.DragMode.RubberBandDrag)
        else:
            pt = self.gridPos(event.scenePos())
            if self.firstTime:
                self.firstTime = False
            else:
                self.conn.addPoint(pt)
            if len(self.conn.connPoints)!=0:
                self.conn.pos2 = pt            
            self.conn.update_path()
                
    def P09(self, obj, event):                                      
        # DRAWFROMOUTPORT + RIGHTMOUSEPRESSED, KEY_ESC
        try:
            self.conn.remove()
            self.scene.undoDgm()
        except:
            pass
        self.conn = None
        self.state = IDLE
       
    def P10(self, obj, event):                                      
        # DRAWFROMOUTPORT + MOUSEMOVE
        item = self.itemByDraw(event.scenePos())
        self.setMouseByDraw(item)
        self.conn.pos2 = self.gridPos(event.scenePos())
        if isinstance(item, InPort):
            self.conn.update_path_draw2InPort()
        else:
            self.conn.update_path_draw2Pt_fw()
                    
    def P11(self, obj, event):                                      
        # ITEMSELECTED + MOUSEMOVE
        self.setMouseInitDraw(event.scenePos())

    def P12(self, obj, event):                                      
        # MOVECONN + MOUSEMOVE
        item = self.scene.currentItem
#         item.draw_color = Qt.GlobalColor.red
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

    def P13(self, obj, event):                                         
        # MOVECONN + MOUSERELEASE
        item = self.scene.currentItem
#         item.draw_color = Qt.GlobalColor.black
        self.state = IDLE
        try:
            item.clean()
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
        except:
            None

    def P14(self, obj, event):                          
        # DRAWFROMINPORT + LEFTMOUSEPRESSED  OR MOUSERELEASED
        item1 = self.findOutPortAt(event.scenePos())
        if isinstance(item1,OutPort):
            self.connectOutPort(item1)  
            self.redrawNodes()
            self.state = IDLE
            self.mainw.view.setDragMode(QGraphicsView.DragMode.RubberBandDrag)
            return
        
        try:
            item2 = self.findOtherConnectionAt(event.scenePos(), self.conn)
        except:
            pass
            
        if isinstance(item2, Connection):
            self.link2Connection(item2)
            self.redrawNodes()
            self.state = IDLE
            self.mainw.view.setDragMode(QGraphicsView.DragMode.RubberBandDrag)
        else:
            pt = self.gridPos(event.scenePos())
            if self.firstTime:
                self.firstTime = False
            else:
                self.conn.insPoint(pt)
            if len(self.conn.connPoints)!=0:
                self.conn.pos1 = pt            
            self.conn.update_path()

    def P15(self, obj, event):                              
        # DRAWFROMINPORT + MOUSEMOVE  
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
        
    def P16(self, obj, event):                                      
        # DRAWFROMCONNECTION + LEFTMOUSEPRESSED
        item = self.findInPortAt(event.scenePos())
        if isinstance(item,InPort):
            self.connectInPort(item)
            self.redrawNodes()
            self.state = IDLE
            self.mainw.view.setDragMode(QGraphicsView.DragMode.RubberBandDrag)
        else:
            pt = self.gridPos(event.scenePos())

            self.conn.addPoint(pt)
           
            if len(self.conn.connPoints)!=0:
                self.conn.pos2 = pt            
            self.conn.update_path()
            if self.firstTime:
                self.firstTime = False
                self.conn.connPoints.remove(self.conn.connPoints[-1])
                
    def eventFilter(self, obj, event):
        if self.active:
            ev = -1
            if event.type() ==  QEvent.Type.GraphicsSceneMouseMove:
                ev = MOUSEMOVE
                
            if event.type() ==  QEvent.Type.GraphicsSceneMousePress:
                self.mainw.statusLabel.setText('')
                if event.button() == Qt.MouseButton.LeftButton:
                    ev = LEFTMOUSEPRESSED
                if event.button() == Qt.MouseButton.RightButton:
                    ev = RIGHTMOUSEPRESSED
            
            if event.type() == QEvent.Type.GraphicsSceneMouseRelease:
                ev = MOUSERELEASED
                    
            if event.type() == QEvent.Type.GraphicsSceneMouseDoubleClick:
                self.mainw.statusLabel.setText('')
                ev = MOUSEDOUBLECLICK
    
            if event.type() == QEvent.Type.KeyPress:
                self.mainw.statusLabel.setText('')
                if event.key() == Qt.Key.Key_Delete:
                    ev = KEY_DEL
                if event.key() == Qt.Key.Key_Escape:
                    ev = KEY_ESC
            if ev != -1:
                fun = self.Fun[self.state][ ev]
                fun(obj, event)
                 
        return False

