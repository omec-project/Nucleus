/*
 * Copyright 2019-present Open Networking Foundation
 * Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include "log.h"
#include <sys/syscall.h>

int pid = 0;
char processName[255] = {0};
uint8_t logging_level;

FILE *log_fp;

void init_logging(char *env, char *file)
{
    pid = getpid();
	if(strcmp(env, "container") == 0) {
		log_fp = (FILE *)stderr;
		fprintf(log_fp, "init_logging %s", env);
	} else {
		log_fp = fopen(file, "a+");
		if (log_fp == NULL)
		{
			printf("Could not open log file");
			exit(0);
		}
		fprintf(log_fp, "init_logging %s", env);
	}
    logging_level = LOG_DEBUG;
}
