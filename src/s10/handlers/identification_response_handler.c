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
#include "s10.h"
#include "s10_config.h"

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
struct ID_req_Q_msg *g_idResInfo;

extern struct GtpV2Stack* gtpStack_gp;
/*
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
*/

/**
* Stage specific message processing.
*/
static int
identification_response_processing(struct ID_RESP_Q_msg * g_idResInfo)
{
	struct MsgBuffer*  idResMsgBuf_p = createMsgBuffer(S10_MSGBUF_SIZE);
    if(idResMsgBuf_p == NULL)
    {
        log_msg(LOG_ERROR, "Error in initializing msg buffers required by gtp codec.");
        return -1;
    }
    struct sockaddr_in tmme_addr = {0};
    GtpV2MessageHeader gtpHeader;
	gtpHeader.msgType =  GTP_IDENTIFICATION_RES;
    uint32_t seq = 0;
	get_sequence(&seq);
	gtpHeader.sequenceNumber = seq;
	gtpHeader.teidPresent = true;
	gtpHeader.teid = 0; 

	tmme_addr.sin_family = AF_INET;
	tmme_addr.sin_port = htons(g_s10_cfg.egtp_def_port);
    if(g_idResInfo->target_mme_ip != 0) {
        tmme_addr.sin_addr.s_addr = g_idResInfo->target_mme_ip;
    } else {
        tmme_addr = g_s10_cp_addr;
    }
	
	log_msg(LOG_INFO,"In Identification response handler->ue_idx:%d",g_idResInfo->ue_idx);

    add_gtp_transaction(gtpHeader.sequenceNumber, 
                          g_idResInfo->ue_idx);
    IdentificationResponseMsgData msgData;
	memset(&msgData, 0, sizeof(msgData));

	msgData.cause.causeValue = g_idResInfo->cause ;
	//msgData.imsi.imsiValue.digits = g_idResInfo->
	//Need integer number from mme-app
	//msgData.ueUsageType.integerNumberValue =

	GtpV2Stack_buildGtpV2Message(gtpStack_gp, idResMsgBuf_p, &gtpHeader, &msgData);

	log_msg(LOG_INFO, "send %d bytes.",MsgBuffer_getBufLen(idResMsgBuf_p));

	int res = sendto (
			g_s10_fd,
			MsgBuffer_getDataPointer(idResMsgBuf_p),
			MsgBuffer_getBufLen(idResMsgBuf_p), 0,
			(struct sockaddr*)(&tmme_addr),
			g_s10_serv_size);
	if (res < 0) {
		log_msg(LOG_ERROR,"Error in sendto in detach stage 3 post to next");
	}

	log_msg(LOG_INFO,"%d bytes sent. Err : %d, %s",res,errno,
			strerror(errno));

	MsgBuffer_free(idResMsgBuf_p);

	return SUCCESS;
}

/**
* Thread function for stage.
*/
void*
identification_response_handler(void *data)
{
	log_msg(LOG_INFO, "Identification Response handler");

	identification_response_processing((struct ID_RESP_Q_msg *) data);

	return NULL;
}
