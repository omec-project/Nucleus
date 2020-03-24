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

#include "log.h"
#include "err_codes.h"
#include "ipc_api.h"
#include "s11_structs.h"
#include "msgType.h"

#include "gtpv2c.h"
#include "gtpv2c_ie.h"
#include <gtpV2StackWrappers.h>

/************************************************************************
Current file : Stage 7 handler. To listen MB from mme-app and fwd to CP
ATTACH stages :
	Stage 1 : IAM-->[stage1 handler]-->AIR, ULR
	Stage 2 : AIA, ULA -->[stage2 handler]--> Auth req
	Stage 3 : Auth resp-->[stage3 handler]-->Sec mode cmd
	Stage 4 : sec mode resp-->[stage4 handler]-->esm infor req
	Stage 5 : esm infor resp-->[stage5 handler]-->create session
	Stage 6 : create session resp-->[stage6 handler]-->init ctx setup
-->	Stage 7 : attach complete-->[stage7 handler]-->modify bearer
**************************************************************************/

/****Globals and externs ***/

extern int g_s11_fd;
extern struct sockaddr_in g_s11_cp_addr;
extern socklen_t g_s11_serv_size;
/*TODO: S11 protocol sequence number - need to make it atomic. multiple thread to access this*/
extern volatile uint32_t g_s11_sequence;

/*TODO: S11 protocol sequence number - need to make it atomic. multiple thread to access this*/
extern volatile uint32_t g_s11_sequence;

struct MsgBuffer*  mbReqMsgBuf_p = NULL;
extern struct GtpV2Stack* gtpStack_gp;
/****Global and externs end***/
/**
* Stage specific message processing.
*/
static int
modify_bearer_processing(struct MB_Q_msg *mb_msg)
{
	GtpV2MessageHeader gtpHeader;
	gtpHeader.msgType = GTP_MODIFY_BEARER_REQ;
	gtpHeader.sequenceNumber = g_s11_sequence;
	gtpHeader.teidPresent = true;
	gtpHeader.teid = mb_msg->s11_sgw_c_fteid.header.teid_gre;

	g_s11_sequence++;

	ModifyBearerRequestMsgData msgData;
	memset(&msgData, 0, sizeof(msgData));
	msgData.bearerContextsToBeModifiedCount = 1;
	msgData.bearerContextsToBeModified[0].epsBearerId.epsBearerId = 5;
	msgData.bearerContextsToBeModified[0].s1EnodebFTeidIePresent = true;
	msgData.bearerContextsToBeModified[0].s1EnodebFTeid.ipv4present = true;
	msgData.bearerContextsToBeModified[0].s1EnodebFTeid.interfaceType = mb_msg->s1u_enb_fteid.header.iface_type;
	msgData.bearerContextsToBeModified[0].s1EnodebFTeid.teidGreKey = mb_msg->s1u_enb_fteid.header.teid_gre;
	msgData.bearerContextsToBeModified[0].s1EnodebFTeid.ipV4Address.ipValue = mb_msg->s1u_enb_fteid.ip.ipv4.s_addr;

	GtpV2Stack_buildGtpV2Message(gtpStack_gp, mbReqMsgBuf_p, &gtpHeader, &msgData);
	sendto(g_s11_fd,
			MsgBuffer_getDataPointer(mbReqMsgBuf_p),
			MsgBuffer_getBufLen(mbReqMsgBuf_p), 0,
			(struct sockaddr*)&g_s11_cp_addr,
			g_s11_serv_size);
	//TODO " error chk, eagain etc?	
	log_msg(LOG_INFO, "Modify beader send, len - %d bytes.\n", MsgBuffer_getBufLen(mbReqMsgBuf_p));

	MsgBuffer_reset(mbReqMsgBuf_p);

	return SUCCESS;
}

/**
* Thread function for stage.
*/
void*
modify_bearer_handler(void *data)
{
	log_msg(LOG_INFO, "Modify bearer handler initialized\n");
	
	modify_bearer_processing((struct MB_Q_msg *)data);

	return NULL;
}
