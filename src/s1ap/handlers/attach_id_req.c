/*
* Copyright 2019-present Open Networking Foundation
*
* SPDX-License-Identifier: Apache-2.0
*
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#include "log.h"
#include "err_codes.h"
#include "s1ap.h"
#include "ipc_api.h"

#include "main.h"
#include "sctp_conn.h"
#include "msgType.h"



/**
* Get ProtocolIE value ID Request sent by mme-app
*/
static int
get_attach_id_request_protoie_value(struct proto_IE *value,struct attachIdReq_info *g_attachIdReqInfo)
{
	
	
	value->no_of_IEs = ATTACH_ID_REQUEST_NO_OF_IES;
	
	value->data = (proto_IEs *) malloc(ATTACH_ID_REQUEST_NO_OF_IES*
			sizeof(proto_IEs));
	

	value->data[0].val.mme_ue_s1ap_id = g_attachIdReqInfo->ue_idx;
	
	value->data[1].val.enb_ue_s1ap_id = g_attachIdReqInfo->s1ap_enb_ue_id;
    

	log_msg(LOG_INFO, "mme_ue_s1ap_id %d and enb_ue_s1ap_id %d",
			g_attachIdReqInfo->ue_idx, g_attachIdReqInfo->s1ap_enb_ue_id);

	return SUCCESS;
}


/**
* Stage specific message processing.
*/
static int
s1ap_attach_id_req_processing(struct attachIdReq_info *g_attachIdReqInfo)
{
	struct Buffer g_buffer = {0};
	struct Buffer g_value_buffer = {0};

	struct s1ap_PDU s1apPDU = {0};


	s1apPDU.procedurecode = id_downlinkNASTransport;
	s1apPDU.criticality = CRITICALITY_IGNORE;

	get_attach_id_request_protoie_value(&s1apPDU.value,g_attachIdReqInfo);

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
	

	/* id-NAS-PDU */
	protocolIe_Id = id_NAS_PDU;
	copyU16(tmpStr, protocolIe_Id);
	buffer_copy(&g_value_buffer, tmpStr,
						sizeof(protocolIe_Id));

	buffer_copy(&g_value_buffer, &protocolIe_criticality,
					sizeof(protocolIe_criticality));

	log_msg(LOG_INFO, "Received Id Req has nas message %d ",g_attachIdReqInfo->nasMsgSize);
	datalen = g_attachIdReqInfo->nasMsgSize + 1; 

	buffer_copy(&g_value_buffer, &datalen,
						sizeof(datalen));

	buffer_copy(&g_value_buffer, &g_attachIdReqInfo->nasMsgSize, sizeof(uint8_t));

	buffer_copy(&g_value_buffer, &g_attachIdReqInfo->nasMsgBuf[0], g_attachIdReqInfo->nasMsgSize);

	buffer_copy(&g_buffer, &g_value_buffer.pos,
						sizeof(uint8_t));

	buffer_copy(&g_buffer, &g_value_buffer,
						g_value_buffer.pos);

	free(s1apPDU.value.data);

	send_sctp_msg(g_attachIdReqInfo->enb_fd, g_buffer.buf, g_buffer.pos, 1);
	
	return SUCCESS;
}



/**
* Thread function for stage.
*/
void*
idreq_handler(void *data)
{
	
	log_msg(LOG_INFO, "S1Ap attach Id Request handler ready.");

	s1ap_attach_id_req_processing((struct attachIdReq_info *)data);

	return NULL;
}

