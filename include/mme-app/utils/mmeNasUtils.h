
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
namespace mme
{
    class UEContext;
    class SessionContext;
    class BearerContext;
}
	
	class MmeNasUtils
	{
		public:
			static int parse_nas_pdu(s1_incoming_msg_header_t* msg_data, unsigned char *nas_msg,  int nas_msg_len, struct nasPDU *nas);
			static void copy_nas_to_s1msg(struct nasPDU *nas, s1_incoming_msg_header_t *s1Msg);
			static void encode_nas_msg(struct Buffer *nasBuffer, struct nasPDU *nas, Secinfo& secContext);
			static void get_negotiated_qos_value(struct esm_qos *qos);
			static void calculate_ext_apn_ambr(const uint64_t &ambr_Mbps, uint8_t &unit, uint16_t& conv_ambr);
			static void select_sec_alg(mme::UEContext *ue_ctxt);
			static void decode_attach_req(unsigned char *msg,  int& nas_msg_len, struct nasPDU *nas);
			static void decode_tau_req(unsigned char *msg,  int& nas_msg_len, struct nasPDU *nas);
			static void cal_nas_bit_rate(uint64_t bit_rate_kbps, uint8_t* out);
			static void encode_eps_qos(const bearer_qos_t& bearerQos, eps_qos_t& eps_qos);
			static void decode_act_ded_br_ctxt_acpt(unsigned char *msg, int& nas_msg_len, struct nasPDU *nas);
			static void decode_act_ded_br_ctxt_rjct(unsigned char *msg, int& nas_msg_len, struct nasPDU *nas);
            		static void decode_deact_ded_br_ctxt_acpt(unsigned char *msg, int& nas_msg_len, struct nasPDU *nas);
			static void encode_act_ded_br_req_nas_pdu(mme::SessionContext* sessionCtxt, mme::BearerContext* bearerCtxt_p, Secinfo& secContext, struct nasPDU *nas);
			static uint8_t encode_act_ded_br_req(struct Buffer *nasBuffer, struct nasPDU *nas);
			static void encode_deact_ded_br_req_nas_pdu(uint8_t bearerId, Secinfo& secContext, struct nasPDU *nas);
	        	static uint8_t encode_deact_ded_br_req(struct Buffer *nasBuffer, struct nasPDU *nas);
	};
#endif
