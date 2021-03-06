#!/bin/bash
set -e

# parameters
ATONE_PROJECT_DIR=$(git rev-parse --show-toplevel)
ATONE_BUILD_VERSION=${1:-0.0}
ATONE_BUILD_HASH="$(git rev-parse v$ATONE_BUILD_VERSION)"
ATONE_BUILD_NUMBER="${ATONE_BUILD_HASH:0:7}"
ATONE_BUILD_ARCH="$(uname -m)"

DEBIAN_PACKAGE_ARCH=$ATONE_BUILD_ARCH
case $DEBIAN_PACKAGE_ARCH in
    x86_64)
        DEBIAN_PACKAGE_ARCH=amd64
        ;;
    armv7l)
        DEBIAN_PACKAGE_ARCH=armhf
        ;;
    aarch64)
        DEBIAN_PACKAGE_ARCH=arm64
        ;;
esac

# variables
basedir="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
startdir="$basedir/work/atone"

# initial setup
mkdir -p "$ATONE_PROJECT_DIR/build/package"
rm -rf "$startdir/repo"

# build docker image
docker build -t atone-package-debian "$basedir"

# build package
docker run -it \
  -e ATONE_BUILD_NUMBER="$ATONE_BUILD_NUMBER" \
  -e ATONE_BUILD_HASH="$ATONE_BUILD_HASH" \
  -e ATONE_BUILD_VERSION="$ATONE_BUILD_VERSION" \
  -e ATONE_BUILD_ARCH="$ATONE_BUILD_ARCH" \
  -e DEBIAN_PACKAGE_ARCH="$DEBIAN_PACKAGE_ARCH" \
  -v "$basedir/work:/work" \
  atone-package-debian

# publish
mv $(find "$startdir/repo" -name '*.deb' -type f) "$ATONE_PROJECT_DIR/build/package/"

