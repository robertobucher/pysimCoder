#!/usr/bin/python3

import argparse
import os
import sys

from supsisim.library import Library
from supsisim.pyEdit import NewEditorMainWindow
from supsisim.qtvers import *


def parse_args() -> tuple[argparse.Namespace, list]:
    """Parse passed arguments and return result."""
    parser = argparse.ArgumentParser("pysimCoder", description="pysimCoder")
    parser.add_argument(
        "-g",
        "--generate",
        action="store_true",
        help="Generate the code without opening diagram",
    )
    parser.add_argument(
        "FILENAME",
        nargs="?",
        default=None,
        help="Name of the file to be opened. Leaving this argument opens empty diagram.",
    )
    return parser.parse_known_args()


if __name__ == "__main__":
    args, unknown = parse_args()
    filename = args.FILENAME

    if filename is not None:
        if filename[-4:] != ".dgm":
            filename = filename + ".dgm"
        if not os.path.isfile(filename):
            filename = "untitled"
    else:
        filename = "untitled"

    if args.generate:
        if filename == "untitled":
            raise NameError("You must enter a valid pysimCoder digram file")
        app = QApplication(unknown)

        fname = str(QFileInfo(filename).baseName())
        main = NewEditorMainWindow(fname, QFileInfo(filename).absolutePath(), None)
        ret = main.codegenAct()
        app.deleteLater()
        os.remove("tmp.py")
        if not ret:
            sys.exit("Failed to generate C code")
    else:
        app = QApplication(unknown)
        screen = app.primaryScreen()
        rect = screen.availableGeometry()
        h = int(rect.height() * 0.9)

        library = Library()
        library.setGeometry(0, 0, 400, h)
        library.setMaximumWidth(800)
        library.setMinimumWidth(400)
        library.show()
        library.setWindowState(Qt.WindowState.WindowMaximized)

        if filename == "untitled":
            library.newFile()
            library.setWindowState(Qt.WindowState.WindowMaximized)
        else:
            library.fopen(filename)
            library.setWindowState(Qt.WindowState.WindowMinimized)
        ret = app.exec()
        app.deleteLater()

