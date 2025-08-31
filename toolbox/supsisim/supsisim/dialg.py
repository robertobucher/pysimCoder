from supsisim.qtvers import *

from supsisim.const import path
from supsisim.getTemplates import dictTemplates
from supsisim.image_update import ImageUpdateMethod, OpenocdUpdateMethod, SHVUpdateMethod
from supsisim.image_confirm import ImageConfirmMethod, SHVConfirmMethod
from supsisim.shv.client import ShvFwUpdateClient
from supsisim.shv.SHVInstance import SHVInstance
from threading import Event
from typing import Any, Callable, Optional
from os import listdir
import asyncio

class IO_Dialog(QDialog):
    def __init__(self,parent=None):
        super(IO_Dialog, self).__init__(parent)
        layout = QGridLayout()
        self.setWindowModality(Qt.WindowModality.ApplicationModal)
        self.resize(380, 180)
        self.spbInput = QSpinBox()
        self.spbOutput = QSpinBox()
        self.spbInput.setValue(1)
        self.spbOutput.setValue(1)

        label2 = QLabel('Number of inputs:')
        label3 = QLabel('Number of outputs')
        self.pbOK = QPushButton('OK')
        self.pbCANCEL = QPushButton('CANCEL')
        layout.addWidget(self.spbInput,0,1)
        layout.addWidget(self.spbOutput,1,1)
        layout.addWidget(label2,0,0)
        layout.addWidget(label3,1,0)
        layout.addWidget(self.pbOK,2,0)
        layout.addWidget(self.pbCANCEL,2,1)
        self.setLayout(layout)
        self.pbOK.clicked.connect(self.accept)
        self.pbCANCEL.clicked.connect(self.reject)

class BlockName_Dialog(QDialog):
    def __init__(self,parent=None):
        super(BlockName_Dialog, self).__init__(parent)
        layout = QGridLayout()
        self.setWindowModality(Qt.WindowModality.ApplicationModal)
        self.resize(380, 100)
        self.name = QLineEdit()

        label1 = QLabel('Block ID:')
        self.pbOK = QPushButton('OK')
        self.pbCANCEL = QPushButton('CANCEL')
        layout.addWidget(label1,0,0)
        layout.addWidget(self.name,0,1)
        layout.addWidget(self.pbOK,2,0)
        layout.addWidget(self.pbCANCEL,2,1)
        self.setLayout(layout)
        self.pbOK.clicked.connect(self.accept)
        self.pbCANCEL.clicked.connect(self.reject)

