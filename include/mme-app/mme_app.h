/*
 * Copyright 2019-present Open Networking Foundation
 * Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __MME_APP_H_
#define __MME_APP_H_

#include <stdbool.h>

#include "s1ap_structs.h"

typedef struct dns_config
{
        unsigned int dns_flag;
	unsigned int concurrent;
	unsigned int percentage;
	unsigned int interval_seconds;
	unsigned int query_timeout_ms;
	unsigned int query_tries;
        char* dns1_ip;

}dns_config_t;


/**
 * MME main application configuration parameters structures.
 * All fields in this will be filled in from input json file.
 */
typedef struct mme_config
{
	unsigned int mme_ip_addr;
	unsigned short mme_sctp_port;
	unsigned int s11_sgw_ip;
	unsigned int s11_pgw_ip;
	unsigned short mme_egtp_def_port;
	char  *mme_egtp_def_hostname;
	char  *mme_name;
        char  *logging;

	char  mcc_dig1;
	char  mcc_dig2;
	char  mcc_dig3;
	char  mnc_dig1;
	char  mnc_dig2;
	char  mnc_dig3;
	struct PLMN plmn_id;

	unsigned int mme_s1ap_ip;
	unsigned int mme_egtp_ip;
	unsigned short mme_group_id;
	unsigned char mme_code;
        dns_config_t dns_config;
} mme_config;

const size_t fifoQSize_c = 1000;

void stat_init();

/* Register for config change trigger */
void register_config_updates(void);
void mme_parse_config(mme_config *);

#endif /*__MME_APP_H_*/
