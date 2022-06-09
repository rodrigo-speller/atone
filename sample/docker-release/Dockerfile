FROM        debian:bullseye

# install dependencies
ARG         DEBIAN_FRONTEND=noninteractive
RUN         apt-get update && apt-get install -y \
              curl \
              && rm -rf /var/lib/apt/lists/*

# install atone
ENV         ATONE_VERSION=0.0.4
ARG         ATONE_INSTALL_URL="https://github.com/rodrigo-speller/atone/releases/download/v$ATONE_VERSION/atone-install.sh"
RUN         curl -sSL $ATONE_INSTALL_URL | bash /dev/stdin

# run atone
ENTRYPOINT         [ "atone" ]
CMD                [ "--help" ]
