#!/bin/sh

if [ $# -eq 0 ]; then
    appname="pysimCoder"
else
    # find all .py applications
    applications=$(find BlockEditor -name "*.py" -execdir basename {} \; | sed s/.py//)
    appname="$1"

    # match whole words only
    if ! echo $applications | grep -w "$appname" > /dev/null 2>&1 ; then
        scriptname=$(basename $0)
        echo "Usage: $scriptname APP APPARGS"
        echo "APP is one of the following:"
        echo "$applications"
        exit 1
    fi

    # only APP arguments
    shift
fi

export PYSUPSICTRL="$( cd "$(dirname "$0")" ; pwd )"

if [ -n "$PYTHONPATH" ] ; then
  PYTHONPATH=":$PYTHONPATH"
fi

export PYTHONPATH=":$PYSUPSICTRL/resources/blocks/rcpBlk$PYTHONPATH"
export PYTHONPATH=":$PYSUPSICTRL/toolbox/supsictrl$PYTHONPATH"
export PYTHONPATH="$PYSUPSICTRL/toolbox/supsisim$PYTHONPATH"

/usr/bin/python3 "$PYSUPSICTRL/BlockEditor/$appname.py" "$@"
