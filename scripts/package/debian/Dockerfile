# Copyright (c) Rodrigo Speller. All rights reserved.
# Licensed under the MIT License. See LICENSE.txt in the project root for license information.

FROM        debian:bullseye

ARG         DEBIAN_FRONTEND=noninteractive

RUN         apt-get update && apt-get install -y \
                g++ \
                gettext-base \
                git \
                make

ENV         ATONE_BUILD_DISTRO=debian
WORKDIR     /work/atone
CMD         ["scripts/package/debian/BUILD"]
