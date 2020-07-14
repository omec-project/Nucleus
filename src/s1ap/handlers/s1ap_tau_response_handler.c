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

static int
get_tau_rsp_protoie_value(struct proto_IE *value, struct tauResp_Q_msg *g_tauRespInfo)
{
    
	value->no_of_IEs = TAU_RSP_NO_OF_IES;

	value->data = (proto_IEs *) calloc(TAU_RSP_NO_OF_IES,
			sizeof(proto_IEs));

	value->data[0].val.mme_ue_s1ap_id = g_tauRespInfo->ue_idx;
	value->data[1].val.enb_ue_s1ap_id = g_tauRespInfo->s1ap_enb_ue_id;

	log_msg(LOG_INFO, "mme_ue_s1ap_id %d and enb_ue_s1ap_id %d\n",
			g_tauRespInfo->ue_idx, g_tauRespInfo->s1ap_enb_ue_id);

	//free(value->data);

	return SUCCESS;
}

static int
tau_rsp_processing(struct tauResp_Q_msg *g_tauRespInfo)
{
	struct s1ap_PDU s1apPDU = {0};
	Buffer g_buffer = {0};
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
      if(s1apPDU.value.data[2].val.nas.elements)
      {
          free(s1apPDU.value.data[2].val.nas.elements);
      }

      if(s1apPDU.value.data)
      {
          free(s1apPDU.value.data);
          s1apPDU.value.data = NULL;
      }
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

	log_msg(LOG_INFO, "Received TAU response from mme-app. Nas message %d \n",g_tauRespInfo->nasMsgSize);
	datalen = g_tauRespInfo->nasMsgSize + 1; 

	buffer_copy(&g_buffer, &datalen,
						sizeof(datalen));

	buffer_copy(&g_buffer, &g_tauRespInfo->nasMsgSize, sizeof(uint8_t));

	buffer_copy(&g_buffer, &g_tauRespInfo->nasMsgBuf[0], g_tauRespInfo->nasMsgSize);
	datalen = g_buffer.pos - s1ap_len_pos - 1;
	memcpy(g_buffer.buf + s1ap_len_pos, &datalen, sizeof(datalen));

   	send_sctp_msg(g_tauRespInfo->enb_fd, g_buffer.buf, g_buffer.pos,1);
	log_msg(LOG_INFO, "\nTAU RESP received from MME\n");
    if(s1apPDU.value.data)
    {
        free(s1apPDU.value.data);
        s1apPDU.value.data = NULL;
    }
    return SUCCESS;
}


void*
tau_response_handler(void *data)
{

	log_msg(LOG_INFO, "TAU response handler ready.\n");

	tau_rsp_processing((struct tauResp_Q_msg *)data);
	
	return NULL;
}
