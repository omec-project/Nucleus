#! /bin/bash
# Copyright 2019-present Open Networking Foundation

#
# Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
# Copyright (c) 2017 Intel Corporation
# Copyright (c) 2019, Infosys Ltd.
#
# SPDX-License-Identifier: Apache-2.0
#

echo "Killing MME application"
killall -9 mme-app s1ap-app s11-app s6a-app
