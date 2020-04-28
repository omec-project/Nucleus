/*
 * Copyright (c) 2019, Infosys Ltd.
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
get_ni_detach_request_protoie_value(struct proto_IE *value, struct ni_detach_request_Q_msg *g_acptReqInfo)
{
	uint8_t ieCnt = 0;

	value->no_of_IEs = NI_DTCH_REQUEST_NO_OF_IES;

	value->data = (proto_IEs *) malloc(NI_DTCH_REQUEST_NO_OF_IES *
			sizeof(proto_IEs));

	value->data[ieCnt].val.mme_ue_s1ap_id = g_acptReqInfo->ue_idx;
	ieCnt++;

	value->data[ieCnt].val.enb_ue_s1ap_id = g_acptReqInfo->enb_s1ap_ue_id;
	ieCnt++;

	struct nasPDU *nas = &(value->data[ieCnt].val.nas);
	nas->header.security_header_type = IntegrityProtectedCiphered;
	nas->header.proto_discriminator = EPSMobilityManagementMessages;

	/* placeholder for mac. mac value will be calculated later */
	uint8_t mac[MAC_SIZE] = {0};
	memcpy(nas->header.mac, mac, MAC_SIZE);

	nas->header.seq_no = g_acptReqInfo->dl_seq_no;
	nas->header.message_type = DetachRequest;
	nas->header.detach_type = 00000002;
	log_msg(LOG_DEBUG,"NAS Msg Type: %x\n",nas->header.message_type);

	ieCnt++;

	return SUCCESS;
}

/**
* Stage specific message processing.
*/
static int
ni_detach_request_processing(struct ni_detach_request_Q_msg *g_acptReqInfo)
{
	unsigned char tmpStr[4];
	struct s1ap_PDU s1apPDU = {0};
	uint16_t protocolIe_Id;
	uint8_t protocolIe_criticality = CRITICALITY_REJECT;
	uint8_t initiating_msg = 0;
	uint8_t datalen = 0;
	uint8_t s1ap_len_pos;
	uint8_t nas_len_pos;
	uint8_t u8value = 0;
	uint8_t mac_data_pos;

	Buffer g_acpt_buffer = {0};

	s1apPDU.procedurecode = id_downlinkNASTransport;
	s1apPDU.criticality = CRITICALITY_IGNORE;

	get_ni_detach_request_protoie_value(&s1apPDU.value, g_acptReqInfo);

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

	nas_len_pos = g_acpt_buffer.pos;

	datalen = 0;
	buffer_copy(&g_acpt_buffer, &datalen, sizeof(datalen));

	buffer_copy(&g_acpt_buffer, &datalen, sizeof(datalen));

	nas_pdu_header *nas_hdr = &(s1apPDU.value.data[2].val.nas.header);

	/* security header and protocol discriminator */
	u8value = (nas_hdr->security_header_type << 4 |
			nas_hdr->proto_discriminator);
	buffer_copy(&g_acpt_buffer, &u8value, sizeof(u8value));

	/* mac */
	/* placeholder for mac. mac value will be calculated later */
	buffer_copy(&g_acpt_buffer, nas_hdr->mac, MAC_SIZE);
	mac_data_pos = g_acpt_buffer.pos;

	/* sequence number */
	buffer_copy(&g_acpt_buffer, &(nas_hdr->seq_no),
			sizeof(nas_hdr->seq_no));

	/* security header and protocol discriminator */
	nas_hdr->security_header_type = Plain;
	u8value = (nas_hdr->security_header_type << 4 |
			nas_hdr->proto_discriminator);
	buffer_copy(&g_acpt_buffer, &u8value, sizeof(u8value));

	/* message type */
	buffer_copy(&g_acpt_buffer, &(nas_hdr->message_type),
			sizeof(nas_hdr->message_type));	
	/* detach type */
        buffer_copy(&g_acpt_buffer, &(nas_hdr->detach_type),
                        sizeof(nas_hdr->detach_type));

	/* NAS PDU end */

	/* Calculate mac */
	uint8_t direction = 1;
	uint8_t bearer = 0;

	calculate_mac(g_acptReqInfo->int_key, nas_hdr->seq_no,
			direction, bearer, &g_acpt_buffer.buf[mac_data_pos],
			g_acpt_buffer.pos - mac_data_pos,
			&g_acpt_buffer.buf[mac_data_pos - MAC_SIZE]);

	/* Copy nas length to nas length field */
	datalen = g_acpt_buffer.pos - nas_len_pos -1;
	memcpy(&(g_acpt_buffer.buf[nas_len_pos]), &datalen, sizeof(datalen));

	/* Copy nas length to nas length field */
	datalen = g_acpt_buffer.pos - nas_len_pos - 2;
	memcpy(&(g_acpt_buffer.buf[nas_len_pos + 1]), &datalen, sizeof(datalen));

	/* Copy length to s1ap length field */
	datalen = g_acpt_buffer.pos - s1ap_len_pos - 1;
	memcpy(g_acpt_buffer.buf + s1ap_len_pos, &datalen, sizeof(datalen));

	/* TODO: temp fix */
	//g_ics_buffer.buf[1] = 0x09;
	send_sctp_msg(g_acptReqInfo->enb_fd, g_acpt_buffer.buf, g_acpt_buffer.pos,1);

	log_msg(LOG_INFO, "NI Detach Request sent to UE.");

	return SUCCESS;
}



/**
* Thread function for stage.
*/
void*
ni_detach_request_handler(void *data)
{
   log_msg(LOG_INFO, "NI detach request handler ready.\n");
   struct ni_detach_request_Q_msg *msg = (struct ni_detach_request_Q_msg *)data;

   ni_detach_request_processing(msg);
   
   return NULL;
}

