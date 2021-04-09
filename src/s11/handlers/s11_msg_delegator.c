/*
 * Copyright 2019-present Open Networking Foundation
 * Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "err_codes.h"
#include "gtpv2c.h"
#include "s11_options.h"
#include "ipc_api.h"
#include "s11.h"
#include "s11_config.h"
#include "s11_structs.h"
#include <gtpV2StackWrappers.h>

extern struct GtpV2Stack* gtpStack_gp;
//int s11_CS_resp_handler(MsgBuffer* message, GtpV2MessageHeader* hdr);
//int s11_MB_resp_handler(MsgBuffer* message, GtpV2MessageHeader* hdr);
//int s11_DS_resp_handler(MsgBuffer* message, GtpV2MessageHeader* hdr);
//int s11_RB_resp_handler(MsgBuffer* message, GtpV2MessageHeader* hdr);
//int s11_DDN_handler(MsgBuffer* message, GtpV2MessageHeader* hdr);

/*
  Get count of no of IEs in gtpv2c msg
*/
static int
get_IE_cnt(char *msg, int len)
{
	int cnt = 0;
	char *tmp = msg+11;
	struct s11_IE_header *header = (struct s11_IE_header *)tmp;

	for(; (char *)tmp <= msg + len; ++cnt) {
		tmp += sizeof(struct s11_IE_header) + ntohs(header->ie_len);
		header = (struct s11_IE_header*)tmp;
	}
	return cnt;
}

void
network_cp_fteid(struct fteid *teid, char *data)
{
	unsigned int *tmp;

	memcpy(teid, data, sizeof(struct fteid));
	teid->header.teid_gre = ntohl(teid->header.teid_gre);
	tmp = (unsigned int*)(data+5);
	*tmp = ntohl(*tmp);
	memcpy(&(teid->ip.ipv4), tmp, sizeof(struct in_addr));  
}

int
parse_bearer_ctx(bearer_ctxt_t *bearer, char* data, short len)
{
	char no_of_ies = 4;
	//TODO: count no of IEs

	for(int i=0; i < no_of_ies; ++i) {
		struct s11_IE_header *header = (struct s11_IE_header*)data;
		char *value = data + sizeof(struct s11_IE_header);

		switch(header->ie_type){
		case S11_IE_CAUSE:
			memcpy(&(bearer->cause), value, sizeof(struct gtp_cause));
			break;

		case S11_IE_FTEID_C:{
			#define S1U_SGW_FTEID 1 /*binary 0001*/
			if((0x0F & (unsigned char)(*value)) 
				== S1U_SGW_FTEID) {
				network_cp_fteid(&bearer->s1u_sgw_teid, value);
			}
			else { /*s5s8 pgw_U ftied*/
				network_cp_fteid(&bearer->s5s8_pgw_u_teid, value);
			}
			break;
		}

		case S11_IE_EPS_BEARER_ID:
			bearer->eps_bearer_id = (unsigned char)(*value);
			break;

		default:
		log_msg(LOG_ERROR, "Unhandled S11 bearer IE: %d", header->ie_type);
		}

		data += ntohs(header->ie_len) + sizeof(struct s11_IE_header); /*goto next IE*/
	}
	return SUCCESS;
}

int	
parse_gtpv2c_IEs(char *msg, int len, struct s11_proto_IE *proto_ies)
{
	proto_ies->no_of_ies = get_IE_cnt(msg, len);

	if(0 == proto_ies->no_of_ies) {
		log_msg(LOG_INFO, "No IEs recvd in message");
		return SUCCESS;
	}
	/*allocated IEs for message*/
	proto_ies->s11_ies = (struct s11_IE*)calloc(sizeof(struct s11_IE),
				proto_ies->no_of_ies);
	msg +=11;

	for(int i=0; i < proto_ies->no_of_ies; ++i) {
		struct s11_IE *ie = &(proto_ies->s11_ies[i]);
		char *data = msg + sizeof(struct s11_IE_header);

		memcpy(&(ie->header), msg, sizeof(struct s11_IE_header));

		switch(ie->header.ie_type){
		case S11_IE_CAUSE:
			memcpy(&(ie->data.cause), data, sizeof(struct gtp_cause));
			break;

		case S11_IE_FTEID_C:{
			#define S11_SGW_C_FTEID 11 /*binary 1011*/
			if((0x0F & (unsigned char)(*data)) 
				== S11_SGW_C_FTEID) {
				network_cp_fteid(&(ie->data.s11_sgw_fteid), data);
			}
			else { /*s5s8 pgw_c ftied*/
				network_cp_fteid(&(ie->data.s5s8_pgw_c_fteid), data);
			}
			break;
		}

		case S11_IE_PAA: {
			memcpy(&(ie->data.pdn_addr.pdn_type), data,
				sizeof(ie->data.pdn_addr.pdn_type));
			memcpy(&(ie->data.pdn_addr.ip_type.ipv4), data+1, sizeof(int));
			break;
		}

		case S11_IE_APN_RESTRICTION:
			break;

		case S11_IE_BEARER_CTX:
			parse_bearer_ctx(&(ie->data.bearer), data, ntohs(ie->header.ie_len));
		break;

		default:
		log_msg(LOG_ERROR, "Unhandled S11 IE: %d", ie->header.ie_type);
		}

		msg += (ntohs(((struct s11_IE_header*)msg)->ie_len) + sizeof(struct s11_IE_header)) ; /*goto next IE*/
	}
	return SUCCESS;
}

void
handle_s11_message(void *message)
{
	log_msg(LOG_INFO, "S11 recv msg handler.");

	MsgBuffer* msgBuf_p = (MsgBuffer*)(message);

	uint32_t sgw_ip = MsgBuffer_readUint32(msgBuf_p, sgw_ip);
	uint16_t src_port = MsgBuffer_readUint16(msgBuf_p, src_port);
	
	GtpV2MessageHeader msgHeader;

	bool rc = GtpV2Stack_decodeMessageHeader(gtpStack_gp, &msgHeader, msgBuf_p);

	if(rc == false)
		log_msg(LOG_ERROR,"Failed to decode the GTP Message");

	switch(msgHeader.msgType){
	case GTP_CREATE_SESSION_RSP:
		s11_CS_resp_handler(msgBuf_p, &msgHeader, sgw_ip);
		break;

	case GTP_MODIFY_BEARER_RSP:
		s11_MB_resp_handler(msgBuf_p, &msgHeader, sgw_ip);
		break;

	case GTP_DELETE_SESSION_RSP:
		s11_DS_resp_handler(msgBuf_p, &msgHeader, sgw_ip);
		break;
	
	case GTP_RABR_RSP:
		s11_RB_resp_handler(msgBuf_p, &msgHeader, sgw_ip);
		break;
	
	case GTP_DOWNLINK_DATA_NOTIFICATION:
		s11_DDN_handler(msgBuf_p, &msgHeader, sgw_ip);
		break;
	
	case GTP_CREATE_BEARER_REQ:
        	s11_CB_req_handler(msgBuf_p, &msgHeader, sgw_ip ,src_port);
        	break;
				
    	case GTP_DELETE_BEARER_REQ:
        	s11_DB_req_handler(msgBuf_p, &msgHeader, sgw_ip ,src_port);
        	break;

    	case GTP_ECHO_REQ:
    		s11_ECHO_req_resp_handler(msgBuf_p, &msgHeader, sgw_ip,src_port);
    		break;

	}

	MsgBuffer_free(msgBuf_p);

	return;
}
