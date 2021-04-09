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
#include <endian.h>

#include "log.h"
#include "err_codes.h"
#include "ipc_api.h"
#include "s1ap_config.h"
#include "main.h"
#include "s1ap.h"
#include "msgType.h"

/**
* Get ProtocolIE value for ICS Request sent by mme-app
*/
static int
get_icsreq_protoie_value(struct proto_IE *value, struct init_ctx_req_Q_msg *g_icsReqInfo)
{
	uint8_t ieCnt = 0;

	value->no_of_IEs = ICS_REQ_NO_OF_IES;

	value->data = (proto_IEs *) calloc(ICS_REQ_NO_OF_IES ,
			sizeof(proto_IEs));
	

	value->data[ieCnt].val.mme_ue_s1ap_id = g_icsReqInfo->ue_idx;
	ieCnt++;

	value->data[ieCnt].val.enb_ue_s1ap_id = g_icsReqInfo->enb_s1ap_ue_id;
	ieCnt++;

	value->data[ieCnt].val.ue_aggrt_max_bit_rate.uEaggregateMaxBitRateDL =
			g_icsReqInfo->exg_max_dl_bitrate;
	value->data[ieCnt].val.ue_aggrt_max_bit_rate.uEaggregateMaxBitRateUL =
				g_icsReqInfo->exg_max_ul_bitrate;
	ieCnt++;

	/* E-RABToBeSetupItemCtxtSUReq start */
	erab_setup_item *e_rab = &(value->data[ieCnt].val.erab_to_be_setup_item_ctxt_su_req);
	/* TODO: Remove hardcoded values. */
	e_rab->e_RAB_ID = 1;
	e_rab->e_RAB_QoS_Params.qci = g_icsReqInfo->qci;
	e_rab->e_RAB_QoS_Params.arPrio.prioLevel = g_icsReqInfo->pl;
	e_rab->e_RAB_QoS_Params.arPrio.preEmptionCapab = g_icsReqInfo->pci;
	e_rab->e_RAB_QoS_Params.arPrio.preEmptionVulnebility = g_icsReqInfo->pvi;

	/*S1u information : transport layer addr and teid*/
	e_rab->transportLayerAddress = htonl(g_icsReqInfo->gtp_teid.ip.ipv4.s_addr);
	//e_rab->gtp_teid = htonl(g_icsReqInfo->gtp_teid.header.teid_gre);
	{
		char *dst = (char *)&(e_rab->gtp_teid);
		char *src = (char *)&(g_icsReqInfo->gtp_teid.header.teid_gre);
		memcpy(dst, src+3, 1);
		memcpy(dst+1, src+2, 1);
		memcpy(dst+2, src+1, 1);
		memcpy(dst+3, src, 1);
	}

    // ajaymerge ... check carefully this ieCnt 
	ieCnt++;
	/* NAS PDU values end */
	/* E-RABToBeSetupItemCtxtSUReq values end */


	/* TODO Get value of ue_sec_capabilities
	 *
	 * value->data[ieCnt].ue_sec_capabilities = ??
	 *
	 * */


	ieCnt++;

	/* TODO: remove hard coded value */
	/*char sec_key[32] = "abcdefghijklmnopqrstuvwxyz012345";
	memcpy(value->data[ieCnt].sec_key, sec_key,
			SECURITY_KEY_SIZE);
	*/

	memcpy(value->data[ieCnt].val.sec_key, g_icsReqInfo->sec_key,
			SECURITY_KEY_SIZE);

	ieCnt++;

	return SUCCESS;
}



