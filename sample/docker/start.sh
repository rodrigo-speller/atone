#!/bin/sh

basedir=$(dirname "$0")

ATONEDIR="$basedir/../.."
ATONE="$ATONEDIR/build/release/bin/atone"

make -C "$ATONEDIR" || exit $?

docker-compose -f "$basedir/docker-compose.yml" up --build --no-start docker-sample
docker-compose -f "$basedir/docker-compose.yml" run docker-sample
