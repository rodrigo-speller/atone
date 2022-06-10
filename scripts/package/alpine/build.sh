#!/bin/bash
set -e;

# parameters
ATONE_PROJECT_DIR=$(git rev-parse --show-toplevel)
ATONE_BUILD_VERSION=${1:-0.0}
ATONE_BUILD_HASH="$(git rev-parse v$ATONE_BUILD_VERSION)"
ATONE_BUILD_NUMBER="${ATONE_BUILD_HASH:0:7}"

# variables
basedir="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
startdir="$basedir/work/atone"

# initial setup
mkdir -p "$ATONE_PROJECT_DIR/build/package"
rm -rf "$startdir/repo"

# build docker image
docker build -t atone-package-alpine "$basedir"

# checksum update
docker run -it \
  -e ATONE_BUILD_NUMBER="$ATONE_BUILD_NUMBER" \
  -e ATONE_BUILD_HASH="$ATONE_BUILD_HASH" \
  -e ATONE_BUILD_VERSION="$ATONE_BUILD_VERSION" \
  -v "$basedir/work:/work" \
  -v "$ATONE_PROJECT_DIR/scripts/keys:/work/keys" \
  atone-package-alpine abuild checksum

# build package
docker run -it \
  -e ATONE_BUILD_NUMBER="$ATONE_BUILD_NUMBER" \
  -e ATONE_BUILD_HASH="$ATONE_BUILD_HASH" \
  -e ATONE_BUILD_VERSION="$ATONE_BUILD_VERSION" \
  -v "$basedir/work:/work" \
  -v "$ATONE_PROJECT_DIR/scripts/keys:/work/keys" \
  atone-package-alpine

# publish
mv $(find "$startdir/repo" -name '*.apk' -type f) "$ATONE_PROJECT_DIR/build/package/"
