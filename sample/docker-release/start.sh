#!/bin/sh
basename="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

# build
docker build . -t atone > /dev/null  || exit 1

# run
docker run -it \
  -v $basename/../config-file:/srv \
  -v $basename/atone.yaml:/etc/atone.yaml \
  atone "$@"
