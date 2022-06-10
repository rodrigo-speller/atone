FROM      alpine:3.13

RUN       apk --no-cache --update add \
            alpine-sdk

RUN       adduser -D packager && \
            addgroup packager abuild && \
            mkdir -p /var/cache/distfiles && \
            chgrp abuild /var/cache/distfiles && \
            chmod g+w /var/cache/distfiles && \
            echo "packager    ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers

ENV       PACKAGER_PRIVKEY=/work/keys/atone-95c4f951.rsa
WORKDIR   /work/atone
USER      packager
CMD       ["abuild", "-P", "/work/atone/repo"]
