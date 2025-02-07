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

/usr/bin/python3 "$PYSUPSICTRL/BlockEditor/$appname.py" "$@"
