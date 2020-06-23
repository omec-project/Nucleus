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
#include "log.h"
#include "main.h"
#include "s1ap.h"
#include "sctp_conn.h"
#include "msgType.h"
#include "snow_3g.h"

/**
* Get ProtocolIE value for Sec Request sent by mme-app
*/
static int
get_secreq_protoie_value(struct proto_IE *value, struct sec_mode_Q_msg * g_secReqInfo)
{
	value->no_of_IEs = SEC_MODE_NO_OF_IES;

	value->data = (proto_IEs *) malloc(SEC_MODE_NO_OF_IES *
			sizeof(proto_IEs));

	value->data[0].val.mme_ue_s1ap_id = g_secReqInfo->ue_idx;
	value->data[1].val.enb_ue_s1ap_id = g_secReqInfo->enb_s1ap_ue_id;

#ifdef S1AP_ENCODE_NAS
	value->data[2].val.nas.header.security_header_type =
			IntegrityProtectedEPSSecCntxt;

	value->data[2].val.nas.header.proto_discriminator =
			EPSMobilityManagementMessages;

	/* placeholder for mac. mac value will be calculated later */
	uint8_t mac[MAC_SIZE] = {0};
	memcpy(value->data[2].val.nas.header.mac, mac, MAC_SIZE);

	value->data[2].val.nas.header.seq_no = g_secReqInfo->dl_seq_no;

	value->data[2].val.nas.header.message_type = SecurityModeCommand;

	value->data[2].val.nas.header.security_encryption_algo = g_secReqInfo->sec_alg;

	value->data[2].val.nas.header.security_integrity_algo = g_secReqInfo->int_alg;

	/* Security Param (1 octet) =
	 * Spare half octet, Type of Security, NAS KSI
	 * TODO: Remove hard coded value
	 */
	value->data[2].val.nas.header.nas_security_param = AUTHREQ_NAS_SECURITY_PARAM;

	value->data[2].val.nas.elements_len = SEC_MODE_NO_OF_NAS_IES;

	value->data[2].val.nas.elements = (nas_pdu_elements *)
			malloc(SEC_MODE_NO_OF_NAS_IES * sizeof(nas_pdu_elements));

	value->data[2].val.nas.elements->pduElement.ue_network.len =
			g_secReqInfo->ue_network.len;
	if(g_secReqInfo->ue_network.len >= 4)
	{
        /*Copy first 4 bytes of security algo info*/
	    memcpy(value->data[2].val.nas.elements->pduElement.ue_network.capab, 
               g_secReqInfo->ue_network.capab, 4);
	   
        log_msg(LOG_DEBUG, "UE network length : %d", g_secReqInfo->ue_network.len);
        if(g_secReqInfo->ms_net_capab.pres == true)
	    {
	        /*The MS Network capability contains the GEA
		* capability. The MSB of 1st Byte and the 2nd to
		* 7th Bit of 2nd byte contain the GEA info.
		* Thus the masks 0x7F : for GEA/1
		* and mask 0x7D: for GEA2 -GEA7
		*/
            log_msg(LOG_DEBUG, "MS network present"); 
	        value->data[2].val.nas.elements->pduElement.ue_network.len = 5;
	    	unsigned char val = 0x00;
		    val = g_secReqInfo->ms_net_capab.capab[0]&0x80;
            val |= g_secReqInfo->ms_net_capab.capab[1]&0x7E;
            val >>= 1;
	        value->data[2].val.nas.elements->pduElement.ue_network.capab[4] = val;
	    }
	    else
	    {
	        /*If MS capability is not present. Then only 
		* Capability till UMTS Algorithms is sent.*/
            log_msg(LOG_DEBUG, "MS network not present"); 
	        value->data[2].val.nas.elements->pduElement.ue_network.len = 4;
	    }
	}
	else
	{
	    /*Copy as much info of UE network capability 
	    * as received.
	    */
            log_msg(LOG_DEBUG, "UE network length again: %d", g_secReqInfo->ue_network.len);
            memcpy(value->data[2].val.nas.elements->pduElement.ue_network.capab,
               g_secReqInfo->ue_network.capab, g_secReqInfo->ue_network.len);
	}
#endif

	return SUCCESS;
}


