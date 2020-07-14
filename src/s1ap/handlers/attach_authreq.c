/*
 * Copyright 2019-present Open Networking Foundation
 * Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#include "log.h"
#include "err_codes.h"
#include "s1ap.h"
#include "ipc_api.h"
#include "main.h"
#include "sctp_conn.h"
#include "msgType.h"

void
buffer_copy(struct Buffer *buffer, void *value, size_t size)
{
	memcpy(buffer->buf + buffer->pos , value, size);
	buffer->pos += size;
	return;
}

/**
* Get ProtocolIE value for Auth Request sent by mme-app
*/
static int
get_authreq_protoie_value(struct proto_IE *value, struct authreq_info *g_authreqInfo)
{
	value->no_of_IEs = AUTH_REQ_NO_OF_IES;

	value->data = (proto_IEs *) malloc(SEC_MODE_NO_OF_IES *
			sizeof(proto_IEs));

	value->data[0].val.mme_ue_s1ap_id = g_authreqInfo->ue_idx;
	value->data[1].val.enb_ue_s1ap_id = g_authreqInfo->enb_s1ap_ue_id;

	log_msg(LOG_INFO, "mme_ue_s1ap_id %d and enb_ue_s1ap_id %d and enodeb fd = %d\n",
			g_authreqInfo->ue_idx, g_authreqInfo->enb_s1ap_ue_id, g_authreqInfo->enb_fd);

#ifdef S1AP_ENCODE_NAS
	/* TODO: Add enum for security header type */
	value->data[2].val.nas.header.security_header_type = 0;
	value->data[2].val.nas.header.proto_discriminator = EPSMobilityManagementMessages;
	value->data[2].val.nas.header.message_type = AuthenticationRequest;
	value->data[2].val.nas.header.nas_security_param = AUTHREQ_NAS_SECURITY_PARAM;

	value->data[2].val.nas.elements = (nas_pdu_elements *)
			malloc(AUTH_REQ_NO_OF_NAS_IES * sizeof(nas_pdu_elements));

	memcpy(value->data[2].val.nas.elements[0].pduElement.rand,
			g_authreqInfo->rand, NAS_RAND_SIZE);
	memcpy(value->data[2].val.nas.elements[1].pduElement.autn,
			g_authreqInfo->autn, NAS_AUTN_SIZE);
#endif


	return SUCCESS;
}


