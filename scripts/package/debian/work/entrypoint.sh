#!/bin/sh

set -e

basedir="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

main() {
  startdir="$basedir/atone"
  pkgdir="$startdir/pkg"
  srcdir="$startdir/src"
  builddir="$srcdir/atone-${ATONE_BUILD_HASH}"

  mkdir -p "$srcdir"
  cd "$srcdir"

  download "https://github.com/rodrigo-speller/atone/archive/${ATONE_BUILD_NUMBER}.tar.gz" "atone-src.tar.gz"
  tar -xf "atone-src.tar.gz"

  build
  check
  package

  rm -rf "$srcdir" "$pkgdir"
}

build() {
  make -C "$builddir" -j1
}

check() {
  "$builddir/build/release/bin/atone" -v || exit 1
}

package() {
  mkdir -p "$pkgdir/sbin" "$pkgdir/DEBIAN"
  cat "$startdir/DEBIAN.control" | envsubst > "$pkgdir/DEBIAN/control"
  cp "$builddir/build/release/bin/atone" "$pkgdir/sbin"

  mkdir -p "$startdir/repo" 
  fakeroot dpkg-deb --build "$pkgdir" "$startdir/repo/atone_${ATONE_BUILD_VERSION}_${DEBIAN_PACKAGE_ARCH}.deb"
}

download() {
    src="$1"
    dst="$2"
    echo $2
    if command -v curl >/dev/null 2>&1; then
        curl -L "$src" -o "$dst"
    elif command -v wget >/dev/null 2>&1; then
        wget "$src" -O "$dst"
    else
        echo "Atone installation script requires curl or wget but neither is installed. Aborting."
        exit 1
    fi
}

main "$@"
