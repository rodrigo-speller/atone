#!/bin/sh

basename=$(dirname "$0")

ATONEDIR="$basename/../.."
ATONE="$ATONEDIR/build/release/bin/atone"

if [ ! -e "$ATONE" ]; then
    make -C "$ATONEDIR"
fi

docker-compose -f "$basename/docker-compose.yml" up --build --no-start docker-sample
docker-compose -f "$basename/docker-compose.yml" run docker-sample
