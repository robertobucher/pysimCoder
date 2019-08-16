from pyqt5  import QMenu, QGraphicsItem, QtCore, QTransform

from supsisim.port import Port, InPort, OutPort
from supsisim.connection import Connection
from supsisim.block import Block
from supsisim.node import Node
from supsisim.dialg import BlockName_Dialog
import supsisim.RCPDlg as pDlg
from supsisim.const import GRID, DB
import numpy as np

# States
IDLE = 0
LEFTMOUSEPRESSED = 1
ITEMSELECTED = 2
DRAWFROMOUTPORT = 3

MOUSEMOVE = 0
LEFTMOUSEPRESSED = 1
RIGHTMOUSEPRESSED = 2
MOUSERELEASED = 3
MOUSEDOUBLECLICK = 4
KEY_DEL = 5
KEY_ESC = 6
    
class Editor(QtCore.QObject):
    """ Editor to handles events"""
    def __init__(self, parent):
        super(Editor, self).__init__(parent)
        self.conn = None
        self.scene = None
        self.movBlk = False
        self.event = None
        self.connFromNode = False
        self.state = IDLE

        self.menuIOBlk = QMenu()
        parBlkAction = self.menuIOBlk.addAction('Block I/Os')
        paramsBlkAction = self.menuIOBlk.addAction('Block Parameters')
        flpBlkAction = self.menuIOBlk.addAction('Flip Block')
        nameBlkAction = self.menuIOBlk.addAction('Change Name')
        cloneBlkAction = self.menuIOBlk.addAction('Clone Block')
        deleteBlkAction = self.menuIOBlk.addAction('Delete Block')
        
        parBlkAction.triggered.connect(self.parBlock)
        flpBlkAction.triggered.connect(self.flipBlock)
        nameBlkAction.triggered.connect(self.nameBlock)
        paramsBlkAction.triggered.connect(self.paramsBlock)
        cloneBlkAction.triggered.connect(self.cloneBlock)
        deleteBlkAction.triggered.connect(self.deleteBlock)

        self.subMenuNode = QMenu()
        nodeDelAction = self.subMenuNode.addAction('Delete node')
        nodeBindAction = self.subMenuNode.addAction('Bind node')
        nodeDelAction.triggered.connect(self.deleteNode)
        nodeBindAction.triggered.connect(self.bindNode)
        
        self.subMenuConn = QMenu()
        connConnectAction = self.subMenuConn.addAction('Start connection')
        connDelAction = self.subMenuConn.addAction('Delete connection')
        connInsAction = self.subMenuConn.addAction('Insert node')
        connConnectAction.triggered.connect(self.newConn)
        connDelAction.triggered.connect(self.deleteConn)
        connInsAction.triggered.connect(self.insConn)

        # Matrix has two index [state, event]
        # States:
        # IDLE                                  0
        # LEFTMOUSEPRESSED       1
        # ITEMSELECTED                 2
        #DRAWFROMOUTPORT       3
        
        # Events
        # MOUSEMOVE                    0
        # LEFTMOUSEPRESSED        1
        # RIGHTMOUSEPRESSED     2
        # MOUSERELEASED             3
        # MOUSEDOUBLECLICK       4
        # KEY_DEL                            5
        # KEY_ESC                            6


        self.Fun = [[self.P00, self.P01, self.P02, self.PDM, self.P03, self.P04, self.P05],
                        [self.P06, self.PDM, self.PDM, self.P07, self.PDM, self.PDM, self.PDM],
                        [self.P12, self.P01, self.P02, self.P11, self.P03, self.P04, self.P05],
                        [self.P10, self.P08, self.P09, self.PDM, self.PDM, self.PDM, self.P09]]
                
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
        self.scene.DgmToUndo()
        item = self.scene.item
        params = item.params.split('|')
        blk = params[0]
        blk = blk.replace('Blk','Dlg')
        if blk in dir(pDlg):
            name =  item.name.replace(' ','_')
            cmd = 'pDlg.' + blk + '(' + str(item.inp) + ',' + str(item.outp) + ',"' + item.params + '"' +  ',"' +  name + '")'
            pars = eval(cmd)
        else:
            pars = pDlg.parsDialog(item.params)
        if pars != item.params:
            item.params = pars
        else:
            self.scene.clearLastUndo()

    def cloneBlock(self):
        self.scene.DgmToUndo()
        item = self.scene.item
        item.clone(QtCore.QPointF(100,100))

    def deleteBlock(self):
        self.scene.DgmToUndo()
        item = self.scene.item
        item.remove()

    def newNode(self, item, pos):
        port2 = item.port2
        pos2 = item.pos2
        c = Connection(None, self.scene)
        b = Node(None, self.scene)
        b.setPos(pos)
        self.lastNode = b
        item.port2.connections.remove(item)
        item.port2.connections.append(c)
        item.port2 = b.port_in
        item.port2.connections.append(item)
        item.pos2 = b.pos()
        c.port1 = b.port_out
        c.port2 = port2
        c.port1.connections.append(c)
        c.pos1 = b.pos()
        c.pos2 = pos2
        item.update_path()
        c.update_path()
        self.scene.isInsConn = False
        self.scene.isConnecting = True
         
    def insertConnection(self,event):
        item = self.itemAt(self.scene.evpos)
        self.newNode(item, self.scene.evpos)

    def genInterPoint(self, p2):
        p1 = self.conn.pos1
        dx = np.abs(p2.x() - p1.x())
        dy = np.abs(p2.y() - p1.y())
        if isinstance(self.conn.port1, OutPort):
            pt = QtCore.QPointF(p2.x(),p1.y())
        else:
            if dx > dy:
                pt = QtCore.QPointF(p2.x(),p1.y())
            else:
                pt = QtCore.QPointF(p1.x(),p2.y())            
        return pt

    def deleteNode(self):
        self.scene.DgmToUndo()
        self.scene.item.remove()

    def bindNode(self):
        self.scene.DgmToUndo()
        item = self.scene.item
        if len(item.port_out.connections) == 1:
            c1 = item.port_in.connections[0]
            c2 = item.port_out.connections[0]
            c = Connection(None, self.scene)
            c.pos1 = c1.pos1
            c.pos2 = c2.pos2
            item.remove()
            c.update_ports_from_pos()
            if isinstance(c.port1, OutPort) and isinstance(c.port2, InPort):
                pos = QtCore.QPointF((c.pos2.x()+c.pos1.x())/2, c.pos1.y()) 
                self.newNode(c, pos)
        else:
            self.scene.clearLastUndo()

    def newConn(self):
        self.scene.DgmToUndo()
        if not self.conn:
            self.insertConnection(self.event)
            item = self.lastNode
            self.scene.DgmToUndo()
            self.state = DRAWFROMOUTPORT
            self.scene.mainw.status.showMessage('DRAWFROMOUTPORT')
            self.conn = Connection(None, self.scene)
            self.conn.port1 = item.port_out
            self.conn.pos1 = item.scenePos()
            self.conn.pos2 = item.scenePos()
        else:
            self.scene.clearLastUndo()
            
    def deleteConn(self):
        self.scene.DgmToUndo()
        self.scene.item.remove()

    def insConn(self):
        self.scene.DgmToUndo()
        if not self.conn:
            self.insertConnection(self.event)
        else:
            self.scene.clearLastUndo()
 
    def install(self, scene):
        scene.installEventFilter(self)
        self.scene = scene

    def itemAt(self, pos):
        items =  self.scene.items(QtCore.QRectF(pos-QtCore.QPointF(DB,DB), QtCore.QSizeF(2*DB,2*DB)))
        for item in items:
            if isinstance(item, Node):
                return item
            
        for item in items:
            if isinstance(item, QGraphicsItem):
                return(item)
        return None

    def itemByDraw(self, pos):
        items =  self.scene.items(QtCore.QRectF(pos-QtCore.QPointF(DB,DB), QtCore.QSizeF(2*DB,2*DB)))
        for item in items:
            if isinstance(item, Node):
                return item
            
        for item in items:
            if isinstance(item, InPort):
                return(item)
        return None

    def itemIsPortIn(self, pos):
        items =  self.scene.items(QtCore.QRectF(pos-QtCore.QPointF(DB,DB), QtCore.QSizeF(2*DB,2*DB)))
        for el in items:
            if isinstance(el, InPort):
                return el
        return None

    def itemIsNode(self, pos):
        items =  self.scene.items(QtCore.QRectF(pos-QtCore.QPointF(DB,DB), QtCore.QSizeF(2*DB,2*DB)))
        for el in items:
            if isinstance(el, Node):
                return el
        return None
    
    def deselect_all(self):
        for el in self.scene.items():
            el.setSelected(False)

    def setMouseInitDraw(self, item):
        if isinstance(item, Block):
            pointer = QtCore.Qt.ArrowCursor
        elif isinstance(item, OutPort) and len(item.connections) == 0:
            pointer = QtCore.Qt.CrossCursor
        elif isinstance(item, Node):
            pointer = QtCore.Qt.CrossCursor
        elif isinstance(item, Connection):
            pointer = QtCore.Qt.PointingHandCursor
        else:
            pointer = QtCore.Qt.ArrowCursor
        self.scene.mainw.view.setCursor(pointer)        
            
    def setMouseByDraw(self, item):
        if isinstance(item, InPort) and len(item.connections)==0:
            pointer = QtCore.Qt.CrossCursor
        elif isinstance(item, Node) and len(item.port_in.connections) == 0:
            pointer = QtCore.Qt.CrossCursor
        else:
            pointer = QtCore.Qt.DragLinkCursor
        self.scene.mainw.view.setCursor(pointer)        
            
    def PDM(self, obj, event):    # Dummy function - No action
        pass

    def P00(self, obj, event):
        item = self.itemAt(event.scenePos())
        self.setMouseInitDraw(item)
        
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
        self.scene.currentItem = self.itemAt(event.scenePos())
        self.state = LEFTMOUSEPRESSED
        item = self.itemIsNode(event.scenePos())
        if isinstance(item, Node):
            self.oldPos = item.scenePos()
        else:
            self.oldPos = None
            
    def P02(self, obj, event):
        item = self.itemAt(event.scenePos())
        self.deselect_all()
        if isinstance(item, Block):
            item.setSelected(True)
            self.scene.item = item
            self.scene.evpos = event.scenePos()
            try:
                self.menuIOBlk.exec_(event.screenPos())
            except:
                pass
                
        if isinstance(item,Node):
            self.scene.item = item
            try:
                self.subMenuNode.exec_(event.screenPos())
            except:
                pass
                
        if isinstance(item,Connection):
            self.scene.item = item
            self.scene.evpos = event.scenePos()
            try:
                self.subMenuConn.exec_(event.screenPos())
            except:
                pass

    def P03(self, obj, event):
        item = self.itemAt(event.scenePos())
        self.deselect_all()
        if isinstance(item, Block):
            item.setSelected(True)
            self.scene.item = item
            self.scene.evpos = event.scenePos()
            self.paramsBlock()

    def P04(self, obj, event):
        items = self.scene.selectedItems()
        for item in items:
            self.scene.DgmToUndo()
            try:
                item.remove()
            except:
                pass
        self.state = IDLE

    def P05(self, obj, event):
        self.state = IDLE

    def P06(self, obj, event):
        item = self.itemAt(event.scenePos())
        if isinstance(item, Node):
            try:
                item.setPos(self.oldPos)
            except:
                pass
        if isinstance(item, Block) or isinstance(item, Node):
            self.scene.DgmToUndo()
                        
    def P07(self, obj, event):
        item = self.itemAt(event.scenePos())
        
        if self.scene.currentItem != None:
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

        if isinstance(item, OutPort):
            self.scene.DgmToUndo()
            self.state = DRAWFROMOUTPORT
            if len(item.connections) == 0:
                self.conn = Connection(None, self.scene)
                self.conn.port1 = item
                self.conn.pos1 = item.scenePos()
                self.conn.pos2 = item.scenePos()

        if  isinstance(item, Node):
            item.setPos(self.oldPos)
            self.scene.DgmToUndo()
            self.state = DRAWFROMOUTPORT
            self.conn = Connection(None, self.scene)
            self.conn.port1 = item.port_out
            self.conn.pos1 = item.scenePos()
            self.conn.pos2 = item.scenePos()
        self.scene.updateDgm()
        
    def P08(self, obj, event):
        PortOrNode = False
        
        item = self.itemIsPortIn(event.scenePos())
        if isinstance(item,InPort):
            if len(item.connections)==0:
                self.scene.DgmToUndo()
                self.conn.port2 = item
                self.conn.pos2 = item.scenePos()
                self.conn.port1.connections.append(self.conn)
                self.conn.port2.connections.append(self.conn)
                if isinstance(self.conn.port1, OutPort):
                    pos = QtCore.QPointF((self.conn.pos2.x()+self.conn.pos1.x())/2, self.conn.pos1.y()) 
                    self.newNode(self.conn, pos)
                        
                self.conn.update_path()
                self.conn = None
                self.state = IDLE
            PortOrNode = True

        item = self.itemIsNode(event.scenePos())
        if isinstance(item, Node):
            if item.port_in.connections == []:
                self.scene.DgmToUndo()
                self.conn.port2 = item.port_in
                self.conn.pos2 = item.scenePos()
                self.conn.port1.connections.append(self.conn)
                self.conn.port2.connections.append(self.conn)   # Problema qui!
                       
                self.conn.update_path()
                self.conn = None
                self.state = IDLE
            PortOrNode = True
                   
        if not PortOrNode:
            b = Node(None, self.scene)
            b.setPos(event.scenePos())
            self.conn.port2 = b.port_in
            self.conn.pos2 = b.scenePos()
            self.conn.port1.connections.append(self.conn)
            self.conn.port2.connections.append(self.conn)
            self.conn.update_path()
                
            self.conn = Connection(None, self.scene)
            self.conn.port1 = b.port_out
            self.conn.pos1 = b.scenePos()
            self.conn.pos2 = event.scenePos()             
                    
    def P09(self, obj, event):
        try:
            item = self.itemAt(event.scenePos())
        except:
            item = None
        if isinstance(item,InPort):
            if len(item.connections)==0:
                self.scene.DgmToUndo()
                self.conn.port2 = item
                self.conn.pos2 = item.scenePos()
                self.conn.port1.connections.append(self.conn)
                self.conn.port2.connections.append(self.conn)
                if isinstance(self.conn.port1, OutPort):
                    pos = QtCore.QPointF((self.conn.pos2.x()+self.conn.pos1.x())/2, self.conn.pos1.y()) 
                    self.newNode(self.conn, pos)
                        
                self.conn.update_path()
                self.conn = None
                self.state = IDLE
        else:         
            pt = self.genInterPoint(self.conn.pos2)
            b = Node(None, self.scene)
            b.setPos(pt)
            self.conn.port2 = b.port_in
            self.conn.pos2 = b.scenePos()
            self.conn.port1.connections.append(self.conn)
            self.conn.port2.connections.append(self.conn)
            self.conn.update_path()
            self.conn = None
            self.state = IDLE
       
    def P10(self, obj, event):
        item = self.itemByDraw(event.scenePos())
        self.setMouseByDraw(item)
        self.conn.pos2 = event.scenePos()
        self.conn.update_path()

    def P11(self, obj, event):
        item = self.itemAt(event.scenePos())
        if  isinstance(item, Node):
            self.scene.DmgToUndo()
            self.state = DRAWFROMOUTPORT
            self.conn = Connection(None, self.scene)
            self.conn.port1 = item.port_out
            self.conn.pos1 = item.scenePos()
            self.conn.pos2 = item.scenePos()

    def P12(self, obj, event):
        item = self.itemAt(event.scenePos())
        self.setMouseInitDraw(item)
                 
    def eventFilter(self, obj, event):
        ev = -1
        if event.type() ==  QtCore.QEvent.GraphicsSceneMouseMove:
            ev = 0
             
        if event.type() ==  QtCore.QEvent.GraphicsSceneMousePress:
            if event.button() == QtCore.Qt.LeftButton:
                ev = 1
            if event.button() == QtCore.Qt.RightButton:
                ev = 2
        
        if event.type() == QtCore.QEvent.GraphicsSceneMouseRelease:
            ev = 3
                
        if event.type() == QtCore.QEvent.GraphicsSceneMouseDoubleClick:
            ev = 4

        if event.type() == QtCore.QEvent.KeyPress:
            if event.key() == QtCore.Qt.Key_Delete:
                ev = 5
            if event.key() == QtCore.Qt.Key_Escape:
                ev = 6
        if ev != -1:
            fun = self.Fun[self.state][ ev]
            fun(obj, event)
                
        return False

    def gridPos(self, pt):
         gr = GRID
         x = gr * ((pt.x() + gr /2) // gr)
         y = gr * ((pt.y() + gr /2) // gr)
         return QtCore.QPointF(x,y)

