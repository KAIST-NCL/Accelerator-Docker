FROM ubuntu:16.04

ARG UNAME=tmpuser
ARG UID=1000
ARG GID=1000
RUN groupadd -g $GID -o $UNAME && \
    useradd -m -u $UID -g $GID -o -s /bin/bash $UNAME

RUN mkdir /FPGA-Runtime-Hook && \
    apt-get update && \
    apt-get install -y build-essential wget && \
    wget -qO- https://dl.google.com/go/go1.11.4.linux-amd64.tar.gz | tar xvz -C /usr/local

ENV PATH $PATH:/usr/local/go/bin

COPY . /FPGA-Runtime-Hook
RUN chown -R $UNAME:$UNAME /FPGA-Runtime-Hook

USER $UNAME

RUN cd /FPGA-Runtime-Hook && \
    make
