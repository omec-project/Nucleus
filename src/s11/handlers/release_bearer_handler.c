/*
 * Copyright (c) 2019, Infosys Ltd.
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

extern struct GtpV2Stack* gtpStack_gp;


/****Global and externs end***/
/**
* Stage specific message processing.
*/
static int
release_bearer_processing(struct RB_Q_msg *rb_msg)
{
    struct MsgBuffer*  rbReqMsgBuf_p = createMsgBuffer(S11_MSGBUF_SIZE);
    if(rbReqMsgBuf_p == NULL)
    {
	log_msg(LOG_ERROR, "Error in initializing msg buffers required by gtp codec.\n");
	return -1;
    }
    GtpV2MessageHeader gtpHeader;	
    gtpHeader.msgType = GTP_RABR_REQ;
    gtpHeader.sequenceNumber = g_s11_sequence;
    gtpHeader.teidPresent = true;
    gtpHeader.teid = rb_msg->s11_sgw_c_fteid.header.teid_gre;
    struct sockaddr_in sgw_ip = {0};
    create_sock_addr(&sgw_ip, g_s11_cfg.egtp_def_port,
                    rb_msg->s11_sgw_c_fteid.ip.ipv4.s_addr);
    g_s11_sequence++;

    ReleaseAccessBearersRequestMsgData msgData;
	memset(&msgData, 0, sizeof(msgData));
        
	msgData.indicationFlagsIePresent = true;
    msgData.indicationFlags.iOI = true;

    add_gtp_transaction(gtpHeader.sequenceNumber, rb_msg->ue_idx); 
    GtpV2Stack_buildGtpV2Message(gtpStack_gp, rbReqMsgBuf_p, &gtpHeader, &msgData);

    sendto(g_s11_fd,
           MsgBuffer_getDataPointer(rbReqMsgBuf_p),
           MsgBuffer_getBufLen(rbReqMsgBuf_p), 0,
           (struct sockaddr*)&sgw_ip,
           g_s11_serv_size);
    //TODO " error chk, eagain etc?
    log_msg(LOG_INFO, "Release Bearer sent, len - %d bytes.\n", MsgBuffer_getBufLen(rbReqMsgBuf_p));

    MsgBuffer_free(rbReqMsgBuf_p);

    return SUCCESS;

}


/**
* Thread function for stage.
*/
void*
release_bearer_handler(void *data)
{
	
	log_msg(LOG_INFO, "Release bearer handler initialized\n");
	
	release_bearer_processing((struct RB_Q_msg *)data);

	return NULL;
}


