/*
 * Copyright 2019-present Open Networking Foundation
 * Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __S10_CONFIG_H_
#define __S10_CONFIG_H_

#include <stdbool.h>

typedef struct s10_config
{
	unsigned int sgw_ip;
	unsigned int pgw_ip;
	unsigned int egtp_def_port;
	unsigned int local_egtp_ip;
} s10_config_t;

void
init_parser(char *path);

int
parse_s10_conf();

#endif /*__S10_CONFIG_H*/
