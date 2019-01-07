import sys
import subprocess

# assumes you got pip installed for the same python version you have 
# want to use for the tool chain
PIP_INSTALL = "-m pip install"
PIP_BIBS = [
            "setuptools",
            "qtconsole", # not crucial
            "numpy",
            "control",
            "matplotlib",
            "slycot",
            "sympy",
            "pyqtgraph",
            "lxml",
           ]
if 3 == sys.version_info[0]:
	PIP_BIBS.append("tk-tools")
	PIP_BIBS.append("scipy")

for module in PIP_BIBS:
    sys.stdout.write("trying to install " + module)
    sys.stdout.flush()
    try:
        __import__(module.replace('-', '_'))
    except:
        pip_ret = subprocess.call([sys.executable,\
                                   "-m", "pip", "install", module])
        sys.stdout.write("\n" + str(pip_ret) + "\n")
    else:
        sys.stdout.write(" ~> %s already installed\n" % module)
    sys.stdout.flush()