class RTgenDlg(QDialog):
    def __init__(self, parent=None):
        super(RTgenDlg, self).__init__(None)
        self.setWindowModality(Qt.WindowModality.ApplicationModal)
        self.resize(600, 100)

        lab1 = QLabel('Template Makefile')
        self.template = QComboBox()
        self.template.addItems(listdir(path+'CodeGen/templates'))
        self.template.setEditable(False)

        lab2 = QLabel('Parameter script')
        self.parscript = QLineEdit('')
        btn_script = QPushButton('BROWSE...')

        lab3 = QLabel('Additional Objs')
        self.addObjs = QLineEdit('')
        btn_addObjs = QPushButton('BROWSE...')

        lab4 = QLabel('Additional C Defines')
        self.addCDefs = QLineEdit('')

        lab5 = QLabel('Additional Make Args')
        self.addMakeArgs = QLineEdit('')

        lab6 = QLabel('Priority')
        self.prio = QLineEdit('')

        lab7 = QLabel('Integration method')
        self.intgMethod = QComboBox()

        lab8 = QLabel('Sampling Time')
        self.Ts = QLineEdit('')

        lab9 = QLabel('Final Time')
        self.Tf = QLineEdit('')

        self.lab10 = QLabel('          eps abs')
        self.lab11 = QLabel('          eps rel')
        self.epsAbs = QLineEdit('1e-6')
        self.epsRel = QLineEdit('1e-6')

        pbOK = QPushButton('OK')
        pbCANCEL = QPushButton('CANCEL')
        grid = QGridLayout()

        grid.addWidget(lab1, 0, 0)
        grid.addWidget(self.template, 0, 1)

        grid.addWidget(lab2, 1, 0)
        grid.addWidget(self.parscript, 1, 1)
        grid.addWidget(btn_script, 1, 2)

        grid.addWidget(lab3, 2, 0)
        grid.addWidget(self.addObjs, 2, 1)
        grid.addWidget(btn_addObjs, 2, 2)

        grid.addWidget(lab4, 3, 0)
        grid.addWidget(self.addCDefs, 3, 1)

        grid.addWidget(lab5, 4, 0)
        grid.addWidget(self.addMakeArgs, 4, 1)

        grid.addWidget(lab6, 5, 0)
        grid.addWidget(self.prio, 5, 1)

        grid.addWidget(lab7, 6, 0)
        grid.addWidget(self.intgMethod, 6, 1)
        grid.addWidget(self.lab10, 5, 2)
        grid.addWidget(self.lab11, 5, 3)
        grid.addWidget(self.epsAbs, 6, 2)
        grid.addWidget(self.epsRel, 6, 3)

        grid.addWidget(lab8, 7, 0)
        grid.addWidget(self.Ts, 7, 1)

        grid.addWidget(lab9, 8, 0)
        grid.addWidget(self.Tf, 8, 1)

        grid.addWidget(pbOK, 9, 0)
        grid.addWidget(pbCANCEL, 9, 1)
        pbOK.clicked.connect(self.accept)
        pbCANCEL.clicked.connect(self.reject)
        btn_addObjs.clicked.connect(self.getObjs)
        btn_script.clicked.connect(self.getScript)
        self.template.currentIndexChanged.connect(self.templateChanged)
        self.setLayout(grid)

    def getTemplate(self):
        fname = QFileDialog.getOpenFileName(self,'Open Template Makefile',
                                                  path+'CodeGen/templates', 'Template (*.tmf)')
        fname = fname[0]
        if len(fname) != 0:
            ln = fname.split('/')
            templ = ln[-1].__str__()
            self.template.setText(templ)

    def getObjs(self):
        fname = QFileDialog.getOpenFileName(self,'Additional libraries',
                                                  '.','Dynamic libraries (*.so)')
        fname = fname[0]
        if len(fname) != 0:
            ln = fname.split('/')
            libname = ln[-1].__str__()
            self.addObjs.setText(libname)

    def getScript(self):
        fname = QFileDialog.getOpenFileName(self,'Open Python script',
                                                  '.', 'Python file (*.py)')
        fname = fname[0]
        if len(fname) != 0:
            self.parscript.setText(fname)

    def templateChanged(self, ind):
        template = str(self.template.currentText())
        self.intgMethod.clear()
        self.intgMethod.addItems(dictTemplates[template])

class SHVDlg(QDialog):
    def __init__(self, parent=None):
        super(SHVDlg, self).__init__(None)
        self.setWindowModality(Qt.WindowModality.ApplicationModal)
        self.resize(600, 100)

        lab1 = QLabel('Enable SHV protocol')
        self.SHVused = QCheckBox('')
        self.SHVused.setChecked(False)
        lab2 = QLabel('Enable tuning over SHV')
        self.SHVtune = QCheckBox('')
        self.SHVtune.setChecked(False)
        lab_shvupdates = QLabel('Turn on SHV Image Updates')
        self.SHVUpdates = QCheckBox('')
        self.SHVUpdates.setChecked(False)
        lab3 = QLabel('SHV Broker IP')
        self.SHVip = QLineEdit('')
        lab4 = QLabel('SHV Broker Port')
        self.SHVport = QLineEdit('')
        lab5 = QLabel('SHV Broker User')
        self.SHVuser = QLineEdit('')
        lab6 = QLabel('SHV Broker Password')
        self.SHVpassw = QLineEdit('')
        lab7 = QLabel('SHV Device ID')
        self.SHVdevid = QLineEdit('')
        lab8 = QLabel('SHV Device Mount Point')
        self.SHVmount = QLineEdit('')
        lab9 = QLabel('SHV Tree Type')
        self.SHVtree = QComboBox()
        self.SHVtree.addItems(['GAVL', 'GSA', 'GSA_STATIC'])

        pbOK = QPushButton('OK')
        pbCANCEL = QPushButton('CANCEL')

        grid = QGridLayout()

        grid.addWidget(lab1, 0, 0)
        grid.addWidget(self.SHVused, 0, 1)
        grid.addWidget(lab2, 1, 0)
        grid.addWidget(self.SHVtune, 1, 1)
        grid.addWidget(lab_shvupdates, 2, 0)
        grid.addWidget(self.SHVUpdates, 2, 1)
        grid.addWidget(lab3, 3, 0)
        grid.addWidget(self.SHVip, 3, 1)
        grid.addWidget(lab4, 4, 0)
        grid.addWidget(self.SHVport, 4, 1)
        grid.addWidget(lab5, 5, 0)
        grid.addWidget(self.SHVuser, 5, 1)
        grid.addWidget(lab6, 6, 0)
        grid.addWidget(self.SHVpassw, 6, 1)
        grid.addWidget(lab7, 7, 0)
        grid.addWidget(self.SHVdevid, 7, 1)
        grid.addWidget(lab8, 8, 0)
        grid.addWidget(self.SHVmount, 8, 1)
        grid.addWidget(lab9, 9, 0)
        grid.addWidget(self.SHVtree, 9, 1)
        grid.addWidget(pbOK, 10, 0)
        grid.addWidget(pbCANCEL, 10, 1)
        pbOK.clicked.connect(self.accept)
        pbCANCEL.clicked.connect(self.reject)

        self.setLayout(grid)

