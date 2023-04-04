#!/usr/bin/false
#
# Copyright (c) Rodrigo Speller. All rights reserved.
# Licensed under the MIT License. See LICENSE.txt in the project root for license information.

# parameters from command line arguments
for arg in "$@"; do
    expr "$arg" : '^[[:upper:]_]\+=' &>/dev/null && eval "$arg"
done

# parameters
ATONE_PROJECT_DIR=$(git rev-parse --show-toplevel)

ATONE_BUILD_HASH="$(git rev-parse HEAD)"
ATONE_BUILD_NUMBER="${ATONE_BUILD_HASH:0:7}"
ATONE_BUILD_DIR=${ATONE_BUILD_DIR:-build}
ATONE_BUILD_VERSION=${ATONE_BUILD_VERSION:-0.0.0}
ATONE_BUILD_DISTRO=${ATONE_BUILD_DISTRO:-unknown}
ATONE_BUILD_ARCH=${ATONE_BUILD_ARCH:-"$(uname -m)"}

ATONE_PACKAGE_FILENAME="atone-${ATONE_BUILD_VERSION}.tar.gz"
ATONE_PACKAGE_ARCH="$ATONE_BUILD_ARCH"
ATONE_PACKAGE_PRIVKEY="$ATONE_PROJECT_DIR/scripts/keys/atone-95c4f951.rsa"

# distribution specific variables
case "$ATONE_BUILD_DISTRO" in

    alpine)
        ATONE_BUILD_DISTRO=alpine

        case $ATONE_PACKAGE_ARCH in
            x86_64 | amd64)
                ATONE_PACKAGE_ARCH=x86_64
                ;;
            armv7l | arm32v7)
                ATONE_PACKAGE_ARCH=armv7
                ;;
            aarch64 | arm64v8)
                ATONE_PACKAGE_ARCH=aarch64
                ;;
        esac

        ATONE_PACKAGE_FILENAME="atone-${ATONE_BUILD_VERSION}-r0-${ATONE_PACKAGE_ARCH}.apk"
        ;;

    ubuntu|debian)
        ATONE_BUILD_DISTRO=debian

        case $ATONE_PACKAGE_ARCH in
            x86_64 | amd64)
                ATONE_PACKAGE_ARCH=amd64
                ;;
            armv7l | arm32v7)
                ATONE_PACKAGE_ARCH=armhf
                ;;
            aarch64 | arm64v8)
                ATONE_PACKAGE_ARCH=arm64
                ;;
        esac

        ATONE_PACKAGE_FILENAME="atone_${ATONE_BUILD_VERSION}_${ATONE_PACKAGE_ARCH}.deb"
        ;;

    *)
        echo "Unknown distribution: $ATONE_BUILD_DISTRO"
        exit 1
        ;;

esac

# variables
builddir="$ATONE_PROJECT_DIR/$ATONE_BUILD_DIR"
tmpldir="$ATONE_PROJECT_DIR/scripts/package/debian"
pkgdir="$builddir/package"
publishdir="$builddir/publish"

export ATONE_PROJECT_DIR

export ATONE_BUILD_HASH
export ATONE_BUILD_NUMBER
export ATONE_BUILD_DIR
export ATONE_BUILD_VERSION
export ATONE_BUILD_DISTRO
export ATONE_BUILD_ARCH

export ATONE_PACKAGE_ARCH
export ATONE_PACKAGE_PRIVKEY
