# Copyright (c) Rodrigo Speller. All rights reserved.
# Licensed under the MIT License. See LICENSE.txt in the project root for license information.

FROM        alpine:3.13

RUN         apk --no-cache --update add \
                alpine-sdk

ENV         ATONE_BUILD_DISTRO=alpine
WORKDIR     /work/atone
CMD         ["scripts/package/alpine/BUILD"]