class UpdimgDlg(QDialog):
    class _UploadConfirmWorker(QThread):
        textbox_sig = pyqtSignal(str)
        work_done   = pyqtSignal()

        def __init__(
                self,
                parent: Any,
                logbox: QTextEdit,
                work_completed_cb: Callable):
            super().__init__(parent)
            self.logbox = logbox
            self.textbox_sig.connect(self.logbox.append)
            self.work_done.connect(work_completed_cb)
            self.task: Optional[asyncio.Task[None]] = None

            self.start_event = Event()
            self.stop_event  = Event()
            self.running = False
            self.thrd_running = True

        async def _wrapper_run(self):
            # We are running, now
            self.running = True

            queue = asyncio.Queue()
            success_str: Optional[str]
            failed_str: Optional[str]
            if isinstance(self.method, ImageUpdateMethod):
                self.task = asyncio.create_task(self.method.upload(queue))
                success_str = "Upload successful!"
                failed_str = "Upload failed!"
            elif isinstance(self.method, ImageConfirmMethod):
                self.task = asyncio.create_task(self.method.confirm(queue))
                success_str = "Confirm successful!"
                failed_str  = "Confirm failed!"
            while not self.stop_event.is_set():
                try:
                    event = await asyncio.wait_for(queue.get(), timeout=0.5)
                    if isinstance(event, str):
                        self.textbox_sig.emit(event)
                    elif isinstance(event, bool):
                        if event:
                            self.textbox_sig.emit(success_str)
                        else:
                            self.textbox_sig.emit(failed_str)
                        break
                except asyncio.TimeoutError:
                    continue
                except asyncio.CancelledError:
                    break

            try:
                await self.task
            except asyncio.CancelledError:
                self.textbox_sig.emit("Task cancelled.")
            # We are not running, now
            self.running = False
            self.stop_event.clear()

        def run(self) -> None:
            while self.thrd_running:
                self.start_event.wait()
                if not self.thrd_running:
                    break
                asyncio.run(self._wrapper_run())
                self.work_done.emit()
                self.start_event.clear()

        def do_work(self, method: ImageUpdateMethod | ImageConfirmMethod) -> None:
            if self.running:
                # Work already running, stop.
                self.work_done.emit()
                return
            self.method = method
            self.textbox_sig.emit("Operation started")
            self.start_event.set()

        def stop_work(self) -> None:
            if not self.running:
                return
            self.textbox_sig.emit("Stopping event.")
            self.stop_event.set()
            self.task.cancel()

        def stop_thrd(self) -> None:
            self.thrd_running = False
            # this effectively unlocks the loop
            self.start_event.set()

    def _on_upd_met_change(self, value):
        en: bool = False
        if value == "openocd":
            en = True

        self.openocd_params_lab.setEnabled(en)
        self.openocd_params_edit.setEnabled(en)

    def _on_start_upd_pushed(self):
        self.updinfo_box.clear()
        method: Optional[ImageUpdateMethod]
        if self.upd_met_combo.currentText() == "openocd":
            method = OpenocdUpdateMethod(self.openocd_params_edit.text(), self.path_to_img)
        else:
            method = SHVUpdateMethod(self.path_to_img, self.shvclient, self.shvparams)

        self.thrd.do_work(method)
        self.start_pb.setEnabled(False)
        self.confirm_pb.setEnabled(False)
        self.cancel_pb.setEnabled(True)

    def _on_cancel_upd_pushed(self):
        if self.thrd.isRunning():
            self.thrd.stop_work()

    def _on_confirm_img_pushed(self):
        self.updinfo_box.clear()
        method: Optonal[ImageConfirmMethod]
        if self.upd_met_combo.currentText() == "openocd":
            # No need to confirm openocd images
            return
        else:
            method = SHVConfirmMethod(self.shvclient, self.shvparams)
        self.thrd.do_work(method)
        self.start_pb.setEnabled(False)
        self.confirm_pb.setEnabled(False)
        self.cancel_pb.setEnabled(True)

    def _on_help_pushed(self):
        msg = QMessageBox()
        msg.setIcon(QMessageBox.Icon.Question)
        msg.setWindowTitle("Help")
        msg.setText(
            "Currently only supporting nuttx!!\n" +
            "Update method: choose openocd or SHV NXBoot Update.\n" +
            "When openocd is chosen, fill in the shell parameters needed to flash the device:\n" +
            "  - fill in the binary to be flashed as @PYSIM_IMG@ in the command\n" +
            "When SHV NXBoot Update is chosen, it is expected:\n" +
            "  1) you're running NuttX model with SHV turned on,\n" +
            "  2) the parameters from the SHV dialog are used to interface the updater.\n"
        )
        msg.setStandardButtons(QMessageBox.StandardButton.Ok)
        msg.setFixedSize(msg.size())
        msg.exec()

    @pyqtSlot()
    def _upload_confirm_work_completed(self):
        self.start_pb.setEnabled(True)
        self.confirm_pb.setEnabled(True)
        self.cancel_pb.setEnabled(False)

    @pyqtSlot()
    def _ok_pb_override(self):
        self._on_cancel_upd_pushed()
        self.thrd.stop_thrd()
        self.thrd.wait()
        self.accept()

    # override
    def reject(self):
        self.thrd.stop_thrd()
        self.thrd.wait()
        super().reject()

    # override
    def closeEvent(self, event):
        self.thrd.stop_thrd()
        self.thrd.wait()
        super().closeEvent(event)

    def __init__(self, path_to_img: str, shvparams: SHVInstance, parent=None):
        super(UpdimgDlg, self).__init__(None)
        self.setWindowModality(Qt.WindowModality.ApplicationModal)
        self.resize(800, 600)
        self.path_to_img = path_to_img
        self.shvclient = ShvFwUpdateClient()
        self.shvparams = shvparams
        f = QFont("Monospace")
        # Top layout
        top_layout = QWidget(self)
        grid_top = QGridLayout(top_layout)

        # Bottom buttons layout
        button_layout = QWidget(self)
        grid_button = QGridLayout(button_layout)

        # Main layout
        main_layout = QVBoxLayout(self)

        self.upd_met_lab = QLabel('Update method')
        # Do not add any items to QComboBox - that will be filled in scene
        self.upd_met_combo = QComboBox()
        # To have responsive labels
        self.upd_met_combo.currentTextChanged.connect(self._on_upd_met_change)
        self.openocd_params_lab = QLabel('Openocd shell parameters')
        self.openocd_params_edit = QLineEdit()
        self.openocd_params_edit.setFont(f)

        self.start_pb = QPushButton('Start Update')
        self.start_pb.clicked.connect(self._on_start_upd_pushed)
        self.cancel_pb = QPushButton('Cancel Operation')
        self.cancel_pb.clicked.connect(self._on_cancel_upd_pushed)
        self.cancel_pb.setEnabled(False)
        self.help_pb = QPushButton('Help')
        self.ok_pb = QPushButton('OK')
        self.confirm_pb = QPushButton('Confirm Image')

        self.help_pb.clicked.connect(self._on_help_pushed)
        self.ok_pb.clicked.connect(self._ok_pb_override)
        self.confirm_pb.clicked.connect(self._on_confirm_img_pushed)

        self.updinfo_box_lab = QLabel('Update procedure log')
        self.updinfo_box = QTextEdit()
        self.updinfo_box.setFont(f)
        self.updinfo_box.setReadOnly(True)

        grid_top.addWidget(self.upd_met_lab, 0, 0)
        grid_top.addWidget(self.upd_met_combo, 0, 1)
        grid_top.addWidget(self.openocd_params_lab, 1, 0)
        grid_top.addWidget(self.openocd_params_edit, 1, 1)

        grid_button.addWidget(self.start_pb, 0, 0)
        grid_button.addWidget(self.cancel_pb, 0, 1)
        grid_button.addWidget(self.confirm_pb, 0, 2)
        grid_button.addWidget(self.help_pb, 0, 3)
        grid_button.addWidget(self.ok_pb, 0, 4)

        self.updinfo_box.setFixedHeight(500)
        self.updinfo_box.setMinimumWidth(0)
        button_layout.setFixedHeight(50)
        button_layout.setMinimumWidth(0)
        self.updinfo_box_lab.setFixedHeight(50)
        self.updinfo_box_lab.setMinimumWidth(0)

        main_layout.addWidget(top_layout, stretch=1)
        main_layout.addWidget(self.updinfo_box_lab, stretch=0)
        main_layout.addWidget(self.updinfo_box, stretch=0)
        main_layout.addWidget(button_layout, stretch=0)

        self.setLayout(main_layout)

        self.thrd = self._UploadConfirmWorker(self, self.updinfo_box,
            self._upload_confirm_work_completed)
        self.thrd.start()
