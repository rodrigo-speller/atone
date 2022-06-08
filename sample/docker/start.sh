#!/bin/sh

basename=$(dirname "$0")

ATONEDIR="$basename/../.."
ATONE="$ATONEDIR/build/release/bin/atone"

make -C "$ATONEDIR" || exit $?

docker-compose -f "$basename/docker-compose.yml" up --build --no-start docker-sample
docker-compose -f "$basename/docker-compose.yml" run docker-sample
