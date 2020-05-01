
/*
 * Copyright 2019-present Open Networking Foundation
 * Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __MME_APP_NAS_STRUCTS_H_
#define __MME_APP_NAS_STRUCTS_H_
#ifndef S1AP_DECODE_NAS

#include <stdint.h>
#include <stdbool.h>

#include "sec.h"
#include "ProcedureCode.h"
#include "Criticality.h"
#include "S1AP-PDU.h"
#include "InitiatingMessage.h"
#include "nas_structs.h"
#include "msgType.h"

#define AUTH_REQ_NO_OF_IES 3
#define SEC_MODE_NO_OF_IES 3
#define ESM_REQ_NO_OF_IES 3
#define EMM_INFO_REQ_NO_OF_IES 3
#define ICS_REQ_NO_OF_IES 6
#define DTCH_ACCEPT_NO_OF_IES 3
#define UE_CTX_RELEASE_NO_OF_IES 3
#define ATTACH_REJECT_NO_OF_IES 3
#define SERVICE_REJECT_NO_OF_IES 3 
#define ATTACH_ID_REQUEST_NO_OF_IES 3
#define TAU_RSP_NO_OF_IES 3

#define NI_UE_CTX_RELEASE_NO_OF_IES 3
#define NI_DTCH_REQUEST_NO_OF_IES 3


#define AUTH_REQ_NO_OF_NAS_IES 2
#define SEC_MODE_NO_OF_NAS_IES 1
#define ICS_REQ_NO_OF_NAS_IES 5
#define TAU_RSP_NO_OF_NAS_IES 2

#define AUTHREQ_NAS_SECURITY_PARAM 0x01
#define SERVICE_REQ_SECURITY_HEADER 12


#define MAX_PCO_OPTION_SIZE 260

#define MSISDN_STR_LEN 10

#define EMM_MAX_TAI_LIST 16

#define SECURITY_KEY_SIZE 32

#define AUTH_SYNC_FAILURE 21
#define AUTH_RESPONSE 53
/* ESM messages */
#define ESM_MSG_ACTV_DEF_BEAR__CTX_REQ 0xc1
/*24.008 - 10.5.6.1
APN name can be in range of min 3 octets to max 102 octets
*/
#define MAX_APN_LEN 102

/*NAS message type codes*/
#define NAS_ESM_RESP 0xda
#define NAS_AUTH_RESP 0x53
#define NAS_AUTH_REJECT 0x54
#define NAS_AUTH_FAILURE 0x5c
#define NAS_IDENTITY_REQUEST 0x55
#define NAS_IDENTITY_RESPONSE 0x56
#define NAS_SEC_MODE_COMPLETE 0x5e
#define NAS_SEC_MODE_REJECT  0x5f
#define NAS_ATTACH_REQUEST 0x41
#define NAS_ATTACH_COMPLETE 0x43
#define NAS_ATTACH_REJECT 0x44
#define NAS_TAU_REQUEST    0x48
#define NAS_TAU_COMPLETE   0x4a
#define NAS_DETACH_REQUEST 0x45
#define NAS_DETACH_ACCEPT 0x46
#define NAS_SERVICE_REQUEST 0x4D

	/****Information elements presentations **/
	#define BCD_IMSI_STR_LEN 15
	
	class MmeNasUtils
	{
		public:
			static void parse_nas_pdu(unsigned char *msg,  int nas_msg_len, struct nasPDU *nas);
			static void copy_nas_to_s1msg(struct nasPDU *nas, s1_incoming_msg_data_t *s1Msg);
			static void encode_nas_msg(struct Buffer *nasBuffer, struct nasPDU *nas);
	};
#endif
#endif /*__S1AP_STRUCTS_H*/