/**
* Stage specific message processing.
*/
static int
icsreq_processing(struct init_ctx_req_Q_msg *g_icsReqInfo)
{

    Buffer g_ics_buffer = {0};
    Buffer g_s1ap_buffer = {0};
    Buffer g_rab1_buffer = {0};
    Buffer g_rab2_buffer = {0};

	unsigned char tmpStr[4];
	struct s1ap_PDU s1apPDU = {0};
	uint16_t protocolIe_Id;
	uint8_t protocolIe_criticality;
	uint8_t initiating_msg = 0;
	uint8_t datalen = 0;
	uint8_t u8value = 0;

	s1apPDU.procedurecode = id_InitialContextSetup;
	s1apPDU.criticality = CRITICALITY_REJECT;

	get_icsreq_protoie_value(&s1apPDU.value, g_icsReqInfo);

	g_ics_buffer.pos = 0;

	buffer_copy(&g_ics_buffer, &initiating_msg,
			sizeof(initiating_msg));

	buffer_copy(&g_ics_buffer, &s1apPDU.procedurecode,
			sizeof(s1apPDU.procedurecode));

	buffer_copy(&g_ics_buffer, &s1apPDU.criticality,
				sizeof(s1apPDU.criticality));

	/* TODO: revisit , why 128 (0x80) required */
#if 0
	s1ap_len_pos = g_ics_buffer.pos;
	u8value = 128;
	buffer_copy(&g_ics_buffer, &u8value, sizeof(u8value));

	


	u8value = 0;
	buffer_copy(&g_ics_buffer, &u8value, sizeof(u8value));
#endif

	g_s1ap_buffer.pos = 0;

	/* TODO remove hardcoded values */
	uint8_t chProtoIENo[3] = {0,0,6};
	if(g_icsReqInfo->ho_restrict_list_presence)
	    chProtoIENo[2] = 7;   // If HO Restriction List is present
	buffer_copy(&g_s1ap_buffer, chProtoIENo, 3);

	/* id-MME-UE-S1AP-ID */
	protocolIe_Id = id_MME_UE_S1AP_ID;
	copyU16(tmpStr, protocolIe_Id);
	buffer_copy(&g_s1ap_buffer, tmpStr, sizeof(protocolIe_Id));
	protocolIe_criticality = CRITICALITY_REJECT;
	buffer_copy(&g_s1ap_buffer, &protocolIe_criticality,
					sizeof(protocolIe_criticality));
	datalen = 2;
	/* TODO need to add proper handling*/
	unsigned char mme_ue_id[3];
	datalen = copyU16(mme_ue_id, s1apPDU.value.data[0].val.mme_ue_s1ap_id);
	buffer_copy(&g_s1ap_buffer, &datalen, sizeof(datalen));
	buffer_copy(&g_s1ap_buffer, mme_ue_id, datalen);

	/* id-eNB-UE-S1AP-ID */
	protocolIe_Id = id_eNB_UE_S1AP_ID;
	copyU16(tmpStr, protocolIe_Id);
	buffer_copy(&g_s1ap_buffer, tmpStr, sizeof(protocolIe_Id));
	buffer_copy(&g_s1ap_buffer, &protocolIe_criticality,
					sizeof(protocolIe_criticality));
	/* TODO needs proper handling*/
	unsigned char enb_ue_id[3];
	datalen = copyU16(enb_ue_id, s1apPDU.value.data[1].val.enb_ue_s1ap_id);
	buffer_copy(&g_s1ap_buffer, &datalen, sizeof(datalen));
	buffer_copy(&g_s1ap_buffer, enb_ue_id, datalen);

	protocolIe_Id = id_uEaggregatedMaximumBitrate;
	copyU16(tmpStr, protocolIe_Id);
	buffer_copy(&g_s1ap_buffer, tmpStr, sizeof(protocolIe_Id));
	buffer_copy(&g_s1ap_buffer, &protocolIe_criticality,
					sizeof(protocolIe_criticality));

	uint8_t maximum_bit_rate_dl = 0;
	uint8_t maximum_bit_rate_ul = 0;

	if(g_icsReqInfo->exg_max_dl_bitrate <= UINT32_MAX &&
			g_icsReqInfo->exg_max_ul_bitrate <= UINT32_MAX)
	{
		datalen = 10;

		maximum_bit_rate_dl = 0x18;
		maximum_bit_rate_ul = 0x60;

		buffer_copy(&g_s1ap_buffer, &datalen, sizeof(datalen));

		buffer_copy(&g_s1ap_buffer, &maximum_bit_rate_dl, sizeof(maximum_bit_rate_dl));

		uint32_t temp_bitrate = htonl(g_icsReqInfo->exg_max_dl_bitrate);
		memset(tmpStr, 0, sizeof(tmpStr));
		memcpy(tmpStr, &temp_bitrate, sizeof(temp_bitrate));

		buffer_copy(&g_s1ap_buffer, tmpStr, sizeof(tmpStr));

		temp_bitrate = 0;
		temp_bitrate = htonl(g_icsReqInfo->exg_max_ul_bitrate);
		memset(tmpStr, 0, sizeof(tmpStr));
		memcpy(tmpStr, &temp_bitrate, sizeof(temp_bitrate));

		buffer_copy(&g_s1ap_buffer, &maximum_bit_rate_ul,
					sizeof(maximum_bit_rate_ul));
		buffer_copy(&g_s1ap_buffer, tmpStr,
					sizeof(tmpStr));
	}
	else
	{
		//Extended UE-AMBR is included when data rate should be above 10 Gbps
		Buffer g_ext_buffer =
		{ 0 };

		/*
		 *  0.......         Extension of UEAggregateMaximumBitrate = 0 :Absent
		 *  .0......         iE-Extensions = 0 :Absent
		 *                   uEaggregateMaximumBitRateDL(BitRate):
		 *  ..100...         BitRate Value Range = 4 (offset from 1) :0 to 1099511627775
		 *  .....000         Padding = 000b
		 */
		maximum_bit_rate_dl = 0x20;
		if (g_icsReqInfo->ext_ue_ambr.ext_ue_ambr_DL >= EXT_UEAMBR_MIN
				|| g_icsReqInfo->ext_ue_ambr.ext_ue_ambr_UL >= EXT_UEAMBR_MIN)
		{
			/*
			 *  0.......         Extension of UEAggregateMaximumBitrate = 0 :Absent
			 *  .1......         iE-Extensions = 1 :Present
			 *                   uEaggregateMaximumBitRateDL(BitRate):
			 *  ..100...         BitRate Value Range = 4 (offset from 1) :0 to 1099511627775
			 *  .....000         Padding = 000b
			 */
			maximum_bit_rate_dl = 0x60;
		}
		buffer_copy(&g_ext_buffer, &maximum_bit_rate_dl,
				sizeof(maximum_bit_rate_dl));

		uint8_t *ue_ambr = NULL;
		uint64_t temp_bitrate = 0;

		temp_bitrate = htobe64(g_icsReqInfo->exg_max_dl_bitrate);
		ue_ambr = (uint8_t*) (&temp_bitrate);
		buffer_copy(&g_ext_buffer, (ue_ambr + 3), 5);

		/*
	 	 uEaggregateMaximumBitRateUL(BitRate):
		 * 100.....           BitRate Value Range = 4 :0 to 1099511627775
		 * ...00000           Padding = 00000b
		 */
		maximum_bit_rate_ul = 0x80;
		buffer_copy(&g_ext_buffer, &maximum_bit_rate_ul,
				sizeof(maximum_bit_rate_ul));
		temp_bitrate = htobe64(g_icsReqInfo->exg_max_ul_bitrate);
		ue_ambr = (uint8_t*) (&temp_bitrate);
		buffer_copy(&g_ext_buffer, (ue_ambr + 3), 5);

		if (g_icsReqInfo->ext_ue_ambr.ext_ue_ambr_DL >= EXT_UEAMBR_MIN
				|| g_icsReqInfo->ext_ue_ambr.ext_ue_ambr_UL >= EXT_UEAMBR_MIN)
		{

			if (g_icsReqInfo->ext_ue_ambr.ext_ue_ambr_DL >= EXT_UEAMBR_MIN
					&& g_icsReqInfo->ext_ue_ambr.ext_ue_ambr_UL >= EXT_UEAMBR_MIN)
			{
				// No: of ProtocolExtensionContainer = 1 : 1 + 1 = 2
				u8value = 0;
				buffer_copy(&g_ext_buffer, &u8value, sizeof(u8value));
				u8value = 1;
				buffer_copy(&g_ext_buffer, &u8value, sizeof(u8value));
			}
			else
			{
				// No: of ProtocolExtensionContainer = 1 : 1 + 0 = 1
				u8value = 0;
				buffer_copy(&g_ext_buffer, &u8value, sizeof(u8value));
				buffer_copy(&g_ext_buffer, &u8value, sizeof(u8value));
			}

			if (g_icsReqInfo->ext_ue_ambr.ext_ue_ambr_DL >= EXT_UEAMBR_MIN)
			{
				/*Extended UE-AMBR-DL*/
				// Protocol Id
				protocolIe_Id = htons(id_extended_uEaggregateMaximumBitRateDL);
				buffer_copy(&g_ext_buffer, &protocolIe_Id, sizeof(protocolIe_Id));

				// Criticality
				protocolIe_criticality = CRITICALITY_IGNORE;
				buffer_copy(&g_ext_buffer, &protocolIe_criticality,
						sizeof(protocolIe_criticality));

				datalen = 9;
				buffer_copy(&g_ext_buffer, &datalen, sizeof(datalen));

				/*
				 * 0.......            Extension of ExtendedBitRate = 0 :Absent
				 * .111....            ExtendedBitRate Value Range = 7 :10000000001 to 18446744073709551615
				 * ....0000            Padding = 0000b
				 */
				u8value = 0x70;
				buffer_copy(&g_ext_buffer, &u8value, sizeof(u8value));

				temp_bitrate = htobe64(
						g_icsReqInfo->ext_ue_ambr.ext_ue_ambr_DL - EXT_UEAMBR_MIN); // offset from the min value
				buffer_copy(&g_ext_buffer, &temp_bitrate, sizeof(temp_bitrate));
			}

			if (g_icsReqInfo->ext_ue_ambr.ext_ue_ambr_UL >= EXT_UEAMBR_MIN)
			{
				/*Extended UE-AMBR-UL*/
				protocolIe_Id = htons(id_extended_uEaggregateMaximumBitRateUL);
				buffer_copy(&g_ext_buffer, &protocolIe_Id, sizeof(protocolIe_Id));
				buffer_copy(&g_ext_buffer, &protocolIe_criticality,
					sizeof(protocolIe_criticality));

				datalen = 9;
				buffer_copy(&g_ext_buffer, &datalen, sizeof(datalen));

				u8value = 0x70;
				buffer_copy(&g_ext_buffer, &u8value, sizeof(u8value));

				temp_bitrate = htobe64(
						g_icsReqInfo->ext_ue_ambr.ext_ue_ambr_UL - EXT_UEAMBR_MIN); // offset from the min value
				buffer_copy(&g_ext_buffer, &temp_bitrate, sizeof(temp_bitrate));
			}
		}

		uint8_t ue_ambr_len = g_ext_buffer.pos;
		buffer_copy(&g_s1ap_buffer, &ue_ambr_len, sizeof(ue_ambr_len));
		buffer_copy(&g_s1ap_buffer, &g_ext_buffer.buf, g_ext_buffer.pos);
	}

	/* id-E-RABToBeSetupListCtxtSUReq */
	erab_setup_item *erab = &(s1apPDU.value.data[3].val.erab_to_be_setup_item_ctxt_su_req);
	protocolIe_Id = id_ERABToBeSetupListCtxtSUReq;
	copyU16(tmpStr, protocolIe_Id);
	buffer_copy(&g_s1ap_buffer, tmpStr, sizeof(protocolIe_Id));
	protocolIe_criticality = CRITICALITY_REJECT;
	buffer_copy(&g_s1ap_buffer, &protocolIe_criticality,
					sizeof(protocolIe_criticality));

    /* Lets put this in new buffer  */
    /*rab_len_1 */
#if 0
	erab_len_pos = g_s1ap_buffer.pos;
	datalen = 0;
	buffer_copy(&g_ics_buffer, &datalen, sizeof(datalen));
#endif
    g_rab1_buffer.pos = 0;

	buffer_copy(&g_rab1_buffer, &initiating_msg,
			sizeof(initiating_msg));

	protocolIe_Id = id_ERABToBeSetupItemCtxtSUReq;
	copyU16(tmpStr, protocolIe_Id);
	buffer_copy(&g_rab1_buffer, tmpStr, sizeof(protocolIe_Id));
	buffer_copy(&g_rab1_buffer, &protocolIe_criticality,
					sizeof(protocolIe_criticality));

    /*rab_len_2 */
#if 0
	erab_item_len_pos = g_rab1_buffer.pos;  
    datalen = 0;
	buffer_copy(&g_ics_buffer, &datalen, sizeof(datalen));
#endif

	/*
	buffer_copy(&g_ics_buffer, &(erab->e_RAB_ID),
			sizeof(erab->e_RAB_ID));
	*/
    g_rab2_buffer.pos = 0;
	/* TODO : Remove hardcoded value of erab id */
	u8value =69; // 0x45 //1;
	buffer_copy(&g_rab2_buffer, &u8value, sizeof(u8value));
	/* TODO: Need to revisit why add 00 before qci value? */
	u8value = 0;
	buffer_copy(&g_rab2_buffer, &u8value, sizeof(u8value));
	buffer_copy(&g_rab2_buffer, &(erab->e_RAB_QoS_Params.qci),
			sizeof(erab->e_RAB_QoS_Params.qci));
	buffer_copy(&g_rab2_buffer, &(erab->e_RAB_QoS_Params.arPrio),
			sizeof(erab->e_RAB_QoS_Params.arPrio));

	/* TODO: Revisit why we need to add 0f 80 before transport add? */

	u8value = 15;
	buffer_copy(&g_rab2_buffer, &u8value, sizeof(u8value));
	u8value = 128;
	buffer_copy(&g_rab2_buffer, &u8value, sizeof(u8value));

	buffer_copy(&g_rab2_buffer, &(erab->transportLayerAddress),
				sizeof(erab->transportLayerAddress));

	buffer_copy(&g_rab2_buffer, &(erab->gtp_teid),
				sizeof(erab->gtp_teid));

	/* E_RABToBeSetupListCtxtSUReq NAS PDU start */
    // at the end we will do.... rab2_buf + <nas_len> + nas_buffer 

#if 0
	nas_len_pos = g_rab2_buffer.pos;
	datalen = 0;
	buffer_copy(&g_ics_buffer, &datalen, sizeof(datalen));
#endif

	log_msg(LOG_INFO, "Received Nas message from mme-app size %d", g_icsReqInfo->nasMsgSize);
//	datalen = g_icsReqInfo->nasMsgSize + 1; 

//	buffer_copy(&g_rab2_buffer, &datalen, sizeof(datalen));

	log_msg(LOG_INFO, "RAB2 payload length before adding NAS %lu", g_rab2_buffer.pos);
	buffer_copy(&g_rab2_buffer, &g_icsReqInfo->nasMsgSize, sizeof(uint8_t));

	buffer_copy(&g_rab2_buffer, &g_icsReqInfo->nasMsgBuf[0], g_icsReqInfo->nasMsgSize);

	log_msg(LOG_INFO, "RAB2 payload length %lu", g_rab2_buffer.pos);
	log_msg(LOG_INFO, "RAB1 payload length before appending RAB2  %lu", g_rab1_buffer.pos);
    /* Now lets append rab2 to rab1 */ 
    if(g_rab2_buffer.pos <= 127)
    {
        datalen = g_rab2_buffer.pos;
	    buffer_copy(&g_rab1_buffer, &datalen, sizeof(datalen));
	    log_msg(LOG_INFO, "RAB1 payload length after adding rab2 lengh  %lu", g_rab1_buffer.pos);
    /* Now lets append rab2 to rab1 */ 
    }
    else
    {
        uint16_t rab2_pay_len  = g_rab2_buffer.pos | 0x8000; // set MSB to 1 
        unsigned char lenStr[2];
        lenStr[0] = rab2_pay_len >> 8;
        lenStr[1] = rab2_pay_len & 0xff;
	    buffer_copy(&g_rab1_buffer, lenStr, sizeof(lenStr));
	    log_msg(LOG_INFO, "RAB1 payload length after adding rab2 lengh  %lu", g_rab1_buffer.pos);
    }
	buffer_copy(&g_rab1_buffer, &g_rab2_buffer.buf[0], g_rab2_buffer.pos);


    /* rab1 + rab2 is appended */ 
    // rab1 is combined now ... 

    /*g_s1ap_buffer is having rab appended to it.. */

	log_msg(LOG_INFO, "RAB1 payload length %lu", g_rab1_buffer.pos);
	log_msg(LOG_INFO, "s1ap buffer payload length before appending RAB1 %lu", g_s1ap_buffer.pos);
    if(g_rab1_buffer.pos <= 127)
    {
        datalen = g_rab1_buffer.pos;
	    buffer_copy(&(g_s1ap_buffer), &datalen, sizeof(datalen));
	    log_msg(LOG_INFO, "s1ap buffer payload length after adding rab1 header %lu", g_s1ap_buffer.pos);
    }
    else
    {
        uint16_t rab1_pay_len  = g_rab1_buffer.pos | 0x8000; // set MSB to 1 
        unsigned char lenStr[2];
        lenStr[0] = rab1_pay_len >> 8;
        lenStr[1] = rab1_pay_len & 0xff;
	    buffer_copy(&g_s1ap_buffer, lenStr, sizeof(lenStr));
	    log_msg(LOG_INFO, "s1ap buffer payload length after adding rab1 header %lu", g_s1ap_buffer.pos);
    }
	buffer_copy(&g_s1ap_buffer, &g_rab1_buffer.buf[0], g_rab1_buffer.pos);
	log_msg(LOG_INFO, "s1ap buffer payload length after appending RAB1 %lu", g_s1ap_buffer.pos);
    /* RAB is appended to s1ap payload now */ 

	/* id-UESecurityCapabilities */
	char ue_sec_capab[5] = {0x1c, 0x00, 0x0c, 0x00, 0x00};
	protocolIe_Id = id_UESecurityCapabilities;
	copyU16(tmpStr, protocolIe_Id);
	buffer_copy(&g_s1ap_buffer, tmpStr, sizeof(protocolIe_Id));
	protocolIe_criticality = CRITICALITY_REJECT;
	buffer_copy(&g_s1ap_buffer, &protocolIe_criticality,
					sizeof(protocolIe_criticality));
	datalen = 5;
	buffer_copy(&g_s1ap_buffer, &datalen, sizeof(datalen));
	buffer_copy(&g_s1ap_buffer, ue_sec_capab, 5);

	protocolIe_Id = id_SecurityKey;
	copyU16(tmpStr, protocolIe_Id);
	buffer_copy(&g_s1ap_buffer, tmpStr, sizeof(protocolIe_Id));
	protocolIe_criticality = CRITICALITY_REJECT;
	buffer_copy(&g_s1ap_buffer, &protocolIe_criticality,
					sizeof(protocolIe_criticality));
	datalen = SECURITY_KEY_SIZE;
	buffer_copy(&g_s1ap_buffer, &datalen, sizeof(datalen));
	buffer_copy(&g_s1ap_buffer, s1apPDU.value.data[5].val.sec_key,
					SECURITY_KEY_SIZE);
	
	if (g_icsReqInfo->ho_restrict_list_presence)
    {
        protocolIe_Id = id_HandoverRestrictionList;
        copyU16(tmpStr, protocolIe_Id);
        buffer_copy(&g_s1ap_buffer, tmpStr, sizeof(protocolIe_Id));
        protocolIe_criticality = CRITICALITY_IGNORE;
        buffer_copy(&g_s1ap_buffer, &protocolIe_criticality,
                sizeof(protocolIe_criticality));
        datalen = 11;
        buffer_copy(&g_s1ap_buffer, &datalen, sizeof(datalen));
        u8value = 4;
        buffer_copy(&g_s1ap_buffer, &u8value, sizeof(u8value));
        buffer_copy(&g_s1ap_buffer,
                g_icsReqInfo->ho_restrict_list.serving_plmn.idx, 3); //servingPLMN field
        u8value = 0;
        buffer_copy(&g_s1ap_buffer, &u8value, sizeof(u8value)); //servingPLMN field end
        buffer_copy(&g_s1ap_buffer, &u8value, sizeof(u8value)); //ieExtensions Field start
        protocolIe_Id = htons(0x105);
        buffer_copy(&g_s1ap_buffer, &protocolIe_Id, sizeof(protocolIe_Id));
        buffer_copy(&g_s1ap_buffer, &protocolIe_criticality,
                sizeof(protocolIe_criticality));
        u8value = 1;  //Extension Field Element
        buffer_copy(&g_s1ap_buffer, &u8value, sizeof(u8value));
        u8value = g_icsReqInfo->ho_restrict_list.nr_restricted_in_eps; //nrRestrictioninEPSasSecondaryRAT
        buffer_copy(&g_s1ap_buffer, &u8value, sizeof(u8value));
    }

	/* Copy length to s1ap length field */
	//datalen = g_s1ap_buffer.pos - s1ap_len_pos - 1;
	//uint16_t s1aplen = g_s1ap_buffer.pos - s1ap_len_pos - 1;
	log_msg(LOG_INFO, "S1AP payload length %lu", g_s1ap_buffer.pos);
	uint16_t s1aplen = g_s1ap_buffer.pos;
    if(s1aplen <= 127 )
    {
        datalen = s1aplen; 
	    buffer_copy(&g_ics_buffer, &datalen, sizeof(datalen));
    }
    else
    {
        s1aplen  = g_s1ap_buffer.pos | 0x8000; // set MSB to 1
        unsigned char lenStr[2];
        lenStr[0] = s1aplen >> 8;
        lenStr[1] = s1aplen & 0xff;
	    buffer_copy(&g_ics_buffer, lenStr, sizeof(lenStr));
    }

    /* this is my final s1ap buffer */
	buffer_copy(&g_ics_buffer, &g_s1ap_buffer.buf[0], g_s1ap_buffer.pos);

	free(s1apPDU.value.data);

	send_sctp_msg(g_icsReqInfo->enb_fd, g_ics_buffer.buf, g_ics_buffer.pos, 1);
	log_msg(LOG_INFO,"Initial Context Setup Request sent successfully");
	return SUCCESS;
}

void*
icsreq_handler(void *data)
{
	log_msg(LOG_INFO, "icsreq handler ready.");


	icsreq_processing((struct init_ctx_req_Q_msg *)data);


	return NULL;
}

