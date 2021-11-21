#!/bin/sh

basedir=$(dirname "$0")

ATONEDIR="$basedir/../.."
ATONE="$ATONEDIR/build/release/bin/atone"

"$ATONE" -c "$basedir/atone.yml"
