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
#include <stdint.h>

#include "err_codes.h"
#include "ipc_api.h"
#include "main.h"
#include "msgType.h"
#include "s1ap.h"
#include  "log.h"
#include "snow_3g.h"



/****Global and externs end***/
extern ipc_handle ipc_S1ap_Hndl;
/**
* Get ProtocolIE value for Sec Request sent by mme-app
*/
static int
get_esmreq_protoie_value(struct proto_IE *value, struct esm_req_Q_msg * g_esmReqInfo)
{
	value->no_of_IEs = ESM_REQ_NO_OF_IES;

	value->data = (proto_IEs *) malloc(ESM_REQ_NO_OF_IES *
			sizeof(proto_IEs));

	value->data[0].val.mme_ue_s1ap_id = g_esmReqInfo->ue_idx;
	value->data[1].val.enb_ue_s1ap_id = g_esmReqInfo->enb_s1ap_ue_id;

	return SUCCESS;
}


/**
* Stage specific message processing.
*/
static int
esmreq_processing(struct esm_req_Q_msg * g_esmReqInfo)
{
    Buffer g_esm_value_buffer = {0};
    Buffer g_esm_buffer = {0};
	unsigned char tmpStr[4];
	struct s1ap_PDU s1apPDU = {0};
	log_msg(LOG_INFO, "ESM Info Request handler MME-s1ap-id %d, enb-s1ap-id %d, fd %d ",
			g_esmReqInfo->ue_idx, g_esmReqInfo->enb_s1ap_ue_id, g_esmReqInfo->enb_fd);

	s1apPDU.procedurecode = id_downlinkNASTransport;
	s1apPDU.criticality = CRITICALITY_IGNORE;

	get_esmreq_protoie_value(&s1apPDU.value, g_esmReqInfo);

	/* Copy values in g_sec_value_buffer */
	g_esm_value_buffer.pos = 0;

	/* TODO remove hardcoded values */
	char chProtoIENo[3] = {0,0,3};
	buffer_copy(&g_esm_value_buffer, chProtoIENo, 3);

	/* id-MME-UE-S1AP-ID */
	uint16_t protocolIe_Id = id_MME_UE_S1AP_ID;
	copyU16(tmpStr, protocolIe_Id);
	buffer_copy(&g_esm_value_buffer, tmpStr,
					sizeof(protocolIe_Id));

	unsigned char protocolIe_criticality = CRITICALITY_REJECT;
	buffer_copy(&g_esm_value_buffer, &protocolIe_criticality,
					sizeof(protocolIe_criticality));

	unsigned char datalen = 2;

	/* TODO need to add proper handling*/
	unsigned char mme_ue_id[3];
	datalen = copyU16(mme_ue_id, s1apPDU.value.data[0].val.mme_ue_s1ap_id);
	buffer_copy(&g_esm_value_buffer, &datalen, sizeof(datalen));
	buffer_copy(&g_esm_value_buffer, mme_ue_id, datalen);

	/* id-eNB-UE-S1AP-ID */
	protocolIe_Id = id_eNB_UE_S1AP_ID;
	copyU16(tmpStr, protocolIe_Id);
	buffer_copy(&g_esm_value_buffer, tmpStr,
						sizeof(protocolIe_Id));

	buffer_copy(&g_esm_value_buffer, &protocolIe_criticality,
					sizeof(protocolIe_criticality));

	/* TODO needs proper handling*/
	unsigned char enb_ue_id[3];
	datalen = copyU16(enb_ue_id, s1apPDU.value.data[1].val.enb_ue_s1ap_id);
	buffer_copy(&g_esm_value_buffer, &datalen, sizeof(datalen));
	buffer_copy(&g_esm_value_buffer, enb_ue_id, datalen);


	/* id-NAS-PDU */
	protocolIe_Id = id_NAS_PDU;
	copyU16(tmpStr, protocolIe_Id);
	buffer_copy(&g_esm_value_buffer, tmpStr,
			sizeof(protocolIe_Id));

	buffer_copy(&g_esm_value_buffer, &protocolIe_criticality,
			sizeof(protocolIe_criticality));


	datalen = g_esmReqInfo->nasMsgSize + 1; 

	buffer_copy(&g_esm_value_buffer, &datalen,
						sizeof(datalen));

	buffer_copy(&g_esm_value_buffer, &g_esmReqInfo->nasMsgSize, sizeof(uint8_t));

	buffer_copy(&g_esm_value_buffer, &g_esmReqInfo->nasMsgBuf[0], g_esmReqInfo->nasMsgSize);

	/* Copy values in g_sec_buffer */
	g_esm_buffer.pos = 0;

	unsigned char initiating_message = 0; /* TODO: Add enum */
	buffer_copy(&g_esm_buffer, &initiating_message,
			sizeof(initiating_message));

	buffer_copy(&g_esm_buffer, &s1apPDU.procedurecode,
			sizeof(s1apPDU.procedurecode));

	buffer_copy(&g_esm_buffer, &s1apPDU.criticality,
			sizeof(s1apPDU.criticality));

	buffer_copy(&g_esm_buffer, &g_esm_value_buffer.pos,
			sizeof(uint8_t));

	buffer_copy(&g_esm_buffer, &g_esm_value_buffer,
			g_esm_value_buffer.pos);

	int ret = send_sctp_msg(g_esmReqInfo->enb_fd, g_esm_buffer.buf, g_esm_buffer.pos, 1);
	if(ret < 0) {
	log_msg(LOG_ERROR, "Failed to send - ESM Info Request. MME-s1ap-id %d, enb-s1ap-id %d, fd %d ",
			g_esmReqInfo->ue_idx, g_esmReqInfo->enb_s1ap_ue_id, g_esmReqInfo->enb_fd);
	}

	free(s1apPDU.value.data);

	return SUCCESS;
}

void*
esmreq_handler(void *data)
{
	esmreq_processing((struct esm_req_Q_msg *)data);

	return NULL;
}
