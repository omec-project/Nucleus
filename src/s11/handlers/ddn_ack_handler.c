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
#include "ipc_api.h"
#include "gtpv2c.h"
#include "gtpv2c_ie.h"
#include "msgType.h"
#include <gtpV2StackWrappers.h>


/****Globals and externs ***/

/*S11 CP communication parameters*/
extern int g_s11_fd;
extern struct sockaddr_in g_s11_cp_addr;
extern socklen_t g_s11_serv_size;

/*TODO: S11 protocol sequence number - need to make it atomic. multiple thread to access this*/
extern volatile uint32_t g_s11_sequence;


extern struct GtpV2Stack* gtpStack_gp;
struct thread_pool *g_tpool;

struct MsgBuffer* ddnAckMsgBuf_p = NULL;
/****Global and externs end***/

/**
* Stage specific message processing.
*/
static int
ddn_ack_processing(struct DDN_ACK_Q_msg *ddn_ack_msg)
{
	GtpV2MessageHeader gtpHeader;
	gtpHeader.msgType =  GTP_DOWNLINK_DATA_NOTIFICATION_ACK;
	gtpHeader.sequenceNumber = ddn_ack_msg->seq_no;
	gtpHeader.teidPresent = true;
	gtpHeader.teid = ddn_ack_msg->s11_sgw_cp_teid;

	DownlinkDataNotificationAcknowledgeMsgData msgData;
	memset(&msgData, 0, sizeof(DownlinkDataNotificationAcknowledgeMsgData));


	msgData.cause.causeValue = ddn_ack_msg->cause;
	

	GtpV2Stack_buildGtpV2Message(gtpStack_gp, ddnAckMsgBuf_p, &gtpHeader, &msgData);


	sendto(g_s11_fd,
			MsgBuffer_getDataPointer(ddnAckMsgBuf_p),
			MsgBuffer_getBufLen(ddnAckMsgBuf_p), 0,
			(struct sockaddr*)&g_s11_cp_addr, g_s11_serv_size);
	
	log_msg(LOG_INFO, "DDN Ack Sent, len - %d bytes.\n", MsgBuffer_getBufLen(ddnAckMsgBuf_p));
	MsgBuffer_reset(ddnAckMsgBuf_p);
	return SUCCESS;
}


/**
* Thread function for stage.
*/
void*
ddn_ack_handler(void *data)
{
	log_msg(LOG_INFO, "DDN Ack handler initialized\n");
    ddn_ack_processing((struct DDN_ACK_Q_msg *)data);
	return NULL;
}

