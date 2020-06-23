
/*
 * Copyright 2019-present Open Networking Foundation
 * Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __MME_APP_NAS_STRUCTS_H_
#define __MME_APP_NAS_STRUCTS_H_

#include <stdint.h>
#include <stdbool.h>
#include "nas_structs.h"
#include "msgType.h"

class Secinfo;
	
	class MmeNasUtils
	{
		public:
#ifndef S1AP_DECODE_NAS
			static int parse_nas_pdu(s1_incoming_msg_data_t* msg_data, unsigned char *nas_msg,  int nas_msg_len, struct nasPDU *nas);
			static void copy_nas_to_s1msg(struct nasPDU *nas, s1_incoming_msg_data_t *s1Msg);
#endif
#ifndef S1AP_ENCODE_NAS 
			static void encode_nas_msg(struct Buffer *nasBuffer, struct nasPDU *nas, const Secinfo& secContext);
			static void get_negotiated_qos_value(struct esm_qos *qos);
#endif
	};
#endif
