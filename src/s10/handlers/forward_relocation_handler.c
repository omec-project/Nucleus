/*
 * Copyright 2019-present Open Networking Foundation
 * Copyright (c) 2019, Infosys Ltd.
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
#include <errno.h>

#include "log.h"
#include "err_codes.h"
#include "ipc_api.h"
#include "msgType.h"

#include "gtpv2c.h"
#include "gtpv2c_ie.h"
#include "s11_config.h"
#include <gtpV2StackWrappers.h>
#include "gtp_cpp_wrapper.h"
#include "s11.h"
#include "s10_config.h"
#include "s10.h"


/************************************************************************
Current file : Stage 5 handler.
ATTACH stages :
	Stage 1 : IAM-->[stage1 handler]-->AIR, ULR
	Stage 2 : AIA, ULA -->[stage2 handler]--> Auth req
	Stage 3 : Auth resp-->[stage3 handler]-->Sec mode cmd
	Stage 4 : sec mode resp-->[stage4 handler]-->esm infor req
-->	Stage 5 : esm infor resp-->[stage5 handler]-->create session
	Stage 6 : create session resp-->[stage6 handler]-->init ctx setup
	Stage 7 : attach complete-->[stage7 handler]-->modify bearer
**************************************************************************/

/****Globals and externs ***/

/*S11 CP communication parameters*/

extern int g_s10_fd;
extern struct sockaddr_in g_s10_cp_addr;
extern socklen_t g_s10_serv_size;

extern s10_config_t g_s10_cfg;

/****Global and externs end***/
struct CS_Q_msg *g_csReqInfo;

struct FR_Q_msg *g_frReqInfo;

extern struct GtpV2Stack* gtpStack_gp;

void
bswap8_array(uint8_t *src, uint8_t *dest, uint32_t len)
{
	for (uint32_t i=0; i<len; i++)
		dest[i] = ((src[i] & 0x0F)<<4 | (src[i] & 0xF0)>>4);

	return;
}

uint32_t
convert_imsi_to_digits_array(uint8_t *src, uint8_t *dest, uint32_t len)
{
	uint8_t msb_digit = 0;
	uint8_t lsb_digit = 0;
	uint8_t num_of_digits = 0;

	for(uint32_t i = 0; i < len; i++)
	{
		lsb_digit = ((src[i] & 0xF0) >> 4);
		dest[(2*i) + 1] = lsb_digit;

		msb_digit = (src[i] & 0x0F);
		dest[2*i] = msb_digit;

		if (lsb_digit != 0x0F)
			num_of_digits = num_of_digits + 2;
		else
			num_of_digits++;
	}

	return num_of_digits;
}


