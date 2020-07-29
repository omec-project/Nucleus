/*
 * Copyright 2019-present Open Networking Foundation
 * Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "s11_options.h"
#include "msgType.h"
#include <string.h>
#include "log.h"

uint8_t create_sock_addr(
                struct sockaddr_in *addr, 
                uint16_t port, uint32_t val)
{
	uint8_t result = 0;
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = htonl(val);
	memset(addr->sin_zero, '\0', sizeof(addr->sin_zero));
	return result;
}

void parse_args(int argc, char **argv)
{
	int args_set = 0;
	int c = 0;

	const struct option long_options[] = {
	  {"config_file",  required_argument, NULL, 'f'},
	  {0, 0, 0, 0}
	};

	do {
		int option_index = 0;

		c = getopt_long(argc, argv, "f:", long_options,
				&option_index);

		if (c == -1)
			break;

		switch (c) {
		case 'f':
			break;
		default:
			log_msg(LOG_ERROR, "Unknown argument - %s.", argv[optind]);
			exit(0);
		}
	} while (c != -1);

	if ((args_set & REQ_ARGS) != REQ_ARGS) {
		log_msg(LOG_ERROR, "Usage: %s\n", argv[0]);
		for (c = 0; long_options[c].name; ++c) {
			log_msg(LOG_ERROR, "\t[ -%s | -%c ] %s\n",
					long_options[c].name,
					long_options[c].val,
					long_options[c].name);
		}
		exit(0);
	}
}

