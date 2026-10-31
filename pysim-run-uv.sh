#!/usr/bin/env sh

if ! [ -z "$PYSUPSICTRL" ]; then
    export PYSUPSICTRL="$( cd "$(dirname "$0")" ; pwd )"
fi

function show_help
{
    # find all .py applications
    applications=$(find "$PYSUPSICTRL/BlockEditor" -name "*.py" -execdir basename {} \; | sed s/.py//)
    scriptname="$(basename $0)"
    echo "Usage: $scriptname [APP APPARGS | *.dgm]"
    echo "APP is one of the following:"
    echo "$applications"
    echo "If a *.dgm file is specified, pysimCoder is run."
    exit 1
}

if [ $# -eq 0 ]; then
    appname="pysimCoder"
else
    case "$1" in
        -h) show_help ; exit 1 ;; 
        -* ) appname="pysimCoder" ;;
        *.dgm ) appname="pysimCoder" ;;
        * ) appname="$(basename $1)" ; shift
    esac
fi

if [ ! -e "$PYSUPSICTRL/BlockEditor/$appname.py" ] ; then
    show_help
fi

uv sync --project "$PYSUPSICTRL"
uv run --project "$PYSUPSICTRL" "$PYSUPSICTRL/BlockEditor/$appname.py" "$@"
