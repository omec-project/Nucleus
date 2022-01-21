#!/bin/bash
#
# Copyright 2019-present Open Networking Foundation
#
# SPDX-License-Identifier: Apache-2.0
#

SUDO=''
[[ $EUID -ne 0 ]] && SUDO=sudo

#Identify what the running OS is so we can do proper package installation
identify_os_version() {
    DISTRO_NAME=$(cat /etc/os-release | grep -e ^NAME | cut -d \" -f 2)
    DISTRO_VERSION_ID=$(cat /etc/os-release | grep -e ^VERSION_ID | cut -d \" -f 2)
    DISTRO_VERSION_NAME=$(cat /etc/os-release | grep -e ^VERSION_CODENAME | cut -d \" -f 2)
}

#Change the current working directory to where the script is stored so
#references work properly
change_to_script_root() {
    #Modify BASH's dir variable after cd'ing to where script is stored
    dir=$(cd -P -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)
}

install_build_pkg_deps() {
    #Names of APT Packages we need to install for Ubuntu 16.04
    UBUNTU_16_04_PACKAGE_NAMES="
        git
        build-essential
        cmake
        libuv1-dev
        libgcrypt-dev
        libidn11-dev
        bison
        flex
        wget
        libgnutls-dev
        zlib1g-dev
        checkinstall
        libsctp-dev
        libssl-dev
        autoconf
        libtool
        pkg-config
        curl
        libcurl4-openssl-dev
        automake
        make
        rapidjson-dev
        unzip
        valgrind
    "
    #Names of APT Packages we need to install for Ubuntu 18.04
    UBUNTU_18_04_PACKAGE_NAMES="
        git
        build-essential
        cmake
        libuv-dev
        libgcrypt-dev
        libidn11-dev
        bison
        flex
        wget
        libgnutls28-dev
        zlib1g-dev
        checkinstall
        libsctp-dev
        libssl-dev
        autoconf
        libtool
        pkg-config
        curl
        libcurl4-openssl-dev
        automake
        make
        rapidjson-dev
        unzip
        valgrind
    "
    #Names of APT Packages we need to install for Ubuntu 20.04
    UBUNTU_20_04_PACKAGE_NAMES="
        git
        build-essential
        cmake
        libuv1-dev
        libgcrypt-dev
        libidn11-dev
        bison
        flex
        wget
        libgnutls28-dev
        zlib1g-dev
        checkinstall
        libsctp-dev
        libssl-dev
        autoconf
        libtool
        pkg-config
        curl
        libcurl4-openssl-dev
        automake
        make
        rapidjson-dev
        unzip
        valgrind
        "
    $SUDO apt update
    case $DISTRO_NAME in
        "Ubuntu") # We're running Ubuntu
            case $DISTRO_VERSION_ID in
                "20.04") # Ubuntu 20.04 (latest LTS as of 2021-09-01)
                    echo "Installing APT Packages for Build Dependencies on Ubuntu 20.04..."
                    $SUDO apt install -y $UBUNTU_20_04_PACKAGE_NAMES
                    ;;
                "18.04") # Ubuntu 18.04
                    echo "Installing APT Packages for Build Dependencies on Ubuntu 18.04..."
                    $SUDO apt install -y $UBUNTU_18_04_PACKAGE_NAMES
                    ;;
                "16.04") #Ubuntu 16.04
                    echo "Installing APT Packages for Build Dependencies on Ubuntu 16.04..."
                    $SUDO apt install -y $UBUNTU_16_04_PACKAGE_NAMES
                    ;;
                *) # Other Version of Ubuntu
                    echo "Running Ubuntu version doesn't have package list for build deps."
                    echo "Defaulting to list for Ubuntu 20.04, there may be missing packages."
                    $SUDO apt install -y $UBUNTU_20_04_PACKAGE_NAMES
                    ;;
            esac
            ;;
        *) # We're running something else
            echo "OS not Ubuntu and doesn't have package list for build deps."
            echo "Aborting..."
            exit 1
    esac
}

install_freediameter() {
    $SUDO rm -rf /tmp/freediameter
    git clone -q https://github.com/sdecugis/freeDiameter.git /tmp/freediameter
    pushd /tmp/freediameter
    git checkout -q tags/1.5.0
    mkdir -p build && cd build
    cmake -DDISABLE_SCTP:BOOL=OFF .. && make -j && $SUDO make install
    popd
}

install_grpc() {
    $SUDO rm -rf /tmp/grpc
    git clone -b v1.27.2 \
    -q https://github.com/grpc/grpc /tmp/grpc
    pushd /tmp/grpc && \
    git submodule update --init && \
        CXXFLAGS='-Wno-error' HAS_SYSTEM_PROTOBUF=false make && make install && ldconfig
    popd
}


install_openssl() {
    $SUDO rm -rf /usr/local/src/openssl-3.0.0-alpha3
    $SUDO rm -rf /usr/lcoal/src/openssl-3.0.0-alpha3.tar.gz
    pushd /usr/local/src/
    $SUDO wget https://www.openssl.org/source/openssl-3.0.0-alpha3.tar.gz
    $SUDO tar -xf openssl-3.0.0-alpha3.tar.gz
    pushd openssl-3.0.0-alpha3
    $SUDO./config --prefix=/usr/local/ssl --openssldir=/usr/local/ssl shared zlib
    make && make install && ldconfig
    popd
    popd
}

install_prometheus() {
    set -xe 
    pushd /tmp
    wget https://github.com/Kitware/CMake/releases/download/v3.18.0/cmake-3.18.0-Linux-x86_64.tar.gz
    tar -zxvf cmake-3.18.0-Linux-x86_64.tar.gz
    $SUDO rm -rf /tmp/prometheus
    git clone -q https://github.com/jupp0r/prometheus-cpp.git /tmp/prometheus
    pushd /tmp/prometheus
    git submodule init
    git submodule update
    mkdir -p _build && cd _build
    /tmp/cmake-3.18.0-Linux-x86_64/bin/cmake .. -DBUILD_SHARED_LIBS=ON && make -j 4 && $SUDO make install && $SUDO make DESTDIR=`pwd`/deploy install
    popd
    popd
}

install_pistache() {
    if [ -d "/openmme/tmp/patches" ]; then
        PATCH_ROOT="/openmme/tmp/"
    else
        PATCH_ROOT=$(dirs -l -0)
    fi
    
    cat $PATCH_ROOT/patches/pistache.patch.1.txt
    pushd /tmp
    echo "Installing pistache"
    $SUDO rm -rf /tmp/pistache
    git clone https://github.com/pistacheio/pistache.git
    pushd pistache
    git checkout 270bbefeb25a402153a55053f845e9c7674ab713
    patch -p1 < $PATCH_ROOT/patches/pistache.patch.1.txt
    mkdir build && cd build
    /tmp/cmake-3.18.0-Linux-x86_64/bin/cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ../
    make 
    $SUDO make install
    popd
    popd
}

install_build_deps() {
    identify_os_version
    change_to_script_root
    install_build_pkg_deps
    install_freediameter
    install_grpc
    install_prometheus
    install_pistache
}

(return 2>/dev/null) && echo "Sourced" && return

set -o errexit
set -o pipefail
set -o nounset

install_build_deps
echo "Dependency install completed"
