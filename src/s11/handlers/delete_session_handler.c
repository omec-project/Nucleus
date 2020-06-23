/*
 * Copyright 2019-present Open Networking Foundation
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

#include "log.h"
#include "err_codes.h"
#include "ipc_api.h"
#include "gtpv2c.h"
#include "gtpv2c_ie.h"
#include "msgType.h"
#include <gtpV2StackWrappers.h>
/************************************************************************
Current file : Stage 1 handler.
ATTACH stages :
	Stage 1 : detach request -->delete session
**************************************************************************/

/****Globals and externs ***/

/*S11 CP communication parameters*/
extern int g_s11_fd;
extern struct sockaddr_in g_s11_cp_addr;
extern socklen_t g_s11_serv_size;
extern volatile uint32_t g_s11_sequence;

struct thread_pool *g_tpool;

extern struct GtpV2Stack* gtpStack_gp;
extern volatile uint32_t g_s11_sequence;

struct MsgBuffer*  dsReqMsgBuf_p = NULL;
/****Global and externs end***/

/**
* Stage specific message processing.
*/
static int
delete_session_processing(struct DS_Q_msg *ds_msg)
{
	GtpV2MessageHeader gtpHeader;
	gtpHeader.msgType = GTP_DELETE_SESSION_REQ;
	gtpHeader.sequenceNumber = g_s11_sequence;
	gtpHeader.teidPresent = true;
	gtpHeader.teid = ds_msg->s11_sgw_c_fteid.header.teid_gre;

	DeleteSessionRequestMsgData msgData;
	memset(&msgData, 0, sizeof(DeleteSessionRequestMsgData));

	msgData.indicationFlagsIePresent = true;
	msgData.indicationFlags.iOI = true;

	msgData.linkedEpsBearerIdIePresent = true;
	msgData.linkedEpsBearerId.epsBearerId = ds_msg->bearer_id;

	GtpV2Stack_buildGtpV2Message(gtpStack_gp, dsReqMsgBuf_p, &gtpHeader, &msgData);
	g_s11_sequence++;

	sendto(g_s11_fd,
			MsgBuffer_getDataPointer(dsReqMsgBuf_p),
			MsgBuffer_getBufLen(dsReqMsgBuf_p), 0,
			(struct sockaddr*)&g_s11_cp_addr, g_s11_serv_size);
	log_msg(LOG_INFO, "Send delete session request\n");

	MsgBuffer_reset(dsReqMsgBuf_p);

	return SUCCESS;
}


/**
* Thread function for stage.
*/
void*
delete_session_handler(void *data)
{
	log_msg(LOG_INFO, "Delete session handler initialized\n");

        delete_session_processing((struct DS_Q_msg *)data);
	return NULL;
}

