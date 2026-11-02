import os
import sys
import platform
import tempfile

GRID = 10
PW = 6

BWmin = 80
BHmin = 60
BHstep = 20

PD = 40
NW = 4
LW = 2
DP = 100.0

DB = 4

VERSION = 0.95

# Host system detection
SYSTEM = platform.system()      # 'Linux', 'Darwin' or 'Windows'
IS_LINUX = SYSTEM == 'Linux'
IS_MACOS = SYSTEM == 'Darwin'
IS_WINDOWS = SYSTEM == 'Windows'

# Host dependent variables, paths, executables
SEP = os.sep                    # path component separator
PATHSEP = os.pathsep            # separator of PATH-like lists
EXE_SUFFIX = '.exe' if IS_WINDOWS else ''
SHLIB_SUFFIX = '.dll' if IS_WINDOWS else '.dylib' if IS_MACOS else '.so'
MAKE = os.environ.get('MAKE', 'make')  # make tool, overridable via $MAKE
TMPDIR = tempfile.gettempdir()

# Install paths (PYSUPSICTRL is set by pysim-run.sh), with trailing separator
path = os.path.join(os.path.normpath(os.environ['PYSUPSICTRL']), '')
respath = os.path.join(path, 'resources', '')

pyrun = sys.executable  # interpreter running pysimCoder

