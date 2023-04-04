#!/bin/bash
#
# Copyright (c) Rodrigo Speller. All rights reserved.
# Licensed under the MIT License. See LICENSE.txt in the project root for license information.
#
# Required packages:
#   g++
#   gettext-base
#   git
#   make

set -e

basedir="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

source "$basedir/../common.sh"

main() {
    echo Building Atone Debian package...
    echo "  ATONE_PROJECT_DIR=$ATONE_PROJECT_DIR"
    echo "  ATONE_BUILD_HASH=$ATONE_BUILD_HASH"
    echo "  ATONE_BUILD_NUMBER=$ATONE_BUILD_NUMBER"
    echo "  ATONE_BUILD_DIR=$ATONE_BUILD_DIR"
    echo "  ATONE_BUILD_VERSION=$ATONE_BUILD_VERSION"
    echo "  ATONE_BUILD_DISTRO=$ATONE_BUILD_DISTRO"
    echo "  ATONE_BUILD_ARCH=$ATONE_BUILD_ARCH"
    echo "  ATONE_PACKAGE_ARCH=$ATONE_PACKAGE_ARCH"

    setup
    build
    check
    package
}

setup() {
    # setup directories
    rm -rf "$pkgdir" "$publishdir"
    mkdir -p "$pkgdir" "$publishdir"
}

build() {
    make -C "$ATONE_PROJECT_DIR" "BUILDDIR=$ATONE_BUILD_DIR" -j1
}

check() {
    "$builddir/release/bin/atone" -v || exit 1
}

package() {
    mkdir -p "$pkgdir/sbin" "$pkgdir/DEBIAN"
    cat "$tmpldir/DEBIAN.control" | envsubst > "$pkgdir/DEBIAN/control"
    cp "$builddir/release/bin/atone" "$pkgdir/sbin"

    dpkg-deb --build "$pkgdir" "$publishdir/${ATONE_PACKAGE_FILENAME}"
}

main "$@"
