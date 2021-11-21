#!/bin/sh

basename=$(dirname "$0")

ATONEDIR="$basename/../.."
ATONE="$ATONEDIR/build/release/bin/atone"

if [ ! -e "$ATONE" ]; then
    make -C "$ATONEDIR"
fi

docker-compose up --build --no-start docker-sample
docker-compose run docker-sample
