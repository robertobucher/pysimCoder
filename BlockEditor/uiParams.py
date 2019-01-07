# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'SendPars.ui'
#
# Created: Thu Dec 18 07:07:39 2014
#      by: PyQt4 UI code generator 4.11.2
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    def _fromUtf8(s):
        return s

try:
    _encoding = QtGui.QApplication.UnicodeUTF8
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig, _encoding)
except AttributeError:
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig)

class Ui_Dialog(object):
    def setupUi(self, Dialog):
        Dialog.setObjectName(_fromUtf8("Dialog"))
        Dialog.resize(440, 413)
        self.gridLayoutWidget = QtGui.QWidget(Dialog)
        self.gridLayoutWidget.setGeometry(QtCore.QRect(20, 60, 391, 271))
        self.gridLayoutWidget.setObjectName(_fromUtf8("gridLayoutWidget"))
        self.gridLayout = QtGui.QGridLayout(self.gridLayoutWidget)
        self.gridLayout.setMargin(0)
        self.gridLayout.setObjectName(_fromUtf8("gridLayout"))
        self.pbSend = QtGui.QPushButton(Dialog)
        self.pbSend.setGeometry(QtCore.QRect(160, 350, 83, 24))
        self.pbSend.setObjectName(_fromUtf8("pbSend"))
        self.spVars = QtGui.QSpinBox(Dialog)
        self.spVars.setGeometry(QtCore.QRect(220, 11, 121, 31))
        self.spVars.setMinimum(1)
        self.spVars.setMaximum(12)
        self.spVars.setObjectName(_fromUtf8("spVars"))
        self.label = QtGui.QLabel(Dialog)
        self.label.setGeometry(QtCore.QRect(100, 20, 91, 16))
        self.label.setObjectName(_fromUtf8("label"))

        self.retranslateUi(Dialog)
        QtCore.QMetaObject.connectSlotsByName(Dialog)

    def retranslateUi(self, Dialog):
        Dialog.setWindowTitle(_translate("Dialog", "Dialog", None))
        self.pbSend.setText(_translate("Dialog", "SEND", None))
        self.label.setText(_translate("Dialog", "Variables", None))

