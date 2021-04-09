/*
 * Copyright 2019-present Open Networking Foundation
 * Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include "log.h"
#include "options.h"

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
		log_msg(LOG_ERROR, "Usage: %s", argv[0]);
		for (c = 0; long_options[c].name; ++c) {
			log_msg(LOG_ERROR, "\t[ -%s | -%c ] %s",
					long_options[c].name,
					long_options[c].val,
					long_options[c].name);
		}
		exit(0);
	}
}

void log_buffer_free(char** buffer)
{
    if(*buffer != NULL)
        free(*buffer);
    *buffer = NULL;
}

void convert_imsi_to_bcd_str(uint8_t *src, uint8_t* dest)
{
  if (!src || !dest)
  {
      log_msg(LOG_ERROR, "invalid buffer pointers.");
      return;
  }

  int len  = BINARY_IMSI_LEN;
  int i = 0;
  for (; i < len - 1; i++) {
      dest[2 * i] = '0' + ((src[i] >> 4) & 0x0F);
      dest[2 * i + 1] = '0' + ((src[i]) & 0x0F);
  }

  dest[2 * (len-1)] = '0' + ((src[i] >> 4) & 0x0F);

  return;
}


