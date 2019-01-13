FROM ubuntu:16.04

ARG UNAME=tmpuser
ARG UID=1000
ARG GID=1000
RUN groupadd -g $GID -o $UNAME && \
    useradd -m -u $UID -g $GID -o -s /bin/bash $UNAME

RUN mkdir /ACC-Manager && \
    apt-get update && \
    apt-get install -y wget autoconf automake libtool curl make g++ unzip libcap-dev libpci-dev && \
    wget -qO- https://github.com/protocolbuffers/protobuf/releases/download/v3.6.1/protobuf-cpp-3.6.1.tar.gz | tar xvz -C / && \
    cd /protobuf-3.6.1 && \
    ./configure && \
    make && \
    make check && \
    make install && \
    ldconfig

COPY . /ACC-Manager
RUN chown -R $UNAME:$UNAME /ACC-Manager

USER $UNAME

RUN cd /ACC-Manager && \
    make
