#!/bin/sh

if [ -z "$1" ]; then
  echo "Usage: $0 <deb-directory>"
  exit 1
fi

if [ ! -d "$1/DEBIAN" ]
then
  echo "Error: Invalid deb-directory: $1"
  exit 1
fi

chmod -R 755 "$1"
chown -R root:root "$1"
dpkg-deb --build "$1"
