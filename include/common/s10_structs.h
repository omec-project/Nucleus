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
typedef struct mm_context {
	uint8_t security_mode;
	bool isDRXPresent;
	bool isuambriPresent;
	bool issambriPresent;


	uint64_t ext_ue_ambr_DL;
	uint64_t ext_ue_ambr_UL;
} mm_context_t;


#pragma pack()

#endif /* S10_STRUCTS_H */
