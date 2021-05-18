/*
 * Copyright 2019-present Open Networking Foundation
 * Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "err_codes.h"
#include "gtpv2c.h"
#include "s10_options.h"
#include "ipc_api.h"
#include "s10.h"
#include "s10_config.h"
#include <gtpV2StackWrappers.h>


void
handle_s10_message(void *message)
{
	log_msg(LOG_INFO, "S10 recv msg handler.");

	return;
}
