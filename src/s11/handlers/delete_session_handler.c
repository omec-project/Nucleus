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
#include "s11_config.h"
#include "msgType.h"
#include "s11_options.h"
#include <gtpV2StackWrappers.h>
#include "gtp_cpp_wrapper.h"
#include "s11.h"
/************************************************************************
Current file : Stage 1 handler.
ATTACH stages :
	Stage 1 : detach request -->delete session
**************************************************************************/

/****Globals and externs ***/

/*S11 CP communication parameters*/
extern int g_s11_fd;
extern struct sockaddr_in g_s11_cp_addr;
extern s11_config_t g_s11_cfg;
extern socklen_t g_s11_serv_size;

struct thread_pool *g_tpool;

extern struct GtpV2Stack* gtpStack_gp;

/**
* Stage specific message processing.
*/
static int
delete_session_processing(struct DS_Q_msg *ds_msg)
{
	struct MsgBuffer*  dsReqMsgBuf_p = createMsgBuffer(S11_MSGBUF_SIZE);
	if(dsReqMsgBuf_p == NULL)
	{
	    log_msg(LOG_ERROR, "Error in initializing msg buffers required by gtp codec.");
            return -1;
	}
	GtpV2MessageHeader gtpHeader;
	gtpHeader.msgType = GTP_DELETE_SESSION_REQ;
    uint32_t seq = 0;
	get_sequence(&seq);
	gtpHeader.sequenceNumber = seq;
	gtpHeader.teidPresent = true;
	gtpHeader.teid = ds_msg->s11_sgw_c_fteid.header.teid_gre;
    struct sockaddr_in sgw_ip = {0};
    create_sock_addr(&sgw_ip, g_s11_cfg.egtp_def_port,
                    ds_msg->s11_sgw_c_fteid.ip.ipv4.s_addr);

	DeleteSessionRequestMsgData msgData;
	memset(&msgData, 0, sizeof(DeleteSessionRequestMsgData));

	msgData.indicationFlagsIePresent = true;
	msgData.indicationFlags.iOI = true;

	msgData.linkedEpsBearerIdIePresent = true;
	msgData.linkedEpsBearerId.epsBearerId = ds_msg->bearer_id;

    add_gtp_transaction(gtpHeader.sequenceNumber, ds_msg->ue_idx); 
	GtpV2Stack_buildGtpV2Message(gtpStack_gp, dsReqMsgBuf_p, &gtpHeader, &msgData);

	sendto(g_s11_fd,
			MsgBuffer_getDataPointer(dsReqMsgBuf_p),
			MsgBuffer_getBufLen(dsReqMsgBuf_p), 0,
			(struct sockaddr*)&sgw_ip, g_s11_serv_size);
	log_msg(LOG_INFO, "Send delete session request");

	MsgBuffer_free(dsReqMsgBuf_p);

	return SUCCESS;
}


/**
* Thread function for stage.
*/
void*
delete_session_handler(void *data)
{
	log_msg(LOG_INFO, "Delete session handler initialized");

        delete_session_processing((struct DS_Q_msg *)data);
	return NULL;
}

