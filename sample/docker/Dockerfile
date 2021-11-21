FROM        debian:bullseye

ARG         DEBIAN_FRONTEND=noninteractive

RUN         apt-get update && apt-get install -y \
                procps

CMD         [ "atone", "-c", "/srv/config/atone.yml" ]
