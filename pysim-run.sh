#!/bin/sh

export PYSUPSICTRL="$( cd "$(dirname "$0")" ; pwd )"

if [ -n "$PYTHONPATH" ] ; then
  PYTHONPATH=":$PYTHONPATH"
fi

export PYTHONPATH=":$PYSUPSICTRL/resources/blocks/rcpBlk$PYTHONPATH"
export PYTHONPATH=":$PYSUPSICTRL/toolbox/supsictrl$PYTHONPATH"
export PYTHONPATH="$PYSUPSICTRL/toolbox/supsisim$PYTHONPATH"

/usr/bin/python3 "$PYSUPSICTRL/BlockEditor/pysimCoder.py" "$@"
