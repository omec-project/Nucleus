/*
 * Copyright 2019-present Open Networking Foundation
 *
 * SPDX-License-Identifier: Apache-2.0
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
#include "options.h"
#include "ipc_api.h"
#include "main.h"
#include "s1ap.h"
#include "s1ap_config.h"
#include "sctp_conn.h"
#include "s1ap_structs.h"
#include "s1ap_msg_codes.h"
#include "msgType.h"
extern s1ap_config g_s1ap_cfg;
extern ipc_handle ipc_S1ap_Hndl;
static Buffer g_buffer = {0};
static int
get_tau_rsp_protoie_value(struct proto_IE *value, struct tauResp_Q_msg *g_tauRespInfo)
{
    
	value->no_of_IEs = TAU_RSP_NO_OF_IES;

	value->data = (proto_IEs *) malloc(TAU_RSP_NO_OF_IES*
			sizeof(proto_IEs));

	value->data[0].val.mme_ue_s1ap_id = g_tauRespInfo->ue_idx;
	value->data[1].val.enb_ue_s1ap_id = g_tauRespInfo->s1ap_enb_ue_id;

	log_msg(LOG_INFO, "mme_ue_s1ap_id %d and enb_ue_s1ap_id %d\n",
			g_tauRespInfo->ue_idx, g_tauRespInfo->s1ap_enb_ue_id);

#ifdef S1AP_ENCODE_NAS
	/* TODO: Add enum for security header type */
	value->data[2].val.nas.header.security_header_type = IntegrityProtectedCiphered;
	value->data[2].val.nas.header.proto_discriminator = EPSMobilityManagementMessages;
    if(g_tauRespInfo->status == 0)
	  value->data[2].val.nas.header.message_type = TauAccept;
    else
	  value->data[2].val.nas.header.message_type = TauReject;
	value->data[2].val.nas.header.nas_security_param = 0;
	/* placeholder for mac. mac value will be calculated later */
	uint8_t mac[MAC_SIZE] = {0};
	memcpy(value->data[2].val.nas.header.mac, mac, MAC_SIZE);
	value->data[2].val.nas.header.seq_no = g_tauRespInfo->dl_seq_no;
	value->data[2].val.nas.header.eps_bearer_identity = 0;
	value->data[2].val.nas.header.procedure_trans_identity = 1;
	value->data[2].val.nas.elements_len = TAU_RSP_NO_OF_NAS_IES;
	value->data[2].val.nas.elements = (nas_pdu_elements *) malloc(TAU_RSP_NO_OF_NAS_IES * sizeof(nas_pdu_elements));
	nas_pdu_elements *nasIEs = value->data[2].val.nas.elements;
	uint8_t nasIeCnt = 0;
	nasIEs[nasIeCnt].pduElement.eps_res = 0; /* TA updated */
	nasIeCnt++;
	nasIEs[nasIeCnt].pduElement.spare = 0; /* TA updated */
	nasIeCnt++;
#endif

	return SUCCESS;
}

