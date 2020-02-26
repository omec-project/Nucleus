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

#ifndef __UE_TABLE_H_
#define __UE_TABLE_H_

#include "s1ap_structs.h"
#include "s11_structs.h"
#include "msgType.h"

struct secinfo {
	uint8_t int_key[NAS_INT_KEY_SIZE];
	uint8_t kenb_key[KENB_SIZE];
};

struct AMBR {
	unsigned int max_requested_bw_dl;
	unsigned int max_requested_bw_ul;
};

#endif /*ue_table*/
