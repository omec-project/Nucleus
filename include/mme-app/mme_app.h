/*
 * Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
 * Copyright (c) 2017 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __MME_APP_H_
#define __MME_APP_H_

#include <stdbool.h>
#include <map>
#include <array>

#include "s1ap_structs.h"

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
	unsigned int enb_ip;
	unsigned short enb_port;
	unsigned short mme_egtp_def_port;
	char  *mme_egtp_def_hostname;
	char  *mme_name;

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
} mme_config;

const size_t fifoQSize_c = 1000;

void stat_init();


#endif /*__MME_APP_H_*/
