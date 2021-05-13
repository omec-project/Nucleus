/*
 * Copyright 2019-present Open Networking Foundation
 * Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __S10_STRUCTS_H_
#define __S10_STRUCTS_H_

#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <nas_structs.h>

#define S10_MSGBUF_SIZE 2048
#define DED_BEARER_COUNT 1

#pragma pack(1)
#ifdef S10_FEATURE
typedef enum hoType {
 	invalid_c,
 	intraMmeS1Ho_c,
	interMmeS1Ho_c
}hoType;
typedef struct mm_context {
	security_mode security_mode;
	E_UTRAN_sec_vector sec_vector;
	drx_params drx;
	unsigned int dl_count;
	unsigned int ul_count;
	unsigned int security_encryption_algo;
	unsigned int security_integrity_algo;
	ue_add_sec_capabilities ue_add_sec_capab;
	UE_net_capab ue_network;
	bool isNHIpresent;
	ue_aggregate_maximum_bitrate ue_aggrt_max_bit_rate;
} mm_context_t;
#endif


#pragma pack()

#endif /* S10_STRUCTS_H */