/**
* Stage specific message processing.
*/
static int
forward_relocation_processing(struct forward_relocation_req_Q_msg * g_frReqInfo)
{
	struct MsgBuffer*  frReqMsgBuf_p = createMsgBuffer(S10_MSGBUF_SIZE);
    if(frReqMsgBuf_p == NULL)
    {
        log_msg(LOG_ERROR, "Error in initializing msg buffers required by gtp codec.");
        return -1;
    }
    struct sockaddr_in tmme_addr = {0};
    GtpV2MessageHeader gtpHeader;
	gtpHeader.msgType = GTP_FORWARD_RELOCATION_REQ;
    uint32_t seq = 0;
	get_sequence(&seq);
	gtpHeader.sequenceNumber = seq;
	gtpHeader.teidPresent = true;
	gtpHeader.teid = 0; // Need to check from 3gpp

	tmme_addr.sin_family = AF_INET;
	tmme_addr.sin_port = htons(g_s10_cfg.egtp_def_port);

    if(g_frReqInfo->target_mme_ip != 0) {
    	tmme_addr.sin_addr.s_addr = g_frReqInfo->target_mme_ip;
    } else {
    	tmme_addr = g_s10_cp_addr;
    }

	log_msg(LOG_INFO,"In Forward Relocation handler->ue_idx:%d",g_frReqInfo->ue_idx);

    add_gtp_transaction(gtpHeader.sequenceNumber, 
                          g_frReqInfo->ue_idx);

    ForwardRelocationRequestMsgData msgData;
	memset(&msgData, 0, sizeof(msgData));

	msgData.imsiIePresent = true;
	memset(msgData.imsi.imsiValue.digits, 0x0f, 16);
	

	uint8_t imsi_len =
			convert_imsi_to_digits_array(g_frReqInfo->IMSI,
					msgData.imsi.imsiValue.digits,
					BINARY_IMSI_LEN);

	printf("************IMSI value= %s************\n", msgData.imsi.imsiValue.digits);

	msgData.imsi.imsiValue.length = imsi_len;
	log_msg(LOG_INFO, "IMSI Len: %d", imsi_len);

	msgData.indicationFlagsIePresent = true;

	// Sender's FTEID for Control Plane
	msgData.senderFTeidForControlPlane.ipv4present = true;
	msgData.senderFTeidForControlPlane.interfaceType = 12; //12: S10 MME GTP-C interface
	msgData.senderFTeidForControlPlane.ipV4Address.ipValue = g_s10_cfg.target_mme_ip;
	msgData.senderFTeidForControlPlane.teidGreKey = g_frReqInfo->ue_idx;

//Target Identification
	//msgData.targetIdentification.targetId =   ; //refer pn 323
	msgData.targetIdentification.targetType = 1; //Macro eNodeB ID : from 3gpp 129.274 v15 PN 321 :reference from logs


//F-container
	msgData.eUtranTransparentContainer.containerType = 3; //E-Utran Transparent ccontaainer: pn:319
	msgData.eUtranTransparentContainer.fContainerField = 0 ; //Coming from s1-ap pn :319


//MM Context
	msgData.mmeSgsnAmfUeMmContext.securityMode = 4; //EPS Security Context and Quadruplets ,PN 314
	msgData.mmeSgsnAmfUeMmContext.nhiPresent = 1 ;
	msgData.mmeSgsnAmfUeMmContext.drxiPresent = 1 ;
	msgData.mmeSgsnAmfUeMmContext.ksiAsme =0 ;
	msgData.mmeSgsnAmfUeMmContext.numberOfQuintuplets = 0;
	msgData.mmeSgsnAmfUeMmContext.numberOfQuadruplet = 1;
	msgData.mmeSgsnAmfUeMmContext.uambriPresent = 1;
	msgData.mmeSgsnAmfUeMmContext.osciPresent = 0 ;
	msgData.mmeSgsnAmfUeMmContext.sambriPresent = 1;
	msgData.mmeSgsnAmfUeMmContext.usedNasIntegrity = 1 ; //3gpp 129.274 v15 Table 8.38-4
	msgData.mmeSgsnAmfUeMmContext.usedNasCipher = 0;
	msgData.mmeSgsnAmfUeMmContext.nasDownlinkCount = 5;
	msgData.mmeSgsnAmfUeMmContext.nasUplinkCount = 4 ;
	//msgData.mmeSgsnAmfUeMmContext.kAsme = ; //will get it from MME-app

	//Authentication Quadruplet
	msgData.mmeSgsnAmfUeMmContext.authenticationQuadruplet.count = 1 ;
	//msgData.mmeSgsnAmfUeMmContext.authenticationQuadruplet.values.rand = (unsigned int)g_frReqInfo->mm_cntxt.sec_vector.rand.val;
	msgData.mmeSgsnAmfUeMmContext.authenticationQuadruplet.values[1].xresLength = 8;
	//msgData.mmeSgsnAmfUeMmContext.authenticationQuadruplet.values.xres =
	msgData.mmeSgsnAmfUeMmContext.authenticationQuadruplet.values[3].autnLength = 16;
	//msgData.mmeSgsnAmfUeMmContext.authenticationQuadruplet.values.autn =
	//msgData.mmeSgsnAmfUeMmContext.authenticationQuadruplet.values.kAsme =


	msgData.mmeSgsnAmfUeMmContext.drxiPresent = 0;
	//msgData.mmeSgsnAmfUeMmContext.nh =
	//msgData.mmeSgsnAmfUeMmContext.uplinkSubscribedUeAmbr = ;
	//msgData.mmeSgsnAmfUeMmContext.downlinkSubscribedUeAmbr = ;
	//msgData.mmeSgsnAmfUeMmContext.uplinkUsedUeAmbr = 5071;
	//msgData.mmeSgsnAmfUeMmContext.downlinkUsedUeAmbr = 5071 ;

	//PDN Connection
	msgData.mmeSgsnUeScefPdnConnections.defaultEpsBearerId.epsBearerId = 5 ; //TS 124 007 V10.0.0
	//msgData.mmeSgsnUeScefPdnConnections.apn.apnValue =                     ; //3GPP TS 23.003 9.1 subclause
	msgData.mmeSgsnUeScefPdnConnections.apnRestriction.restrictionValue = 1 ; // 3gpp 129.274 v15 Table 8.57-1
	msgData.mmeSgsnUeScefPdnConnections.selectionMode.selectionMode = 0; // 3gpp 129.274 v15 Table 8.58-1:(MS or network provided APN, subscription verified)
	msgData.mmeSgsnUeScefPdnConnections.aggregateMaximumBitRate.maxMbrDownlink = 1024 ;//3GPP TS 24.301 Figure 9.9.4.2.
	msgData.mmeSgsnUeScefPdnConnections.aggregateMaximumBitRate.maxMbrUplink = 1024;
	//3GPP TS 32.251--charging characteristics
	//msgData.mmeSgsnUeScefPdnConnections.chargingCharacteristics.value =


	GtpV2Stack_buildGtpV2Message(gtpStack_gp, frReqMsgBuf_p, &gtpHeader, &msgData);

	log_msg(LOG_INFO, "send %d bytes.",MsgBuffer_getBufLen(frReqMsgBuf_p));

	int res = sendto (
			g_s10_fd,
			MsgBuffer_getDataPointer(frReqMsgBuf_p),
			MsgBuffer_getBufLen(frReqMsgBuf_p), 0,
			(struct sockaddr*)(&tmme_addr),
			g_s10_serv_size);
	if (res < 0) {
		log_msg(LOG_ERROR,"Error in sendto in detach stage 3 post to next");
	}

	log_msg(LOG_INFO,"%d bytes sent. Err : %d, %s",res,errno,
			strerror(errno));

	MsgBuffer_free(frReqMsgBuf_p);

	return SUCCESS;
}

/**
* Thread function for stage.
*/
void*
forward_relocation_handler(void *data)
{
	log_msg(LOG_INFO, "Forward Relocation Request handler");

	forward_relocation_processing((struct forward_relocation_req_Q_msg *) data);

	return NULL;
}
