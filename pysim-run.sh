#!/bin/sh

export PYSUPSICTRL="$( cd "$(dirname "$0")" ; pwd )"

if [ $# -eq 0 ]; then
    appname="pysimCoder"
else
    case "$1" in
        -* ) appname="pysimCoder" ;;
        *.dgm ) appname="pysimCoder" ;;
        * ) appname="$(basename $1)" ; shift
    esac
fi

if [ ! -e "$PYSUPSICTRL/BlockEditor/$appname.py" ] ; then
    # find all .py applications
    applications=$(find "$PYSUPSICTRL/BlockEditor" -name "*.py" -execdir basename {} \; | sed s/.py//)
    scriptname="$(basename $0)"
    echo "Usage: $scriptname APP APPARGS"
    echo "APP is one of the following:"
    echo "$applications"
    exit 1
fi

if [ -n "$PYTHONPATH" ] ; then
  PYTHONPATH=":$PYTHONPATH"
fi

export PYTHONPATH=":$PYSUPSICTRL/resources/blocks/rcpBlk$PYTHONPATH"
export PYTHONPATH=":$PYSUPSICTRL/toolbox/supsictrl$PYTHONPATH"
export PYTHONPATH="$PYSUPSICTRL/toolbox/supsisim$PYTHONPATH"

# Prefer an active virtual environment, then the uv-managed .venv
if [ -n "$VIRTUAL_ENV" ]; then
    PYTHON_INTR="$VIRTUAL_ENV/bin/python3"
elif [ -x "$PYSUPSICTRL/.venv/bin/python3" ]; then
    # Activate it, so that python3/jupyter spawned by the app use it too
    export VIRTUAL_ENV="$PYSUPSICTRL/.venv"
    export PATH="$VIRTUAL_ENV/bin:$PATH"
    PYTHON_INTR="$VIRTUAL_ENV/bin/python3"
else
    PYTHON_INTR="/usr/bin/env python3"
fi

$PYTHON_INTR "$PYSUPSICTRL/BlockEditor/$appname.py" "$@"
