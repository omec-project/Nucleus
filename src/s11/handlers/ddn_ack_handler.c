/*
 * Copyright (c) 2019, Infosys Ltd.
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
#include "s11_config.h"
#include "s11_options.h"
#include <gtpV2StackWrappers.h>


/****Globals and externs ***/

/*S11 CP communication parameters*/
extern int g_s11_fd;
extern s11_config_t g_s11_cfg;
extern struct sockaddr_in g_s11_cp_addr;
extern socklen_t g_s11_serv_size;

/*TODO: S11 protocol sequence number - need to make it atomic. multiple thread to access this*/
extern volatile uint32_t g_s11_sequence;


extern struct GtpV2Stack* gtpStack_gp;
struct thread_pool *g_tpool;

/****Global and externs end***/

/**
* Stage specific message processing.
*/
static int
ddn_ack_processing(struct DDN_ACK_Q_msg *ddn_ack_msg)
{
	struct MsgBuffer* ddnAckMsgBuf_p = createMsgBuffer(S11_MSGBUF_SIZE);
	if(ddnAckMsgBuf_p == NULL)
	{
	    log_msg(LOG_ERROR, "Error in initializing msg buffers required by gtp codec.\n");
            return -1;
	}
	GtpV2MessageHeader gtpHeader;
	gtpHeader.msgType =  GTP_DOWNLINK_DATA_NOTIFICATION_ACK;
	gtpHeader.sequenceNumber = ddn_ack_msg->seq_no;
	gtpHeader.teidPresent = true;
	gtpHeader.teid = ddn_ack_msg->s11_sgw_c_fteid.header.teid_gre;
    struct sockaddr_in sgw_ip = {0};
    create_sock_addr(&sgw_ip, g_s11_cfg.egtp_def_port, ddn_ack_msg->s11_sgw_c_fteid.ip.ipv4.s_addr);

	DownlinkDataNotificationAcknowledgeMsgData msgData;
	memset(&msgData, 0, sizeof(DownlinkDataNotificationAcknowledgeMsgData));


	msgData.cause.causeValue = ddn_ack_msg->cause;
	

	GtpV2Stack_buildGtpV2Message(gtpStack_gp, ddnAckMsgBuf_p, &gtpHeader, &msgData);


	sendto(g_s11_fd,
			MsgBuffer_getDataPointer(ddnAckMsgBuf_p),
			MsgBuffer_getBufLen(ddnAckMsgBuf_p), 0,
			(struct sockaddr*)&sgw_ip, g_s11_serv_size);
	
	log_msg(LOG_INFO, "DDN Ack Sent, len - %d bytes.\n", MsgBuffer_getBufLen(ddnAckMsgBuf_p));
	MsgBuffer_free(ddnAckMsgBuf_p);
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

