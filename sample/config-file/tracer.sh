#!/bin/bash

echo "tracer:" "$0" "$@"

trap_signals () {
    func="$1" ; shift

    for sig ; do
        trap "$func" "$sig"
    done
}

signal_handler()
{
    echo "tracer: signal $@"
    _terminated=1
}

_terminated=0

trap_signals signal_handler INT TERM HUP KILL

while true; do

    sleep 1
    [ $_terminated -eq 1 ] && exit 0
    if [ $PPID -ne `ps -ho ppid $$ | xargs` ]; then
        echo "Parent process killed"
        exit 0
    fi

done
