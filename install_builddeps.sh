#!/bin/bash
#
# Copyright 2019-present Open Networking Foundation
#
# SPDX-License-Identifier: Apache-2.0
#

THIRD_PARTY_DIR="third_party"
SUDO=''
[[ $EUID -ne 0 ]] && SUDO=sudo

install_build_pkg_deps() {
	$SUDO apt-get update && $SUDO apt-get install -y \
		git \
		build-essential \
		cmake \
		libuv-dev \
		libgcrypt-dev \
		libidn11-dev \
		bison \
		flex \
        wget \
		libgnutls-dev \
        zlib1g-dev \
        checkinstall \
		libsctp-dev \
		libssl-dev \
		autoconf \
		libtool \
		pkg-config \
		curl \
  		automake \
		make \
        rapidjson-dev \
		unzip sudo
}

install_freediameter() {
        $SUDO rm -rf $THIRD_PARTY_DIR/freediameter
	git clone -q https://github.com/omec-project/freediameter.git $THIRD_PARTY_DIR/freediameter
	pushd $THIRD_PARTY_DIR/freediameter
	mkdir -p build && cd build
	cmake -DDISABLE_SCTP:BOOL=OFF .. && make -j && $SUDO make install
        popd
}

install_grpc() {
	$SUDO rm -rf $THIRD_PARTY_DIR/grpc
	git clone -b v1.27.2 \
	-q https://github.com/grpc/grpc $THIRD_PARTY_DIR/grpc
	pushd $THIRD_PARTY_DIR/grpc && \
	git submodule update --init && \
    	CXXFLAGS='-Wno-error' make && make install && ldconfig
        popd
}

install_epctools() {
        $SUDO rm -rf $THIRD_PARTY_DIR/epctools
        git clone -q https://github.com/omec-project/epctools.git $THIRD_PARTY_DIR/epctools
        pushd $THIRD_PARTY_DIR/epctools
        git submodule update --init
        ./configure &&  $SUDO make install && ldconfig
        popd
}


install_openssl() {
    $SUDO rm -rf /usr/local/src/openssl-3.0.0-alpha3
    $SUDO rm -rf /usr/lcoal/src/openssl-3.0.0-alpha3.tar.gz
    cd /usr/local/src/
    $SUDO wget https://www.openssl.org/source/openssl-3.0.0-alpha3.tar.gz
    $SUDO tar -xf openssl-3.0.0-alpha3.tar.gz
    cd openssl-3.0.0-alpha3
    $SUDO./config --prefix=/usr/local/ssl --openssldir=/usr/local/ssl shared zlib
    make && make install && ldconfig
}
install_build_deps() {
	install_build_pkg_deps
	install_freediameter
	install_grpc
	install_epctools
}

(return 2>/dev/null) && echo "Sourced" && return

set -o errexit
set -o pipefail
set -o nounset

install_build_deps
echo "Dependency install completed"
