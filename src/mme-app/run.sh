#! /bin/bash
# Copyright 2019-present Open Networking Foundation
#
# Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
# Copyright (c) 2017 Intel Corporation
# Copyright (c) 2019, Infosys Ltd.
#
# SPDX-License-Identifier: Apache-2.0
#

export LD_LIBRARY_PATH=./lib:/usr/local/lib:/usr/local/ssl/lib
echo "Start MME application"
./bin/mme-app &
sleep 1
./bin/s1ap-app  &
sleep 1
./bin/s6a-app > /dev/null &
sleep 1
./bin/s11-app > /dev/null &
