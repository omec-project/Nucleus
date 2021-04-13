/*
 * Copyright 2019-present Open Networking Foundation
 * Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __LOG_1_H_
#define __LOG_1_H_

#ifdef __cplusplus
extern "C"{
#endif
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <sys/syscall.h>
#include <unistd.h>

#define __file__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

extern int pid;
extern FILE *log_fp;
extern uint8_t logging_level;
enum log_levels{
	LOG_ERROR,
	LOG_WARNING,
	LOG_INFO,
	LOG_DEBUG,
    LOG_NEVER,
};

static const char *log_level_name[] = { "ERROR", "WARN", "INFO", "DEBUG", "NEVER"};

void init_logging(char *env, char *file);
void init_backtrace(char *binary);

#define log_msg(prio, msg, ...) do {\
	if(prio<=logging_level) { \
        char _s[30]; \
        time_t _t = time(NULL); \
        struct tm * _p = localtime(&_t);\
        strftime(_s, 30, "%Y-%m-%d %H:%M:%S", _p);\
		fprintf(log_fp, "[%s] %s : %s : %s : %u : [%u:%ld] " msg " \n", log_level_name[prio], _s, __file__ , __func__, __LINE__, pid, syscall(SYS_gettid), ##__VA_ARGS__);\
	}\
} while (0) 

static inline void set_logging_level(char *log_level)
{
    if(strcmp(log_level, "debug") == 0)
    {
        logging_level = LOG_DEBUG;
    }
    else if(strcmp(log_level, "info") == 0)
    {
        logging_level= LOG_INFO;
    }
    else if(strcmp(log_level, "warn") == 0)
    {
        logging_level = LOG_WARNING;
    }
    else if(strcmp(log_level, "error") == 0)
    {
        logging_level = LOG_ERROR;
    }
    else
    {
	    log_msg(LOG_INFO, "logging disabled ");
        logging_level = LOG_NEVER;
    }
}

#ifdef __cplusplus
}
#endif

#endif /* __LOG_1_H_ */
