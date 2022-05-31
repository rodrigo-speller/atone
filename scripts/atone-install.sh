#!/bin/sh

# atone installation script
#
# How to use:
#   curl -sSL https://github.com/rodrigo-speller/atone/releases/download/v0.0.2/atone-install.sh | bash /dev/stdin
#

install() {
    ATONE_BASE_URL="https://github.com/rodrigo-speller/atone/releases/download"
    DEBIAN_PACAKGE_URL="$ATONE_BASE_URL/v0.0.2/atone_0.0.2_amd64.deb"
    DEBIAN_PACKAGE_NAME=$(basename $DEBIAN_PACAKGE_URL)
    
    ATONE_TEMP=$(mktemp -d)
    PACKAGE_FILE="$ATONE_TEMP/$DEBIAN_PACKAGE_NAME"

    download_debian

    dpkg --install "$PACKAGE_FILE"
    rm -rf "$ATONE_TEMP"

    atone -v
}

download_debian() {
    if command -v curl >/dev/null 2>&1; then
        curl -L $DEBIAN_PACAKGE_URL -o "$PACKAGE_FILE"
    elif command -v wget >/dev/null 2>&1; then
        wget $DEBIAN_PACAKGE_URL -O "$PACKAGE_FILE"
    else
        echo "Atone installation script requires curl or wget but neither is installed. Aborting."
        exit 1
    fi
}

install "$@"
