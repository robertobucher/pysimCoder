from supsisim.qtvers import *

from supsisim.block import Block
from supsisim.subsblock import subsBlock
from supsisim.port import Port, InPort, OutPort
from supsisim.connection import Connection
from supsisim.dialg import RTgenDlg, SHVDlg, UpdimgDlg
from supsisim.const import VERSION, pyrun, TEMP, respath, BWmin
from supsisim.getTemplates import dictTemplates
from supsisim.RCPblk import RcpParam
from .shv import ShvClient, SHVInstance
from lxml import etree
import os
import io
import subprocess
import time
import json
import re


IDLE = 0

class GraphicsView(QGraphicsView):
    def __init__(self, parent=None):
        super(GraphicsView, self).__init__(parent)
        self.setDragMode(QGraphicsView.DragMode.RubberBandDrag)
        self.setSceneRect(QRectF(-2000, -2000, 4000, 4000))
        self.setRenderHint(QPainter.RenderHint.Antialiasing)
        self.setRenderHint(QPainter.RenderHint.TextAntialiasing)
        self.setAcceptDrops(True)

    def wheelEvent(self, event):
        factor = 1.41 ** (-event.angleDelta().y() / 240.0)
        self.scale(factor, factor)


class Scene(QGraphicsScene):
    class UpdimgContext():
        def __init__(self, met: str, openocd_params: str):
            self.met = met
            self.openocd_params = openocd_params

    def __init__(self, main, parent=None):
        super(Scene,self).__init__(parent)
        self.mainw = main

        self.selection = []
        self.currentItem = None
        self.blocks = set()

        self.template = 'sim.tmf'
        self.intgMethod = 'standard RK4'
        self.epsAbs = '1e-6'
        self.epsRel = '1e-6'
        self.addObjs = ''
        self.addCDefs = ''
        self.addMakeArgs = ''
        self.Ts = '0.01'
        self.script = ''
        self.Tf = '10'
        self.prio = ''

        self.SHV = SHVInstance(self.mainw.filename)
        self.updimgCtx = self.UpdimgContext("openocd", "")

        self.brokerConnection = ShvClient()

        self.undoList = []

    def dragMoveEvent(self, event):
        if event.mimeData().hasText():
            event.accept()
        else:
            event.ignore()

    def dragLeaveEvent(self, event):
        return

    def dropEvent(self,event):
        if event.mimeData().hasText():
            self.DgmToUndo()
            msg = event.mimeData().text()
            blk = json.loads(msg)
            blk['pos'] = (event.scenePos().x(), event.scenePos().y())
            self.loadBlock(blk)

    def DgmToDict(self, dataDict):
        # Transform the block diagram into a python dict
        init = {'code': 'pysimCoder',
                'ver' : VERSION,
                'date' : time.strftime("%d.%m.%Y - %H:%M:%S"),
            }
        dataDict['init'] = init

        keys = ['template', 'Ts', 'AddObj', 'AddCDefs', 'AddMakeArgs', 'script', 'intgMethod', 'epsAbs', 'epsRel', 'Tf', 'prio']
        vals = [self.template, self.Ts, self.addObjs, self.addCDefs, self.addMakeArgs, self.script, self.intgMethod, self.epsAbs, self.epsRel, self.Tf, self.prio]
        dataDict['simulate'] = dict(zip(keys, vals))

        keys = ['used', 'ip', 'port', 'user', 'passwd', 'devid', 'mount', 'tree', 'updates']
        vals = [self.SHV.used, self.SHV.ip, self.SHV.port, self.SHV.user, self.SHV.passw,
                self.SHV.devid, self.SHV.mount, self.SHV.tree, self.SHV.updates]
        dataDict['SHV'] = dict(zip(keys, vals))
        keys = ['method', 'openocd_params']
        vals = [self.updimgCtx.met, self.updimgCtx.openocd_params]
        dataDict['updimg']  = dict(zip(keys, vals))

        self.saveItems(dataDict)

    def saveItems(self, dataDict):
        items = self.items()

        dgmBlocks = []
        dgmSubsystem = []
        dgmConnections = []

        for item in items:
            if isinstance(item, subsBlock):
                dgmSubsystem.append(item)
            elif isinstance(item, Block):
                dgmBlocks.append(item)
            elif isinstance(item, Connection):
                dgmConnections.append(item)
            else:
                pass

        blk = []
        for item in dgmBlocks:
            b = item.save()
            blk.append(b)
        dataDict['blocks'] = blk

        conn = []
        for item in dgmConnections:
            c = item.save()
            conn.append(c)
        dataDict['connections'] = conn

        subs = []
        for item in dgmSubsystem:
            s = item.save()
            subs.append(s)
        dataDict['subsystems'] = subs

    def clearDgm(self):
        items = self.items()
        for item in items:
            for thing in item.childItems():
                if isinstance(thing, Port):
                    for conn in thing.connections:
                        conn.remove()
            self.removeItem(item)
        self.blocks.clear()

    def getBlock(self, item):
        pos = (float(item.findtext('posX')), float(item.findtext('posY')))
        vals = [item.findtext('name'), int(item.findtext('inp')), int(item.findtext('outp')),
                item.findtext('inset')=='1', item.findtext('outset')=='1', item.findtext('icon'),
                item.findtext('params'), item.findtext('help'),
                int(item.findtext('dims')), int(item.findtext('height')),item.findtext('flip')=='1', pos]
        keys = ['name', 'inp', 'outp', 'inset', 'outset', 'icon', 'params', 'help', 'dims', height, 'flip', 'pos']
        return dict(zip(keys, vals))

    def getConnection(self, item):
        pos1 = (float(item.findtext('pos1X')), float(item.findtext('pos1Y')))
        pos2 = (float(item.findtext('pos2X')), float(item.findtext('pos2Y')))
        points = item.findall('pt')
        pts = []
        for el in points:
            pt = el.text.split(',')
            pt = (float(pt[0]), float(pt[1]))
            pts.append(pt)
        keys = ['pos1', 'pos2', 'points']
        vals = [pos1, pos2, pts]
        return dict(zip(keys, vals))

    def DictToDgm(self, dataDict, dx = 0, dy = 0):
        # Transform the file dict into a block diagram
        try:
            self.template = dataDict['simulate']['template']
            try:
                self.intgMethod = dataDict['simulate']['intgMethod']
            except:
                self.intgMethod = 'standard RK4'
            try:
                self.epsAbs = dataDict['simulate']['epsAbs']
                self.epsRel = dataDict['simulate']['epsRel']
            except:
                self.epsAbs = '1.e-6'
                self.epdrel = '1e-6'
            self.Ts = dataDict['simulate']['Ts']
            self.addObjs = dataDict['simulate']['AddObj']
            self.addCDefs = dataDict['simulate']['AddCDefs']
            self.addMakeArgs = dataDict['simulate']['AddMakeArgs']
            self.script = dataDict['simulate']['script']
            self.Tf = dataDict['simulate']['Tf']
            self.prio = dataDict['simulate']['prio']
        except:
            pass

        """
        We need to access SHV field with try/except to keep support
        for older pysimCoder diagrams.
        """
        try:
            self.SHV.used = dataDict['SHV']['used']
            self.SHV.ip = dataDict['SHV']['ip']
            self.SHV.port = dataDict['SHV']['port']
            self.SHV.user = dataDict['SHV']['user']
            self.SHV.passw = dataDict['SHV']['passwd']
            self.SHV.devid = dataDict['SHV']['devid']
            self.SHV.mount = dataDict['SHV']['mount']
            self.SHV.tree = dataDict['SHV']['tree']
            self.SHV.updates = dataDict['SHV']['updates']
        except:
            pass

        try:
            self.updimgCtx.met = dataDict['updimg']['method']
            self.updimgCtx.openocd_params = dataDict['updimg']['openocd_params']
        except:
            pass

        try:
            for item in dataDict['blocks']:
                self.loadBlock(item, dx, dy)
        except:
            pass

        try:
            for item in dataDict['subsystems']:
                self.loadSubsystem(item, dx, dy)
        except:
            pass

        try:
            for item in dataDict['connections']:
                self.loadConn(item, dx, dy)
        except:
            pass

        try:
            self.mainw.editor.redrawNodes()
        except:
            pass

    def loadBlock(self, item, dx = 0, dy = 0):
        b = Block(None, self, item['name'], item['inp'], item['outp'],
                item['inset'], item['outset'], item['icon'],
                item['params'], item['help'], item['dims'], item['flip'] )

        b.setPos(item['pos'][0]+dx, item['pos'][1]+dy)

    def loadConn(self, item, dx = 0.0, dy = 0.0):
        c = Connection(None, self)
        c.load(item, dx, dy)

    def loadSubsystem(self, subs, dx = 0, dy = 0):
        item = subs['block']
        b = subsBlock(None, self, item['name'], item['inp'], item['outp'],
                item['inset'], item['outset'], item['icon'],
                item['params'], item['help'], item['dims'], item['flip'] )

        b.setPos(item['pos'][0]+dx, item['pos'][1]+dy)
        b.load(subs)

    def clearLastUndo(self):
        if len(self.undoList) > 1:
            msg = self.undoList.pop()

    def DgmToUndo(self):
        fileDict = {}
        self.mainw.modified = True
        self.DgmToDict(fileDict)
        self.undoList.append(fileDict)

    def undoDgm(self):
        if len(self.undoList) > 1:
            dgm = self.undoList.pop()
        else:
            dgm = self.undoList[0]
            self.mainw.modified = False

        self.clearDgm()
        self.DictToDgm(dgm)
        self.mainw.editor.state = IDLE

    def updateDgm(self):
        items = self.items()

        for item in items:
            if isinstance(item, Block):
                pos = item.pos()
                item.setPos(pos)

    def saveDgm(self, fname):
        fileDict = {}
        self.DgmToDict(fileDict)
        f = open(fname,'w')
        js = json.dumps(fileDict)
        f.write(js)
        f.close()

    def loadDgm(self, fname):
        f = open(fname,'r')
        msg = f.read()
        f.close()

        # ============================================================
        msg = msg.replace('width', 'dims')  # backward compatibility!
        # ============================================================

        fileDict = json.loads(msg)
        self.clearDgm()

        self.DictToDgm(fileDict)
        self.undoList = [fileDict]

    def find_itemAt(self, pos):
        items = self.items(QRectF(pos-QPointF(1,1), QSizeF(3,3)))
        for item in items:
            if isinstance(item, QGraphicsItem) and not isinstance(item, Connection):
                return item
        return None

    def setParamsBlk(self):
        self.mainw.paramsBlock()

    def codegenDlg(self):
        dialog = RTgenDlg(self)
        ind = dialog.template.findText(self.template)
        dialog.template.setCurrentIndex(ind)
        template = str(dialog.template.currentText())
        dialog.intgMethod.clear()
        dialog.intgMethod.addItems(dictTemplates[template])
        ind = dialog.intgMethod.findText(self.intgMethod)
        dialog.intgMethod.setCurrentIndex(ind)
        dialog.epsAbs.setText(self.epsAbs)
        dialog.epsRel.setText(self.epsRel)
        dialog.addObjs.setText(self.addObjs)
        dialog.addCDefs.setText(self.addCDefs)
        dialog.addMakeArgs.setText(self.addMakeArgs)
        dialog.Ts.setText(self.Ts)
        dialog.parscript.setText(self.script)
        dialog.Tf.setText(self.Tf)
        dialog.prio.setText(self.prio)
        res = dialog.exec()
        if res != 1:
            return

        self.template = str(dialog.template.currentText())
        self.intgMethod = str(dialog.intgMethod.currentText())
        self.epsAbs = str(dialog.epsAbs.text())
        self.epsRel = str(dialog.epsRel.text())
        self.addObjs = str(dialog.addObjs.text())
        self.addCDefs = str(dialog.addCDefs.text())
        self.addMakeArgs = str(dialog.addMakeArgs.text())
        self.Ts = str(dialog.Ts.text())
        self.script = str(dialog.parscript.text())
        self.prio =  str(dialog.prio.text())
        self.Tf = str(dialog.Tf.text())

    def SHVSetDlg(self):
        dialog = SHVDlg(self)
        dialog.SHVused.setChecked(self.SHV.used)
        dialog.SHVtune.setChecked(self.SHV.tuned)
        dialog.SHVUpdates.setChecked(self.SHV.updates)
        dialog.SHVip.setText(self.SHV.ip)
        dialog.SHVport.setText(self.SHV.port)
        dialog.SHVuser.setText(self.SHV.user)
        dialog.SHVpassw.setText(self.SHV.passw)
        dialog.SHVdevid.setText(self.SHV.devid)
        dialog.SHVmount.setText(self.SHV.mount)
        dialog.SHVtree.setCurrentText(self.SHV.tree)
        res = dialog.exec()
        if res != 1:
            return

        self.SHV.used = dialog.SHVused.isChecked()
        self.SHV.tuned = dialog.SHVtune.isChecked()
        self.SHV.ip = str(dialog.SHVip.text())
        self.SHV.port = str(dialog.SHVport.text())
        self.SHV.user = str(dialog.SHVuser.text())
        self.SHV.passw = str(dialog.SHVpassw.text())
        self.SHV.devid = str(dialog.SHVdevid.text())
        self.SHV.mount = str(dialog.SHVmount.text())
        self.SHV.tree = str(dialog.SHVtree.currentText())
        self.SHV.updates = dialog.SHVUpdates.isChecked()

        if not self.SHV.tuned and self.brokerConnection is not None:
            self.brokerConnection.disconnect()

    def updimgDlg(self):
        # REVISIT: each makefile should define the executable filename
        if self.template == "nuttx_timerhook.tmf" or \
           self.template == "nuttx_systemtickhook.tmf" or \
           self.template == "nuttx.tmf":
            upd_filename = "./" + self.mainw.filename + ".nximg"
        else:
            upd_filename = self.mainw.filename

        dialog = UpdimgDlg(upd_filename, self.SHV, parent=self)
        dialog.upd_met_combo.addItem("openocd")
        if self.SHV.updates == True:
            dialog.upd_met_combo.addItem("SHV File Node Update")

        # Reset the dialog's context
        dialog.upd_met_combo.setCurrentIndex(0)
        _idx = dialog.upd_met_combo.findText(self.updimgCtx.met)
        if _idx >= 0:
            dialog.upd_met_combo.setCurrentIndex(_idx)
        dialog.openocd_params_edit.setText(self.updimgCtx.openocd_params)

        res = dialog.exec()
        if res != 1:
            return

        self.updimgCtx.met = dialog.upd_met_combo.currentText()
        self.updimgCtx.openocd_params = dialog.openocd_params_edit.text()

    def findAllItems(self, scene):
        items = []
        for item in scene.items():
            if isinstance(item, subsBlock):
                blk = item.getInternalBlocks()
                for el in blk:
                    el.setSysPath(f'/{item.name}')
                    items.append(el)
            elif isinstance(item, Block):
                item.setSysPath('')
                items.append(item)

            else:
                pass

        items.sort(key=lambda p: p.name)
        count = 0
        for el in items:
            if el.params != 'IOBlk':
                if el.ident == -1:
                    el.ident = count
                    count = count + 1

        return items

    def cleanBlkList(self, items):
        item2rem = []
        for item in items:
            if item.params == 'IOBlk':
                item2rem.append(item)
                pin, pout = item.getPorts()
                pInSub, pOutSub = item.subsParent.getPorts()
                for p in pin:
                    # This is a subsystem output!
                    n = int(item.name.lstrip('out_'))
                    for c in p.connections:
                        prev_port = c.port1
                        portSub =  pOutSub[n-1]
                        for cs in portSub.connections:
                            newConn = Connection(None, item.scene)
                            newConn.port1 = c.port1
                            newConn.pos1 = c.pos1
                            newConn.port2 = cs.port2
                            newConn.pos2 = cs.pos2
                            prev_port.connections.append(newConn)
                            try:
                                prev_port.connections.remove(c)
                            except:
                                pass
                            next_port = cs.port2
                            next_port.connections.append(newConn)
                            try:
                                next_port.connections.remove(cs)
                            except:
                                pass
                            item.scene.removeItem(newConn)

                for p in pout:
                    # This is a subsystem input!
                    n = int(item.name.lstrip('in_'))
                    for c in p.connections:
                        next_port = c.port2
                        portSub =  pInSub[n-1]
                        for cs in portSub.connections:
                            newConn = Connection(None, item.scene)
                            newConn.port1 = cs.port1
                            newConn.pos1 = cs.pos1
                            newConn.port2 = c.port2
                            newConn.pos2 = c.pos2
                            next_port.connections.append(newConn)
                            try:
                                next_port.connections.remove(c)
                            except:
                                pass
                            prev_port = cs.port1
                            prev_port.connections.append(newConn)
                            try:
                                prev_port.connections.remove(cs)
                            except:
                                pass
                            item.scene.removeItem(newConn)

        for item in item2rem:
            items.remove(item)

        return items

    def codegen(self, flag):
        def _parseAddCDefs(addCDefs: str) -> str:
            ret = ''
            # the splitter is a comma, then check the syntax
            addCDefs = addCDefs.strip()
            if addCDefs == '':
                # empty
                return ret
            args = list(addCDefs.split(','))
            for a in args:
                if a.count('=') != 1:
                    raise ValueError("Bad format of additional build args!")
                left, right = a.split('=')
                # delete trailing whitespaces
                left = left.strip()
                right = right.strip()
                if not re.fullmatch("[0-9a-zA-Z_]+", left):
                    # the macro contains forbidden characters
                    raise ValueError("Bad format of additional build args!")
                ret += "\\\'-D" + str(left) + '=' + str(right) + "\\\' "
            return ret

        # REVISIT: might check all passed arguments in the dialog.

        try:
            self.parsedAddCDefs = _parseAddCDefs(self.addCDefs)
        except ValueError as e:
            print(e)
            self.mainw.statusLabel.setText('Error by Code generation!')
            return False

        dgmBlocks = self.findAllItems(self)

        # Clean Subsystems and reattach
        dgmBlocks = self.cleanBlkList(dgmBlocks)
        try:
            nid = 1
            for item in dgmBlocks:
                for thing in item.childItems():
                    if isinstance(thing, OutPort):
                        thing.nodeID = str(nid)
                        nid += 1

            for item in dgmBlocks:
                for thing in item.childItems():
                    if isinstance(thing, InPort):
                        try:
                            c = thing.connections[0]
                        except:
                            print('Problem in diagram: input signals probably not connected!')
                        while not isinstance(c.port1, OutPort):
                            try:
                                c = c.port1.parent.port_in.connections[0]
                            except (AttributeError, ValueError):
                                raise ValueError('Problem in diagram: outputs connected together!')
                        thing.nodeID = c.port1.nodeID

            self.generateCCode(dgmBlocks)

            self.mainw.statusLabel.setText('Code generation OK!')
            try:
                os.mkdir('./' + self.mainw.filename + '_gen')
            except:
                pass
            if flag:
                cmd = pyrun + ' tmp.py'
                try:
                    subprocess.run(cmd, shell=True, check=True)
                except subprocess.CalledProcessError:
                    self.mainw.statusLabel.setText("Failed to compile generated code!")
                    del(dgmBlocks)
                    return False

            # Reset block diagram to previous state
            del(dgmBlocks)
            return True

        except:
            self.mainw.statusLabel.setText('Error by Code generation!')
            return False

    def blkInstance(self, item):
        def _recheck_param(tocheck) -> RcpParam.Type:
            """
            Rechecks UNKNOWN parameters. This function is present
            in case older diagrams are opened.
            """

            tocheck = str(tocheck).replace(' ', '')

            # This one is easy.
            if (tocheck[0] == '"' and tocheck[-1] == '"'):
                return RcpParam.Type.STR
            if (tocheck[0] == "'" and tocheck[-1] == "'"):
                return RcpParam.Type.STR

            # Some old parameters include number arrays.
            # Check it.
            if (tocheck[0] == '[' and tocheck[-1] == ']'):
                numbers = [n.replace(' ', '') for n in tocheck[1:-1].split(',')]
                cnt = 0
                force_double = False
                for n in numbers:
                    try:
                        _ = int(n)
                        cnt += 1
                    except ValueError:
                        try:
                            _ = float(n)
                            force_double = True
                            cnt += 1
                        except ValueError:
                            pass
                # All array members are numbers.
                if cnt == len(numbers):
                    if force_double:
                        return RcpParam.Type.DOUBLE
                    else:
                        return RcpParam.Type.INT

            return RcpParam.Type.UNKNOWN

        ln = item.params.split('|')
        blk_name: str = item.getCodeName().replace(' ','_')
        txt = blk_name + ' = ' + ln[0] + '('
        if item.inp != 0:
            inp = '['
            for thing in item.childItems():
                if isinstance(thing, InPort):
                    inp += thing.nodeID +','
            inp = inp.rstrip(',') + ']'
            txt += inp + ','

        if item.outp != 0:
            outp = '['
            for thing in item.childItems():
                if isinstance(thing, OutPort):
                    outp += thing.nodeID +','
            outp = outp.rstrip(',') + ']'
            txt += outp +','
        txt = txt.rstrip(',')
        N = len(ln)
        parArr = []
        for n in range(1,N):
            shv_flag: RcpParam.ShvFlag = RcpParam.ShvFlag.HIDDEN
            par = ln[n].split(':')
            try:
                match str(par[2]).replace(' ', ''):
                    case "double":
                        param_type = RcpParam.Type.DOUBLE
                        shv_flag = RcpParam.ShvFlag.EDITABLE
                    case "int" | "integer":
                        param_type = RcpParam.Type.INT
                    case "str" | "string":
                        param_type = RcpParam.Type.STR
                    case "bool" | "boolean":
                        param_type = RcpParam.Type.BOOL
                    case _:
                        param_type = _recheck_param(par[1])
            except:
                param_type = _recheck_param(par[1])

            parArr.append((par[0], par[1], param_type, shv_flag))

        # Check if Block is PLOT
        if ln[0] == 'plotBlk':
            txt += ", '" + item.getCodeName().replace(' ','_') + "'"

        txt_param: str | None = None
        if parArr:
            txt += f", {blk_name}_params"
            txt_param = f"{blk_name}_params = ["
            for param in parArr:
                txt_param += f"RcpParam('{param[0]}', {param[1]}, {param[2]}, {param[3].value}), "
            txt_param += ']\n'

            txt_param += f"for param in {blk_name}_params:\n"
            txt_param += "    if isinstance(param.value, list):\n"
            txt_param += "        param.is_list = True\n"
            txt_param += "        param.shv_flags = 0"

        txt += ')'
        txt = txt.replace('(, ', '(')
        return txt, txt_param

    def generateCCode(self, items):
        try:
            f = open(self.script,'r')
            txt = f.read()
            f.close()
            txt += '\n'
        except:
            txt = ''

        dir1 = respath + 'blocks/rcpBlk'
        txt += 'import os\n\n'

        txt += 'from supsisim.RCPblk import RCPblk, RcpParam\n\n'
        txt += 'dirs = os.listdir("' + dir1 + '")\n'
        txt += 'for el in dirs:\n'
        txt += '    files = os.listdir("' + dir1 + '" + "/" + el)\n'
        txt += '    for f in files:\n'
        txt += '        if not f.endswith(".py"):\n'
        txt += '            continue\n'
        txt += '        f = f.rstrip("*.py")\n'
        txt += '        if True: #try:\n'
        txt += '            cmd = "from " + el + "." + f + " import *"\n'
        txt += '            exec(cmd)\n'
        txt += '        #except:\n'
        txt += '        #    print("import of block class failed " + cmd)\n'
        txt += '        #    pass\n'

        txt += 'from supsisim.RCPgen import *\n'
        txt += 'from control import *\n'

        blkList = []
        sysPathList = []
        for item in items:
            if isinstance(item, Block):
                blkList.append(item.getCodeName().replace(' ','_'))
                blk_text, param_text = self.blkInstance(item)
                if param_text is not None:
                    txt += param_text + '\n'
                txt += blk_text + '\n\n'

        fname = self.mainw.filename
        with open('tmp.py', 'w') as fn:
            fn.write(txt + '\n')

            for item in blkList:
                fn.write(item + '.name = \'' + item + '\'\n')

            txt = '\nblks = ['
            for item in blkList:
                txt += item + ','
            txt += ']\n'
            fn.write(txt)

            txt = 'sysPath = '+ str(sysPathList) + '\n\n'
            txt += 'tmp = 0\n'
            txt += 'for item in sysPath:\n'
            txt += '  blks[tmp].sysPath = item\n'
            txt += '  tmp += 1\n\n'
            fn.write(txt)

            txt =  'os.environ["SHV_USED"] = \"' + str(self.SHV.used) + '\"\n'
            txt += 'os.environ["SHV_BROKER_IP"] = \"' + self.SHV.ip + '\"\n'
            txt += 'os.environ["SHV_BROKER_PORT"] = \"' + self.SHV.port + '\"\n'
            txt += 'os.environ["SHV_BROKER_USER"] = \"' + self.SHV.user + '\"\n'
            txt += 'os.environ["SHV_BROKER_PASSWORD"] = \"' + self.SHV.passw + '\"\n'
            txt += 'os.environ["SHV_BROKER_DEV_ID"] = \"' + self.SHV.devid + '\"\n'
            txt += 'os.environ["SHV_BROKER_MOUNT"] = \"' + self.SHV.mount + "/" + self.SHV.devid + '\"\n'
            txt += 'os.environ["SHV_TREE_TYPE"] = \"' + self.SHV.tree + '\"\n\n'
            txt +=  'os.environ["SHV_UPDATES_USED"] = \"' + str(self.SHV.updates) + '\"\n'
            fn.write(txt)

            fnm = './' + fname + '_gen'

            fn.write('fname = ' + "'" + fname + "'\n")
            fn.write('os.chdir("'+ fnm +'")\n')
            fn.write('genCode(fname, ' + self.Ts + ', blks, ' + "'" + self.intgMethod + "', " + \
                    self.epsAbs + ', ' + self.epsRel +')\n')
            fn.write("genMake(fname, '" + self.template + "', addObj = '" +
                  self.addObjs + "', addCDefs = '" + self.parsedAddCDefs + "')\n")
            fn.write('\nimport os\n')
            fn.write('os.system("make clean")\n')
            fn.write('if (os.system("make")) != 0:\n')
            fn.write('  raise RuntimeError("C code compilation failed")\n')
            fn.write('os.chdir("..")\n')

    def simrun(self):
        if self.codegen(False):
            cmd  = '\n'
            cmd += 'import matplotlib.pyplot as plt\n'
            cmd += 'import numpy as np\n\n'
            prio = self.prio.replace(' ','')
            if prio != '':
                prio = ' -p ' + prio
            cmd += 'os.system("./' + self.mainw.filename + prio + ' -f ' + self.Tf + '")\n'
            fnm = self.mainw.filename
            cmd += 'os.system("' + 'rm -r ' + fnm + ' ' + fnm + '_gen' + '" )'

            try:
                os.mkdir(TEMP + '/' + self.mainw.filename + '_gen')
            except:
                pass
            f = open('tmp.py','a')
            f.write(cmd)
            f.close()
            cmd = pyrun + ' tmp.py'
            try:
                os.system(cmd)
                self.mainw.statusLabel.setText('Simulation finished')
            except:
                pass

    def debugInfo(self):
        items = self.items()
        dgmBlocks = []
        dgmConnections = []
        for item in items:
            if isinstance(item, Block):
                dgmBlocks.append(item)
            elif isinstance(item, Connection):
                dgmConnections.append(item)
            else:
                pass
        print('Blocks:')
        for item in dgmBlocks:
            print(item)
        print('\nConnections:')
        for item in dgmConnections:
            print(item)

    def getBrokerConnection(self) -> ShvClient:

        shv = self.SHV
        self.brokerConnection.update_parameters_and_connect(shv.ip, shv.port, shv.user, shv.passw, shv.devid, shv.mount)

        return self.brokerConnection

    def mousePressEvent(self, event):
        self.pos1 = event.scenePos()
        super(Scene,self).mousePressEvent(event)

    def mouseReleaseEvent(self, event):
        items = self.selectedItems()
        try:
            rect = QRectF(self.pos1, event.scenePos())
            for el in items:
                if isinstance(el, Connection):
                    if el.connInSelection(rect):
                        el.setSelected(True)
                    else:
                        el.setSelected(False)
        except:
            pass

        super(Scene,self).mouseReleaseEvent(event)
