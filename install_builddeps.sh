#!/bin/bash
#
# Copyright 2019-present Open Networking Foundation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
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
		libgnutls-dev \
		libsctp-dev \
		libssl-dev \
		autoconf \
		libtool \
		pkg-config \
		curl \
  		automake \
		make \
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
	git clone -b v1.26.0 \
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