/**
* Stage specific message processing.
*/
static int
secreq_processing(struct sec_mode_Q_msg * g_secReqInfo)
{
	Buffer g_sec_buffer = {0};
	Buffer g_sec_value_buffer = {0};

	unsigned char tmpStr[4];
	struct s1ap_PDU s1apPDU = {0};

	s1apPDU.procedurecode = id_downlinkNASTransport;
	s1apPDU.criticality = CRITICALITY_IGNORE;

	get_secreq_protoie_value(&s1apPDU.value, g_secReqInfo);

	/* Copy values to g_sec_nas_buffer */

#ifdef S1AP_ENCODE_NAS
	/* id-NAS-PDU */
	Buffer g_sec_nas_buffer = {0};
	g_sec_nas_buffer.pos = 0;
	nasPDU nas = s1apPDU.value.data[2].val.nas;

	unsigned char value = (nas.header.security_header_type << 4 |
			nas.header.proto_discriminator);

	buffer_copy(&g_sec_nas_buffer, &value, sizeof(value));

	/* placeholder for mac. mac value will be calculated later */
	buffer_copy(&g_sec_nas_buffer, &nas.header.mac, MAC_SIZE);

	uint8_t mac_data_pos;
	mac_data_pos = g_sec_nas_buffer.pos;

	buffer_copy(&g_sec_nas_buffer, &nas.header.seq_no,
			sizeof(nas.header.seq_no));

	nas.header.security_header_type = Plain;
	value = nas.header.security_header_type |
				nas.header.proto_discriminator;
	buffer_copy(&g_sec_nas_buffer, &value, sizeof(value));

	buffer_copy(&g_sec_nas_buffer, &nas.header.message_type,
			sizeof(nas.header.message_type));

	value = (nas.header.security_encryption_algo << 4 |
				nas.header.security_integrity_algo);
	buffer_copy(&g_sec_nas_buffer, &value, sizeof(value));

	buffer_copy(&g_sec_nas_buffer, &nas.header.nas_security_param,
			sizeof(nas.header.nas_security_param));

	buffer_copy(&g_sec_nas_buffer, &nas.elements->pduElement.ue_network.len,
			sizeof(nas.elements->pduElement.ue_network.len));

	buffer_copy(&g_sec_nas_buffer, &nas.elements->pduElement.ue_network.capab,
			nas.elements->pduElement.ue_network.len);

    	/* Request IMEI from the device */
	uint8_t imei = 0xc1;
	buffer_copy(&g_sec_nas_buffer, &imei, sizeof(imei));

	/* Calculate mac */
	uint8_t direction = 1;
	uint8_t bearer = 0;

	calculate_aes_mac(g_secReqInfo->int_key, g_secReqInfo->dl_count,
			direction, bearer, &g_sec_nas_buffer.buf[mac_data_pos],
			g_sec_nas_buffer.pos - mac_data_pos,
			&g_sec_nas_buffer.buf[mac_data_pos - MAC_SIZE]);
#endif

	/* Copy values in g_sec_value_buffer */
	g_sec_value_buffer.pos = 0;

	/* TODO remove hardcoded values */
	char chProtoIENo[3] = {0,0,3};
	buffer_copy(&g_sec_value_buffer, chProtoIENo, 3);

	/* id-MME-UE-S1AP-ID */
	uint16_t protocolIe_Id = id_MME_UE_S1AP_ID;
	copyU16(tmpStr, protocolIe_Id);
	buffer_copy(&g_sec_value_buffer, tmpStr,
					sizeof(protocolIe_Id));

	unsigned char protocolIe_criticality = CRITICALITY_REJECT;
	buffer_copy(&g_sec_value_buffer, &protocolIe_criticality,
					sizeof(protocolIe_criticality));

	unsigned char datalen = 2;

	/* TODO need to add proper handling*/
	unsigned char mme_ue_id[3];
	datalen = copyU16(mme_ue_id, s1apPDU.value.data[0].val.mme_ue_s1ap_id);
	buffer_copy(&g_sec_value_buffer, &datalen, sizeof(datalen));
	buffer_copy(&g_sec_value_buffer, mme_ue_id, datalen);

	/* id-eNB-UE-S1AP-ID */
	protocolIe_Id = id_eNB_UE_S1AP_ID;
	copyU16(tmpStr, protocolIe_Id);
	buffer_copy(&g_sec_value_buffer, tmpStr,
						sizeof(protocolIe_Id));

	buffer_copy(&g_sec_value_buffer, &protocolIe_criticality,
					sizeof(protocolIe_criticality));

	/* TODO needs proper handling*/
	unsigned char enb_ue_id[3];
	datalen = copyU16(enb_ue_id, s1apPDU.value.data[1].val.enb_ue_s1ap_id);
	buffer_copy(&g_sec_value_buffer, &datalen, sizeof(datalen));
	buffer_copy(&g_sec_value_buffer, enb_ue_id, datalen);

	/* id-NAS-PDU */
	protocolIe_Id = id_NAS_PDU;
	copyU16(tmpStr, protocolIe_Id);
	buffer_copy(&g_sec_value_buffer, tmpStr,
			sizeof(protocolIe_Id));

	buffer_copy(&g_sec_value_buffer, &protocolIe_criticality,
			sizeof(protocolIe_criticality));


#ifdef S1AP_ENCODE_NAS
	datalen = g_sec_nas_buffer.pos + 1;
	buffer_copy(&g_sec_value_buffer, &datalen,
			sizeof(datalen));

	buffer_copy(&g_sec_value_buffer, &g_sec_nas_buffer.pos,
			sizeof(g_sec_nas_buffer.pos));

	buffer_copy(&g_sec_value_buffer, &g_sec_nas_buffer,
			g_sec_nas_buffer.pos);
#else
	log_msg(LOG_INFO, "Received Security Command  has nas message %d \n",g_secReqInfo->nasMsgSize);
	datalen = g_secReqInfo->nasMsgSize + 1; 

	buffer_copy(&g_sec_value_buffer, &datalen,
						sizeof(datalen));

	buffer_copy(&g_sec_value_buffer, &g_secReqInfo->nasMsgSize, sizeof(uint8_t));

	buffer_copy(&g_sec_value_buffer, &g_secReqInfo->nasMsgBuf[0], g_secReqInfo->nasMsgSize);

#endif

	/* Copy values in g_sec_buffer */
	g_sec_buffer.pos = 0;

	unsigned char initiating_message = 0; /* TODO: Add enum */
	buffer_copy(&g_sec_buffer, &initiating_message,
			sizeof(initiating_message));

	buffer_copy(&g_sec_buffer, &s1apPDU.procedurecode,
			sizeof(s1apPDU.procedurecode));

	buffer_copy(&g_sec_buffer, &s1apPDU.criticality,
			sizeof(s1apPDU.criticality));

	buffer_copy(&g_sec_buffer, &g_sec_value_buffer.pos,
			sizeof(g_sec_value_buffer.pos));

	buffer_copy(&g_sec_buffer, &g_sec_value_buffer,
			g_sec_value_buffer.pos);

	free(s1apPDU.value.data[2].val.nas.elements);
	free(s1apPDU.value.data);
	//STIMER_GET_CURRENT_TP(g_attach_stats[s1apPDU.value.data[1].enb_ue_s1ap_id].secreq_out);

	send_sctp_msg(g_secReqInfo->enb_fd, g_sec_buffer.buf, g_sec_buffer.pos, 1);

	return SUCCESS;
}

/**
* Thread function for stage.
*/
void*
secreq_handler(void *data)
{
	log_msg(LOG_INFO, "SecReq handler ready.\n");

	secreq_processing((struct sec_mode_Q_msg *)data);

	return NULL;
}
