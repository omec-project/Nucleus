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
#include "s11_config.h"
#include "s11_options.h"
#include <gtpV2StackWrappers.h>
#include "gtp_cpp_wrapper.h"

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
extern s11_config_t g_s11_cfg;
extern socklen_t g_s11_serv_size;
/*TODO: S11 protocol sequence number - need to make it atomic. multiple thread to access this*/
extern volatile uint32_t g_s11_sequence;

/*TODO: S11 protocol sequence number - need to make it atomic. multiple thread to access this*/
extern volatile uint32_t g_s11_sequence;

extern struct GtpV2Stack* gtpStack_gp;
/****Global and externs end***/
/**
* Stage specific message processing.
*/
static int
modify_bearer_processing(struct MB_Q_msg *mb_msg)
{
	struct MsgBuffer*  mbReqMsgBuf_p = createMsgBuffer(S11_MSGBUF_SIZE);
	if(mbReqMsgBuf_p == NULL)
	{
	    log_msg(LOG_ERROR, "Error in initializing msg buffers required by gtp codec.\n");
            return -1;
	}
	GtpV2MessageHeader gtpHeader;
	gtpHeader.msgType = GTP_MODIFY_BEARER_REQ;
	gtpHeader.sequenceNumber = g_s11_sequence;
	gtpHeader.teidPresent = true;
	gtpHeader.teid = mb_msg->s11_sgw_c_fteid.header.teid_gre;
    struct sockaddr_in sgw_ip = {0};
    create_sock_addr(&sgw_ip, g_s11_cfg.egtp_def_port,
                    mb_msg->s11_sgw_c_fteid.ip.ipv4.s_addr);

	g_s11_sequence++;

	ModifyBearerRequestMsgData msgData;
	memset(&msgData, 0, sizeof(msgData));
	struct TAI *tai = &(mb_msg->tai);
	struct CGI *cgi = &(mb_msg->utran_cgi);

	if (mb_msg->servingNetworkIePresent)
    {
        msgData.servingNetworkIePresent = true;
        msgData.servingNetwork.mccDigit1 = tai->plmn_id.idx[0] & 0x0F;
        msgData.servingNetwork.mccDigit2 = (tai->plmn_id.idx[0] & 0xF0) >> 4;
        msgData.servingNetwork.mccDigit3 = tai->plmn_id.idx[1] & 0x0F;
        msgData.servingNetwork.mncDigit1 = tai->plmn_id.idx[2] & 0x0F;
        msgData.servingNetwork.mncDigit2 = (tai->plmn_id.idx[2] & 0xF0) >> 4;
        msgData.servingNetwork.mncDigit3 = (tai->plmn_id.idx[1] & 0xF0) >> 4;
    }

    if (mb_msg->userLocationInformationIePresent)
    {
        msgData.userLocationInformationIePresent = true;
        msgData.userLocationInformation.taipresent = true;
        msgData.userLocationInformation.ecgipresent = true;

        msgData.userLocationInformation.tai.trackingAreaCode = ntohs(tai->tac);
        msgData.userLocationInformation.tai.mccDigit1 = tai->plmn_id.idx[0]
                & 0x0F;
        msgData.userLocationInformation.tai.mccDigit2 = (tai->plmn_id.idx[0]
                & 0xF0) >> 4;
        msgData.userLocationInformation.tai.mccDigit3 = tai->plmn_id.idx[1]
                & 0x0F;
        msgData.userLocationInformation.tai.mncDigit1 = tai->plmn_id.idx[2]
                & 0x0F;
        msgData.userLocationInformation.tai.mncDigit2 = (tai->plmn_id.idx[2]
                & 0xF0) >> 4;
        msgData.userLocationInformation.tai.mncDigit3 = (tai->plmn_id.idx[1]
                & 0xF0) >> 4;

        msgData.userLocationInformation.ecgi.eUtranCellId = ntohl(cgi->cell_id)
                >> 4;
        msgData.userLocationInformation.ecgi.mccDigit1 = cgi->plmn_id.idx[0]
                & 0x0F;
        msgData.userLocationInformation.ecgi.mccDigit2 = (cgi->plmn_id.idx[0]
                & 0xF0) >> 4;
        msgData.userLocationInformation.ecgi.mccDigit3 = cgi->plmn_id.idx[1]
                & 0x0F;
        msgData.userLocationInformation.ecgi.mncDigit1 = cgi->plmn_id.idx[2]
                & 0x0F;
        msgData.userLocationInformation.ecgi.mncDigit2 = (cgi->plmn_id.idx[2]
                & 0xF0) >> 4;
        msgData.userLocationInformation.ecgi.mncDigit3 = (cgi->plmn_id.idx[1]
                & 0xF0) >> 4;
    }

	//TODO:Support dedicated bearer
	msgData.bearerContextsToBeModifiedCount = 1;
	msgData.bearerContextsToBeModified[0].epsBearerId.epsBearerId = 5;
	msgData.bearerContextsToBeModified[0].s1EnodebFTeidIePresent = true;
	msgData.bearerContextsToBeModified[0].s1EnodebFTeid.ipv4present = true;
	msgData.bearerContextsToBeModified[0].s1EnodebFTeid.interfaceType = mb_msg->s1u_enb_fteid.header.iface_type;
	msgData.bearerContextsToBeModified[0].s1EnodebFTeid.teidGreKey = mb_msg->s1u_enb_fteid.header.teid_gre;
	msgData.bearerContextsToBeModified[0].s1EnodebFTeid.ipV4Address.ipValue = mb_msg->s1u_enb_fteid.ip.ipv4.s_addr;

    add_gtp_transaction(gtpHeader.sequenceNumber, mb_msg->ue_idx); 
	GtpV2Stack_buildGtpV2Message(gtpStack_gp, mbReqMsgBuf_p, &gtpHeader, &msgData);
	sendto(g_s11_fd,
			MsgBuffer_getDataPointer(mbReqMsgBuf_p),
			MsgBuffer_getBufLen(mbReqMsgBuf_p), 0,
			(struct sockaddr*)&sgw_ip,
			g_s11_serv_size);
	//TODO " error chk, eagain etc?	
	log_msg(LOG_INFO, "Modify bearer sent, len - %d bytes.\n", MsgBuffer_getBufLen(mbReqMsgBuf_p));

	MsgBuffer_free(mbReqMsgBuf_p);

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
