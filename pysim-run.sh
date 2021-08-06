#!/bin/sh

export PYSUPSICTRL="$( cd "$(dirname "$0")" ; pwd )"

if [ -n "$PYTHONPATH" ] ; then
  PYTHONPATH=":$PYTHONPATH"
fi
export PYTHONPATH="$PYSUPSICTRL/resources/blocks/rcpBlk:$PYSUPSICTRL/toolbox$PYTHONPATH"

if true ; then
  #export PYTHONPATH="$PYSUPSICTRL/toolbox/supsisim/src:$PYSUPSICTRL/toolbox/supsictrl/src:$PYTHONPATH"
  PYSUPSICTRL_LINKS="$HOME/Documents/PYTHON/toolbox"
  mkdir -p "$PYSUPSICTRL_LINKS"
  rm -f "$PYSUPSICTRL_LINKS/supsisim"
  ln -s "$PYSUPSICTRL/toolbox/supsisim/src" "$PYSUPSICTRL_LINKS/supsisim"
  rm -f "$PYSUPSICTRL_LINKS/supsictrl"
  ln -s "$PYSUPSICTRL/toolbox/supsictrl/src" "$PYSUPSICTRL_LINKS/supsictrl"
  export PYTHONPATH="$PYSUPSICTRL_LINKS:$PYTHONPATH"
fi

/usr/bin/python3 "$PYSUPSICTRL/BlockEditor/pysimCoder.py" "$@"
