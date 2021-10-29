FROM debian:11-slim

RUN apt-get -y update
RUN apt-get -y dist-upgrade

RUN apt-get -y install \
    gcc \
    gfortran \
    git \
    cmake \
    binutils \
    emacs \
    python3.9 \
    python3-numpy \
    python3-scipy \
    python3-sympy \
    python3-matplotlib \
    python3-pip \
    jupyter-qtconsole \
    python3-lxml \
    python3-pyqtgraph \
    libopenblas-dev \
    liblapack-dev \
    libxml2-dev \
    libcomedi-dev \
    python3-pyqt5
    
RUN apt-get -y clean
RUN apt-get -y autoremove

ADD . /pysimCoder
WORKDIR /pysimCoder
RUN make -f make4docker.mak

CMD /bin/bash

