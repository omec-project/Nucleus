#!/bin/bash
#
# Copyright 2020-present Open Networking Foundation
#
# SPDX-License-Identifier: Apache-2.0
#

SUDO=''
[[ $EUID -ne 0 ]] && SUDO=sudo

install_prometheus() {
    set -xe 
    $SUDO apt-get install -y curl libcurl4-openssl-dev
    cd /tmp
    wget https://github.com/Kitware/CMake/releases/download/v3.18.0/cmake-3.18.0-Linux-x86_64.tar.gz
    tar -zxvf cmake-3.18.0-Linux-x86_64.tar.gz
    $SUDO rm -rf /tmp/prometheus
	git clone -q https://github.com/jupp0r/prometheus-cpp.git /tmp/prometheus
	pushd /tmp/prometheus
    git submodule init
    git submodule update
	mkdir -p _build && cd _build
	/tmp/cmake-3.18.0-Linux-x86_64/bin/cmake .. -DBUILD_SHARED_LIBS=ON && make -j 4 && $SUDO make install && $SUDO make DESTDIR=`pwd`/deploy install
}
(return 2>/dev/null) && echo "Sourced" && return

set -o errexit
set -o pipefail
set -o nounset

install_prometheus
echo "Prometheus Dependency install completed"
