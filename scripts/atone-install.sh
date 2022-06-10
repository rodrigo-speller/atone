#!/bin/sh

# atone installation script
#
# How to use:
#   curl -sSL https://github.com/rodrigo-speller/atone/releases/download/latest/atone-install.sh | sh
#

set -e

install() {
    ATONE_VERSION=0.0.4
    ATONE_BASE_URL="https://github.com/rodrigo-speller/atone/releases/download/v${ATONE_VERSION}"
    
    ATONE_TEMP=$(mktemp -d)

    if is_distro "debian"; then
        install_debian
    elif is_distro "alpine"; then
        install_alpine
    else
        echo "Unsupported distribution"
        exit 1
    fi

    rm -rf "$ATONE_TEMP"
    atone -v
}

install_debian() {
    DEBIAN_PACAKGE_URL="${ATONE_BASE_URL}/atone_${ATONE_VERSION}_amd64.deb"
    DEBIAN_PACKAGE_NAME=$(basename $DEBIAN_PACAKGE_URL)
    DEBIAN_PACKAGE_FILE="$ATONE_TEMP/$DEBIAN_PACKAGE_NAME"

    download "$DEBIAN_PACAKGE_URL" "$DEBIAN_PACKAGE_FILE"

    dpkg --install "$DEBIAN_PACKAGE_FILE"
}

install_alpine() {
    ALPINE_PACAKGE_URL="${ATONE_BASE_URL}/atone-${ATONE_VERSION}-r0-x86_64.apk"
    ALPINE_PACKAGE_NAME=$(basename $ALPINE_PACAKGE_URL)
    ALPINE_PACKAGE_FILE="$ATONE_TEMP/$ALPINE_PACKAGE_NAME"

    download "$ALPINE_PACAKGE_URL" "$ALPINE_PACKAGE_FILE"

    apk add --allow-untrusted "$ALPINE_PACKAGE_FILE"
}

download() {
    src="$1"; dst="$2"

    if command -v curl >/dev/null 2>&1; then
        curl -L "$src" -o "$dst"
    elif command -v wget >/dev/null 2>&1; then
        wget "$src" -O "$dst"
    else
        echo "Atone installation script requires curl or wget but neither is installed. Aborting."
        exit 1
    fi
}

is_distro() {
    if [ "${ID:-linux}" = "$1" ]; then
        return 0;
    fi

    IFS=" "
    for x in ${ID_LIKE:-linux}; do
      if [ "$x" = "$1" ]; then
        return 0
      fi
    done

    return 1
}

test -e /etc/os-release && os_release='/etc/os-release' || os_release='/usr/lib/os-release'
. "${os_release}"

install "$@"
