FROM      debian:bullseye

ARG       DEBIAN_FRONTEND=noninteractive

RUN       apt-get update && apt-get install -y \
            curl \
            fakeroot \
            g++ \
            gettext-base \
            make

RUN       adduser --disabled-password --gecos "" packager && \
            addgroup packager root && \
            echo "packager    ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers

WORKDIR   /work/atone
USER      packager
CMD       ["/work/entrypoint.sh"]
