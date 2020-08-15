import os
import sys

print("Trying to install modules for ubuntu")

# install some dependencies
APT_GET_INSTALL = "apt-get install -y"
APT_GET_BIBS = [
                "make",
                "curl",
                "gfortran",
                "liblapack-dev",
                "libopenblas-dev",
               ]

if 3 == sys.version_info[0]:
    PYTHON = "python3"
    APT_GET_BIBS += [
        PYTHON + "-dev",
        PYTHON + "-distutils",
        PYTHON + "-testresources",
        PYTHON + "-pyqt5",
        PYTHON + "-pyqt5.qtsvg",
        PYTHON + "-dbus.mainloop.qt",
        PATHON + "-pyqtgraph",
        PATHON + "-serial",
        PYTHON + "-lxml",
        PYTHON + "-setuptools",
        PYTHON + "-simplejson",
        PYTHON + "-skimage",
        PYTHON + "-skimage-lib",
        PYTHON + "-tk",
    ]
else:
    APT_GET_BIBS += [
        "python-dev",
        "python-distutils",
        "python-testresources",
        "python-scipy",
        "python-qt5",
        "python-qt4-dbus",
        "python-tk",
    ]

print(os.system("apt-get update"))
for module in APT_GET_BIBS:
    print("trying to install " + module)
    print(os.system(" ".join((APT_GET_INSTALL, module))))

# install pip
print(os.system('curl "https://bootstrap.pypa.io/get-pip.py" '
                + '| sudo ' + sys.executable))