/**
* Stage specific message processing.
*/
static int
authreq_processing(struct authreq_info *g_authreqInfo)
{
	struct Buffer g_buffer = {0};
	struct Buffer g_value_buffer = {0};

	struct s1ap_PDU s1apPDU = {0};

	/* Assigning values to s1apPDU */
	s1apPDU.procedurecode = id_downlinkNASTransport;
	s1apPDU.criticality = CRITICALITY_IGNORE;

	get_authreq_protoie_value(&s1apPDU.value, g_authreqInfo);

	/* Copy values to buffer from s1apPDU */

	g_buffer.pos = 0;

	uint8_t initiating_message = 0; /* TODO: Add enum */
	buffer_copy(&g_buffer, &initiating_message,
			sizeof(initiating_message));

	buffer_copy(&g_buffer, &s1apPDU.procedurecode,
			sizeof(s1apPDU.procedurecode));

	buffer_copy(&g_buffer, &s1apPDU.criticality,
			sizeof(s1apPDU.criticality));

	/* Copy values in g_value_buffer */
	g_value_buffer.pos = 0;

	/* TODO remove hardcoded values */
	unsigned char chProtoIENo[3] = {0,0,3};

	buffer_copy(&g_value_buffer, chProtoIENo, 3);

	unsigned char tmpStr[4];
	/* id-MME-UE-S1AP-ID */
	uint16_t protocolIe_Id = id_MME_UE_S1AP_ID;
	copyU16(tmpStr, protocolIe_Id);
	buffer_copy(&g_value_buffer, tmpStr,
					sizeof(protocolIe_Id));

	uint8_t protocolIe_criticality = CRITICALITY_REJECT;
	buffer_copy(&g_value_buffer, &protocolIe_criticality,
					sizeof(protocolIe_criticality));

	uint8_t datalen = 2;

	/* TODO needs proper handling*/
	unsigned char mme_ue_id[3];
	datalen = copyU16(mme_ue_id,
			s1apPDU.value.data[0].val.mme_ue_s1ap_id);
	buffer_copy(&g_value_buffer, &datalen, sizeof(datalen));
	buffer_copy(&g_value_buffer, mme_ue_id, datalen);

	/* id-eNB-UE-S1AP-ID */

	protocolIe_Id = id_eNB_UE_S1AP_ID;
	copyU16(tmpStr, protocolIe_Id);
	buffer_copy(&g_value_buffer, tmpStr,
						sizeof(protocolIe_Id));

	buffer_copy(&g_value_buffer, &protocolIe_criticality,
					sizeof(protocolIe_criticality));


	/* TODO needs proper handling*/
	unsigned char enb_ue_id[3];
	datalen = copyU16(enb_ue_id,
			s1apPDU.value.data[1].val.enb_ue_s1ap_id);
	buffer_copy(&g_value_buffer, &datalen, sizeof(datalen));
	buffer_copy(&g_value_buffer, enb_ue_id, datalen);
	//STIMER_GET_CURRENT_TP(g_attach_stats[s1apPDU.value.enb_ue_s1ap_id].esm_in);

	/* id-NAS-PDU */
	protocolIe_Id = id_NAS_PDU;
	copyU16(tmpStr, protocolIe_Id);
	buffer_copy(&g_value_buffer, tmpStr,
						sizeof(protocolIe_Id));

	buffer_copy(&g_value_buffer, &protocolIe_criticality,
					sizeof(protocolIe_criticality));

#ifdef S1AP_ENCODE_NAS
	struct nasPDU *nas = &(s1apPDU.value.data[2].val.nas);
	uint8_t value = (nas->header.security_header_type) |
			nas->header.proto_discriminator;

	struct Buffer g_nas_buffer = {0};
	g_nas_buffer.pos = 0;

	buffer_copy(&g_nas_buffer, &value, sizeof(value));

	buffer_copy(&g_nas_buffer, &nas->header.message_type,
						sizeof(nas->header.message_type));

	buffer_copy(&g_nas_buffer, &nas->header.nas_security_param,
						sizeof(nas->header.nas_security_param));

	buffer_copy(&g_nas_buffer, 
	     &nas->elements[0].pduElement.rand,
	     sizeof(nas->elements[0].pduElement.rand));

	datalen = 16;
	buffer_copy(&g_nas_buffer, &datalen, sizeof(datalen));

	buffer_copy(&g_nas_buffer, 
	   &nas->elements[1].pduElement.autn,
	   sizeof(nas->elements[1].pduElement.autn));
	datalen = g_nas_buffer.pos + 1;

	buffer_copy(&g_value_buffer, &datalen,
						sizeof(datalen));

	buffer_copy(&g_value_buffer, &g_nas_buffer.pos,
						sizeof(g_nas_buffer.pos));


	buffer_copy(&g_value_buffer, &g_nas_buffer,
						g_nas_buffer.pos);

#else
	log_msg(LOG_INFO, "Received Authrequest has nas message %d \n",g_authreqInfo->nasMsgSize);
	datalen = g_authreqInfo->nasMsgSize + 1; 

	buffer_copy(&g_value_buffer, &datalen,
						sizeof(datalen));

	buffer_copy(&g_value_buffer, &g_authreqInfo->nasMsgSize, sizeof(uint8_t));

	buffer_copy(&g_value_buffer, &g_authreqInfo->nasMsgBuf[0], g_authreqInfo->nasMsgSize);

#endif

	buffer_copy(&g_buffer, &g_value_buffer.pos,
						sizeof(g_value_buffer.pos));

	buffer_copy(&g_buffer, &g_value_buffer,
						g_value_buffer.pos);

#ifdef S1AP_ENCODE_NAS
	free(s1apPDU.value.data[2].val.nas.elements);
#endif
	free(s1apPDU.value.data);

	send_sctp_msg(g_authreqInfo->enb_fd, g_buffer.buf, g_buffer.pos, 1);

	return SUCCESS;
}

void*
authreq_handler(void *data)
{
	log_msg(LOG_INFO, "AuthReq handler.\n");

	authreq_processing((struct authreq_info *)data);

	return NULL;
}
