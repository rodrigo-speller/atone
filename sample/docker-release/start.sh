#!/bin/sh
basedir="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

# build
docker build . -t atone > /dev/null  || exit 1

# run
docker run -it \
  -v $basedir/../config-file:/srv \
  -v $basedir/atone.yaml:/etc/atone.yaml \
  atone "$@"
