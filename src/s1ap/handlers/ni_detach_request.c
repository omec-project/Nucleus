/*
 * Copyright (c) 2019, Infosys Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#include "log.h"
#include "err_codes.h"
#include "ipc_api.h"
#include "s1ap.h"
#include "s1ap_config.h"
#include "main.h"
#include "msgType.h"

static int
get_ni_detach_request_protoie_value(struct proto_IE *value, struct ni_detach_request_Q_msg *g_niDetachReqInfo)
{
	uint8_t ieCnt = 0;

	value->no_of_IEs = NI_DTCH_REQUEST_NO_OF_IES;

	value->data = (proto_IEs *) calloc(NI_DTCH_REQUEST_NO_OF_IES,
			sizeof(proto_IEs));

	value->data[ieCnt].val.mme_ue_s1ap_id = g_niDetachReqInfo->ue_idx;
	ieCnt++;

	value->data[ieCnt].val.enb_ue_s1ap_id = g_niDetachReqInfo->enb_s1ap_ue_id;
	ieCnt++;

	return SUCCESS;
}

/**
* Stage specific message processing.
*/
static int
ni_detach_request_processing(struct ni_detach_request_Q_msg *g_niDetachReqInfo)
{
	unsigned char tmpStr[4];
	struct s1ap_PDU s1apPDU = {0};
	uint16_t protocolIe_Id;
	uint8_t protocolIe_criticality = CRITICALITY_REJECT;
	uint8_t initiating_msg = 0;
	uint8_t datalen = 0;
	uint8_t s1ap_len_pos;
	uint8_t u8value = 0;

	Buffer g_acpt_buffer = {0};

	s1apPDU.procedurecode = id_downlinkNASTransport;
	s1apPDU.criticality = CRITICALITY_IGNORE;

	get_ni_detach_request_protoie_value(&s1apPDU.value, g_niDetachReqInfo);

	g_acpt_buffer.pos = 0;

	buffer_copy(&g_acpt_buffer, &initiating_msg,
			sizeof(initiating_msg));

	buffer_copy(&g_acpt_buffer, &s1apPDU.procedurecode,
			sizeof(s1apPDU.procedurecode));

	buffer_copy(&g_acpt_buffer, &s1apPDU.criticality,
				sizeof(s1apPDU.criticality));

	s1ap_len_pos = g_acpt_buffer.pos;

	/* length of NI Detach Request */
	u8value = 0;
	buffer_copy(&g_acpt_buffer, &u8value, sizeof(u8value));

	/* TODO remove hardcoded values */
	uint8_t chProtoIENo[3] = {0,0,3};
	buffer_copy(&g_acpt_buffer, chProtoIENo, 3);

	/* id-MME-UE-S1AP-ID */
	protocolIe_Id = id_MME_UE_S1AP_ID;
	copyU16(tmpStr, protocolIe_Id);
	buffer_copy(&g_acpt_buffer, tmpStr, sizeof(protocolIe_Id));
	buffer_copy(&g_acpt_buffer, &protocolIe_criticality,
					sizeof(protocolIe_criticality));
	datalen = 2;
	/* TODO need to add proper handling*/
	unsigned char mme_ue_id[3];
	datalen = copyU16(mme_ue_id, s1apPDU.value.data[0].val.mme_ue_s1ap_id);
	buffer_copy(&g_acpt_buffer, &datalen, sizeof(datalen));
	buffer_copy(&g_acpt_buffer, mme_ue_id, datalen);

	/* id-eNB-UE-S1AP-ID */
	protocolIe_Id = id_eNB_UE_S1AP_ID;
	copyU16(tmpStr, protocolIe_Id);
	buffer_copy(&g_acpt_buffer, tmpStr, sizeof(protocolIe_Id));
	buffer_copy(&g_acpt_buffer, &protocolIe_criticality,
					sizeof(protocolIe_criticality));
	/* TODO needs proper handling*/
	unsigned char enb_ue_id[3];
	datalen = copyU16(enb_ue_id, s1apPDU.value.data[1].val.enb_ue_s1ap_id);
	buffer_copy(&g_acpt_buffer, &datalen, sizeof(datalen));
	buffer_copy(&g_acpt_buffer, enb_ue_id, datalen);

	/* NAS PDU start */
	protocolIe_Id = id_NAS_PDU;
	copyU16(tmpStr, protocolIe_Id);
	buffer_copy(&g_acpt_buffer, tmpStr, sizeof(protocolIe_Id));
	buffer_copy(&g_acpt_buffer, &protocolIe_criticality,
					sizeof(protocolIe_criticality));

	log_msg(LOG_INFO, "Received network initiated detach - nas message %d ",g_niDetachReqInfo->nasMsgSize);

	datalen = g_niDetachReqInfo->nasMsgSize + 1; 

	buffer_copy(&g_acpt_buffer, &datalen, sizeof(datalen));

    if(g_niDetachReqInfo->nasMsgSize <= 127)
    {
        buffer_copy(&g_acpt_buffer, &g_niDetachReqInfo->nasMsgSize, sizeof(uint8_t));
        buffer_copy(&g_acpt_buffer, &g_niDetachReqInfo->nasMsgBuf[0], g_niDetachReqInfo->nasMsgSize);
    }
    else
    {
        uint16_t nas_len  = g_niDetachReqInfo->nasMsgSize | 0x8000; // set MSB to 1
        unsigned char lenStr[2];
        lenStr[0] = nas_len >> 8;
        lenStr[1] = nas_len & 0xff;
        buffer_copy(&g_acpt_buffer, lenStr, sizeof(lenStr));
        buffer_copy(&g_acpt_buffer, &g_niDetachReqInfo->nasMsgBuf[0], g_niDetachReqInfo->nasMsgSize);
    }

	/* Copy length to s1ap length field */
	datalen = g_acpt_buffer.pos - s1ap_len_pos - 1;
	memcpy(g_acpt_buffer.buf + s1ap_len_pos, &datalen, sizeof(datalen));

	/* TODO: temp fix */
	//g_ics_buffer.buf[1] = 0x09;
	send_sctp_msg(g_niDetachReqInfo->enb_fd, g_acpt_buffer.buf, g_acpt_buffer.pos,1);

	log_msg(LOG_INFO, "NI Detach Request sent to UE.");
    	free(s1apPDU.value.data);

	return SUCCESS;
}



/**
* Thread function for stage.
*/
void*
ni_detach_request_handler(void *data)
{
   log_msg(LOG_INFO, "NI detach request handler ready.");
   struct ni_detach_request_Q_msg *msg = (struct ni_detach_request_Q_msg *)data;

   ni_detach_request_processing(msg);
   
   return NULL;
}

