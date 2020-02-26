/*
 * Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
 * Copyright (c) 2017 Intel Corporation
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
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "log.h"
#include "err_codes.h"
#include "message_queues.h"
#include "ipc_api.h"
#include "gtpv2c.h"
#include "gtpv2c_ie.h"
#include "msgType.h"
#include "../../gtpV2Codec/gtpV2StackWrappers.h"
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
static char buf[S11_DTCHREQ_STAGE1_BUF_SIZE];

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

