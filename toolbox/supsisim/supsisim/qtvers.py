"""
qtvers.py
COmpatibility layer PySide6, PyQt6 e PyQt5.
Ordine di preferenza:
    1) PySide6
    2) PyQt6
    3) PyQt5
"""

# =========================
# 1. TRY: PySide6
# =========================
try:
    from PySide6 import QtCore, QtGui, QtWidgets, QtSvg, QtPrintSupport
    from PySide6.QtUiTools import QUiLoader
    import xml.etree.ElementTree as ET

    QT_LIB = "PySide6"

    # Alias compatibilità segnali
    pyqtSignal = QtCore.Signal
    pyqtSlot = QtCore.Slot

    class PySide6UicEmulator:
        @staticmethod
        def loadUiType(ui_path):
            tree = ET.parse(ui_path)
            base_class_name = tree.find('widget').get('class')
            base_class = getattr(QtWidgets, base_class_name)

            class TemplateConfig(object):
                def setupUi(self, target_widget):
                    loader = QUiLoader()
                    # Carichiamo l'UI. Il loader restituisce l'istanza del contenuto del file .ui
                    ui_instance = loader.load(ui_path)

                    # --- GESTIONE LAYOUT/CONTENUTO ---
                    if isinstance(target_widget, QtWidgets.QMainWindow):
                        # Se è una MainWindow, dobbiamo prendere il centralWidget del file .ui
                        # e spostarlo nella nostra istanza reale
                        central_widget = ui_instance.centralWidget()
                        if central_widget:
                            target_widget.setCentralWidget(central_widget)

                        # Copiamo anche menuBar, toolBar e statusBar se presenti
                        if ui_instance.menuBar():
                            target_widget.setMenuBar(ui_instance.menuBar())
                        for toolbar in ui_instance.findChildren(QtWidgets.QToolBar):
                            target_widget.addToolBar(toolbar)
                        if ui_instance.statusBar():
                            target_widget.setStatusBar(ui_instance.statusBar())
                    else:
                        # Se è un QWidget o QDialog, creiamo un layout per forzare
                        # il contenuto a riempire tutta la finestra
                        layout = QtWidgets.QVBoxLayout(target_widget)
                        layout.setContentsMargins(0, 0, 0, 0)
                        layout.addWidget(ui_instance)

                    # --- MAPPATURA ATTRIBUTI (Il punto cruciale) ---
                    # Dobbiamo rendere ogni widget (es. self.btn_ok) accessibile
                    # direttamente da target_widget
                    for name in dir(ui_instance):
                        # Evitiamo metodi privati e built-in
                        if not name.startswith('__'):
                            member = getattr(ui_instance, name)
                            # Se è un widget o un'azione, lo portiamo su target_widget
                            if isinstance(member, (QtCore.QObject, QtGui.QAction)):
                                setattr(target_widget, name, member)

            return (TemplateConfig, base_class)

    uic = PySide6UicEmulator()

# =========================
# 2 Try: PyQt6
# =========================
except ImportError:
    try:
        from PyQt6 import QtCore, QtGui, QtWidgets, QtSvg, QtPrintSupport, uic

        QT_LIB = "PyQt6"

        pyqtSignal = QtCore.pyqtSignal
        pyqtSlot = QtCore.pyqtSlot

# =========================
# 3. FALLBACK: PyQt5
# =========================
    except ImportError:
        from PyQt5 import QtCore, QtGui, QtWidgets, QtSvg, QtPrintSupport, uic

        QT_LIB = "PyQt5"

        pyqtSignal = QtCore.pyqtSignal
        pyqtSlot = QtCore.pyqtSlot

# =====================================================
#Export classes for the project
# =====================================================

# ---- QtCore ----
QEvent = QtCore.QEvent
QTimer = QtCore.QTimer
QFileInfo = QtCore.QFileInfo
QMimeData = QtCore.QMimeData
QObject = QtCore.QObject
QPointF = QtCore.QPointF
QRectF = QtCore.QRectF
QSettings = QtCore.QSettings
QSizeF = QtCore.QSizeF
# QT_VERSION_STR = QtCore.QT_VERSION_STR
try:
    QVariant = QtCore.QVariant
except AttributeError:
    QVariant = object
Qt = QtCore.Qt
QThread = QtCore.QThread

# ---- QtGui ----
QAction = QtGui.QAction
QDrag = QtGui.QDrag
QFont = QtGui.QFont
QIcon = QtGui.QIcon
QPixmap = QtGui.QPixmap
QImage = QtGui.QImage
QPainter = QtGui.QPainter
QPainterPath = QtGui.QPainterPath
QBrush = QtGui.QBrush
QPen = QtGui.QPen
QTransform = QtGui.QTransform

# ---- QtWidgets ----
QApplication = QtWidgets.QApplication
QCheckBox = QtWidgets.QCheckBox
QComboBox = QtWidgets.QComboBox
QDialog = QtWidgets.QDialog
QDialogButtonBox = QtWidgets.QDialogButtonBox
QFileDialog = QtWidgets.QFileDialog
QGraphicsItem = QtWidgets.QGraphicsItem
QGraphicsPathItem = QtWidgets.QGraphicsPathItem
QGraphicsRectItem = QtWidgets.QGraphicsRectItem
QGraphicsScene = QtWidgets.QGraphicsScene
QGraphicsTextItem = QtWidgets.QGraphicsTextItem
QGraphicsView = QtWidgets.QGraphicsView
QGridLayout = QtWidgets.QGridLayout
QHBoxLayout = QtWidgets.QHBoxLayout
QLabel = QtWidgets.QLabel
QLineEdit = QtWidgets.QLineEdit
QListWidget = QtWidgets.QListWidget
QMainWindow = QtWidgets.QMainWindow
QMenu = QtWidgets.QMenu
QMenuBar = QtWidgets.QMenuBar
QMessageBox = QtWidgets.QMessageBox
QPushButton = QtWidgets.QPushButton
QSpinBox = QtWidgets.QSpinBox
QTabWidget = QtWidgets.QTabWidget
QTableWidget = QtWidgets.QTableWidget
QTableWidgetItem = QtWidgets.QTableWidgetItem
QTextEdit = QtWidgets.QTextEdit
QVBoxLayout = QtWidgets.QVBoxLayout
QWidget = QtWidgets.QWidget

# ---- QtPrintSupport ----
QPrinter = QtPrintSupport.QPrinter
QPrintDialog = QtPrintSupport.QPrintDialog

#print(QT_LIB)


