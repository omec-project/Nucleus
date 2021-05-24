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

/****Globals and externs ***/

/*S11 CP communication parameters*/
extern int g_s10_fd;
extern struct sockaddr_in g_s10_cp_addr;
extern socklen_t g_s10_serv_size;

extern s11_config_t g_s10_cfg;

/****Global and externs end***/
struct FWD_ACC_CTXT_ACK_Q_msg * g_facaReqInfo;

extern struct GtpV2Stack* gtpStack_gp;

/**
* Stage specific message processing.
*/
static int
fwd_acc_ctxt_ack_processing(struct FWD_ACC_CTXT_ACK_Q_msg * g_facaReqInfo)
{
	struct MsgBuffer*  fwdMsgBuf_p = createMsgBuffer(S10_MSGBUF_SIZE);
    if(fwdMsgBuf_p == NULL)
    {
        log_msg(LOG_ERROR, "Error in initializing msg buffers required by gtp codec.");
        return -1;
    }
    struct sockaddr_in mme_addr = {0};
    GtpV2MessageHeader gtpHeader;
	gtpHeader.msgType = GTP_FORWARD_ACCESS_CONTEXT_ACKNOWLEDGE;
    uint32_t seq = 0;
	get_sequence(&seq);
	gtpHeader.sequenceNumber = seq;
	gtpHeader.teidPresent = true;
	gtpHeader.teid = 0; 

	mme_addr.sin_family = AF_INET;
	mme_addr.sin_port = htons(g_s10_cfg.egtp_def_port);
    if(g_facaReqInfo->mme_ip != 0) {
    	mme_addr.sin_addr.s_addr = g_facaReqInfo->mme_ip;
    } else {
    	mme_addr = g_s10_cp_addr;
    }
	
	log_msg(LOG_INFO,"In forward access context ack handler->ue_idx:%d",g_facaReqInfo->ue_idx);

    add_gtp_transaction(gtpHeader.sequenceNumber,
    		g_facaReqInfo->ue_idx);
    ForwardAccessContextAcknowledgeMsgData msgData;
	memset(&msgData, 0, sizeof(msgData));

	GtpV2Stack_buildGtpV2Message(gtpStack_gp, fwdMsgBuf_p, &gtpHeader, &msgData);

	log_msg(LOG_INFO, "send %d bytes.",MsgBuffer_getBufLen(fwdMsgBuf_p));

	int res = sendto (
			g_s10_fd,
			MsgBuffer_getDataPointer(fwdMsgBuf_p),
			MsgBuffer_getBufLen(fwdMsgBuf_p), 0,
			(struct sockaddr*)(&mme_addr),
			g_s10_serv_size);
	if (res < 0) {
		log_msg(LOG_ERROR,"Error in sendto in detach stage 3 post to next");
	}

	log_msg(LOG_INFO,"%d bytes sent. Err : %d, %s",res,errno,
			strerror(errno));

	MsgBuffer_free(fwdMsgBuf_p);

	return SUCCESS;
}

/**
* Thread function for stage.
*/
void*
forward_access_context_ack_handler(void *data)
{
	log_msg(LOG_INFO, "Forward Access Context Ack handler");

	fwd_acc_ctxt_ack_processing((struct FWD_ACC_CTXT_ACK_Q_msg *) data);

	return NULL;
}