static int
tau_rsp_processing(struct tauResp_Q_msg *g_tauRespInfo)
{
	struct s1ap_PDU s1apPDU = {0};
	uint8_t s1ap_len_pos;
	uint8_t datalen;
	uint8_t u8value;

    if(g_tauRespInfo->status != 0)
    {
	  /* Assigning values to s1apPDU */
	  s1apPDU.procedurecode = id_errorIndication;
	  s1apPDU.criticality = CRITICALITY_IGNORE;
	  get_tau_rsp_protoie_value(&s1apPDU.value,g_tauRespInfo);
          g_buffer.pos = 0;

	  uint8_t initiating_message = 0; /* TODO: Add enum */
	  buffer_copy(&g_buffer, &initiating_message,
	  		sizeof(initiating_message));

	  buffer_copy(&g_buffer, &s1apPDU.procedurecode,
	  		sizeof(s1apPDU.procedurecode));

	  buffer_copy(&g_buffer, &s1apPDU.criticality,
	  		sizeof(s1apPDU.criticality));

	  s1ap_len_pos = g_buffer.pos;

	  /* length of S1ap message */
	  u8value = 0;
	  buffer_copy(&g_buffer, &u8value, sizeof(u8value));

	  /* TODO remove hardcoded values */
	  unsigned char chProtoIENo[3] = {0,0,2};

	  buffer_copy(&g_buffer, chProtoIENo, 3);

	  unsigned char tmpStr[4];

      /* id-eNB-UE-S1AP-ID */

	  uint16_t protocolIe_Id = id_eNB_UE_S1AP_ID;
	  uint8_t protocolIe_criticality = CRITICALITY_REJECT;
	  copyU16(tmpStr, protocolIe_Id);
	  buffer_copy(&g_buffer, tmpStr,
	  					sizeof(protocolIe_Id));

	  buffer_copy(&g_buffer, &protocolIe_criticality,
	  				sizeof(protocolIe_criticality));


	  /* TODO needs proper handling*/
	  unsigned char enb_ue_id[3];
	  datalen = copyU16(enb_ue_id,
	  		s1apPDU.value.data[1].val.enb_ue_s1ap_id);
	  buffer_copy(&g_buffer, &datalen, sizeof(datalen));
	  buffer_copy(&g_buffer, enb_ue_id, datalen);

	  unsigned char cause[6] = {0,2,40,2,4, 0};
	  buffer_copy(&g_buffer, cause, sizeof(cause));

	  /* Copy length to s1ap length field */
	  datalen = g_buffer.pos - s1ap_len_pos - 1;
	  memcpy(g_buffer.buf + s1ap_len_pos, &datalen, sizeof(datalen));
   	  send_sctp_msg(g_tauRespInfo->enb_fd, g_buffer.buf, g_buffer.pos,1);
      free(s1apPDU.value.data);
      return E_FAIL;
    }

	/* Assigning values to s1apPDU */
	s1apPDU.procedurecode = id_downlinkNASTransport;
	s1apPDU.criticality = CRITICALITY_IGNORE;

	get_tau_rsp_protoie_value(&s1apPDU.value,g_tauRespInfo);
 
	/* Copy values to buffer from s1apPDU */

	g_buffer.pos = 0;

	uint8_t initiating_message = 0; /* TODO: Add enum */
	buffer_copy(&g_buffer, &initiating_message,
			sizeof(initiating_message));

	buffer_copy(&g_buffer, &s1apPDU.procedurecode,
			sizeof(s1apPDU.procedurecode));

	buffer_copy(&g_buffer, &s1apPDU.criticality,
			sizeof(s1apPDU.criticality));

	s1ap_len_pos = g_buffer.pos;

	/* length of S1ap message */
	u8value = 0;
	buffer_copy(&g_buffer, &u8value, sizeof(u8value));

	/* TODO remove hardcoded values */
	unsigned char chProtoIENo[3] = {0,0,3};

	buffer_copy(&g_buffer, chProtoIENo, 3);

	unsigned char tmpStr[4];

	/* id-MME-UE-S1AP-ID */
	uint16_t protocolIe_Id = id_MME_UE_S1AP_ID;
	copyU16(tmpStr, protocolIe_Id);
	buffer_copy(&g_buffer, tmpStr,
					sizeof(protocolIe_Id));

	uint8_t protocolIe_criticality = CRITICALITY_REJECT;
	buffer_copy(&g_buffer, &protocolIe_criticality,
					sizeof(protocolIe_criticality));

	/* TODO needs proper handling*/
	unsigned char mme_ue_id[3];
	datalen = copyU16(mme_ue_id,
			s1apPDU.value.data[0].val.mme_ue_s1ap_id);
	buffer_copy(&g_buffer, &datalen, sizeof(datalen));
	buffer_copy(&g_buffer, mme_ue_id, datalen);

	/* id-eNB-UE-S1AP-ID */

	protocolIe_Id = id_eNB_UE_S1AP_ID;
	copyU16(tmpStr, protocolIe_Id);
	buffer_copy(&g_buffer, tmpStr,
						sizeof(protocolIe_Id));

	buffer_copy(&g_buffer, &protocolIe_criticality,
					sizeof(protocolIe_criticality));

	/* TODO needs proper handling*/
	unsigned char enb_ue_id[3];
	datalen = copyU16(enb_ue_id,
			s1apPDU.value.data[1].val.enb_ue_s1ap_id);
	buffer_copy(&g_buffer, &datalen, sizeof(datalen));
	buffer_copy(&g_buffer, enb_ue_id, datalen);

	/* id-NAS-PDU */
	protocolIe_Id = id_NAS_PDU;
	copyU16(tmpStr, protocolIe_Id);
	buffer_copy(&g_buffer, tmpStr,
						sizeof(protocolIe_Id));
	buffer_copy(&g_buffer, &protocolIe_criticality,
					sizeof(protocolIe_criticality));

#ifdef S1AP_ENCODE_NAS
	uint8_t mac_data_pos;
	uint8_t nas_len_pos;

	nas_len_pos = g_buffer.pos;
	datalen = 0;
	buffer_copy(&g_buffer, &datalen, sizeof(datalen));
	buffer_copy(&g_buffer, &datalen, sizeof(datalen));

	struct nasPDU *nas = &(s1apPDU.value.data[2].val.nas);
	struct nas_pdu_header *nas_hdr = &(s1apPDU.value.data[2].val.nas.header);

    u8value = (nas->header.security_header_type << 4) |
			nas->header.proto_discriminator;
	buffer_copy(&g_buffer, &u8value, sizeof(u8value));

	/* mac */
	/* placeholder for mac. mac value will be calculated later */
	buffer_copy(&g_buffer, nas_hdr->mac, MAC_SIZE);
	mac_data_pos = g_buffer.pos;

	/* sequence number */
	buffer_copy(&g_buffer, &(nas_hdr->seq_no),
			sizeof(nas_hdr->seq_no));

	/* security header and protocol discriminator */
	nas_hdr->security_header_type = Plain;
	u8value = (0 << 4 | nas_hdr->proto_discriminator);
	buffer_copy(&g_buffer, &u8value, sizeof(u8value));

	/* message type */
	buffer_copy(&g_buffer, &nas->header.message_type,
						sizeof(nas->header.message_type));


	u8value = 0; 
	buffer_copy(&g_buffer, &u8value, sizeof(u8value));
	u8value = 0x5a; 
	buffer_copy(&g_buffer, &u8value, sizeof(u8value));
#define DISABLE_TAU 0
#if DISABLE_TAU
	u8value = 0xe0; 
#else
	u8value = 0x21; 
#endif
	buffer_copy(&g_buffer, &u8value, sizeof(u8value));

#if 1 
    /* adding GUTI */
	u8value = 0x50; /* element id TODO: define macro or enum */
	buffer_copy(&g_buffer, &u8value, sizeof(u8value));
	datalen = 11;
	buffer_copy(&g_buffer, &datalen, sizeof(datalen));
	u8value = 246; /* TODO: remove hard coding */
	buffer_copy(&g_buffer, &u8value, sizeof(u8value));


    buffer_copy(&g_buffer, &g_tauRespInfo->tai.plmn_id, 3);

    uint16_t grpid = htons(g_s1ap_cfg.mme_group_id);
	buffer_copy(&g_buffer, &grpid, sizeof(grpid)); 

    u8value = g_s1ap_cfg.mme_code;
	buffer_copy(&g_buffer, &u8value, sizeof(u8value));

    uint32_t mtmsi = htonl(g_tauRespInfo->m_tmsi); 
	buffer_copy(&g_buffer, &(mtmsi), sizeof(mtmsi));
#endif



#if 1
    /*TODO : Experiment */
	u8value = 0x23; /* element id TODO: define macro or enum */
	buffer_copy(&g_buffer, &u8value, sizeof(u8value));
	datalen = 0x05;
	buffer_copy(&g_buffer, &datalen, sizeof(datalen));
    
    u8value = 0xf4; //
	buffer_copy(&g_buffer, &u8value, sizeof(u8value));

    mtmsi = htonl(g_tauRespInfo->ue_idx); 
	buffer_copy(&g_buffer, &(mtmsi), sizeof(mtmsi));
#endif
	/* NAS PDU end */

	/* Calculate mac */
	uint8_t direction = 1;
	uint8_t bearer = 0;

	calculate_aes_mac(g_tauRespInfo->int_key, g_tauRespInfo->dl_count,
			direction, bearer, &g_buffer.buf[mac_data_pos],
			g_buffer.pos - mac_data_pos,
			&g_buffer.buf[mac_data_pos - MAC_SIZE]);

	
	/* Copy nas length to nas length field */
	datalen = g_buffer.pos - nas_len_pos - 1;
	memcpy(&g_buffer.buf[nas_len_pos], &datalen, sizeof(datalen));

	/* Copy nas length to nas length field */
	datalen = g_buffer.pos - nas_len_pos - 2;
	memcpy(&(g_buffer.buf[nas_len_pos + 1]), &datalen, sizeof(datalen));

	/* Copy length to s1ap length field */
	datalen = g_buffer.pos - s1ap_len_pos - 1;
	memcpy(g_buffer.buf + s1ap_len_pos, &datalen, sizeof(datalen));
#else
	log_msg(LOG_INFO, "Received TAU response from mme-app. Nas message %d \n",g_tauRespInfo->nasMsgSize);
	datalen = g_tauRespInfo->nasMsgSize + 1; 

	buffer_copy(&g_buffer, &datalen,
						sizeof(datalen));

	buffer_copy(&g_buffer, &g_tauRespInfo->nasMsgSize, sizeof(uint8_t));

	buffer_copy(&g_buffer, &g_tauRespInfo->nasMsgBuf[0], g_tauRespInfo->nasMsgSize);
#endif

   	send_sctp_msg(g_tauRespInfo->enb_fd, g_buffer.buf, g_buffer.pos,1);
	log_msg(LOG_INFO, "\nTAU RESP received from MME\n");
    free(s1apPDU.value.data);
	return SUCCESS;
}


void*
tau_response_handler(void *data)
{

	log_msg(LOG_INFO, "TAU response handler ready.\n");

	tau_rsp_processing((struct tauResp_Q_msg *)data);
	
	return NULL;
}
