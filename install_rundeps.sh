#!/bin/bash
#
# Copyright 2019-present Open Networking Foundation
#
# SPDX-License-Identifier: Apache-2.0
#

SUDO=''
[[ $EUID -ne 0 ]] && SUDO=sudo

install_run_pkg_deps() {
	$SUDO apt-get update && $SUDO apt-get install -y \
		openssl \
		libidn11 \
		libgnutls30 \
		libsctp1 \
		netbase\
		openssh-server\
		sshpass
}

install_run_utils() {
        $SUDO apt-get update && $SUDO apt-get install -y \
        jq \
        dnsutils \
		iproute2 \
		iputils-ping \
		tcpdump
}

install_ssh_server() {
	$SUDO mkdir /var/run/sshd
	echo 'root:mypass' | chpasswd
	sed -i 's/PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config
	sed 's@session\s*required\s*pam_loginuid.so@session optional pam_loginuid.so@g' -i /etc/pam.d/sshd
	echo "export VISIBLE=now" >> /etc/profile
}

cleanup_image() {
	$SUDO rm -rf /var/lib/apt/lists/*
}

install_run_deps() {
        install_run_pkg_deps
        install_ssh_server
}

(return 2>/dev/null) && echo "Sourced" && return

set -o errexit
set -o pipefail
set -o nounset

install_run_deps
echo "Dependency install completed"
