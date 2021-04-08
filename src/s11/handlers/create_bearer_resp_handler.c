/*
 * Copyright (c) 2020, Infosys Ltd.
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

/****Globals and externs ***/

/*S11 CP communication parameters*/
extern int g_s11_fd;
extern struct sockaddr_in g_s11_cp_addr;
extern s11_config_t g_s11_cfg;
extern socklen_t g_s11_serv_size;

struct thread_pool *g_tpool;

extern struct GtpV2Stack* gtpStack_gp;

/****Global and externs end***/

/**
* Stage specific message processing.
*/
static int
cb_resp_processing(struct CB_RESP_Q_msg *cb_resp_msg)
{
	
	struct MsgBuffer*  cbRespMsgBuf_p = createMsgBuffer(S11_MSGBUF_SIZE);
	if(cbRespMsgBuf_p == NULL)
	{
        log_msg(LOG_ERROR, "Error in initializing msg buffers required by gtp codec.");
        return -1;
    }
	
	GtpV2MessageHeader gtpHeader;
	gtpHeader.msgType =  GTP_CREATE_BEARER_RSP;
	gtpHeader.sequenceNumber = cb_resp_msg->seq_no;
	gtpHeader.teidPresent = true;
	gtpHeader.teid = cb_resp_msg->s11_sgw_c_fteid.header.teid_gre;
    struct sockaddr_in sgw_ip = {0};
    create_sock_addr(&sgw_ip, cb_resp_msg->destination_port,
                    cb_resp_msg->s11_sgw_c_fteid.ip.ipv4.s_addr);

	CreateBearerResponseMsgData msgData;
	memset(&msgData, 0, sizeof(CreateBearerResponseMsgData));

	msgData.cause.causeValue = cb_resp_msg->cause;
        
	msgData.bearerContextsCount = cb_resp_msg->bearer_ctxt_cb_resp_list.bearers_count;
	for(int i = 0; i < msgData.bearerContextsCount; i++)
	{
        msgData.bearerContexts[i].epsBearerId.epsBearerId =
                cb_resp_msg->bearer_ctxt_cb_resp_list.bearer_ctxt[i].eps_bearer_id;

        msgData.bearerContexts[i].cause.causeValue =
                cb_resp_msg->bearer_ctxt_cb_resp_list.bearer_ctxt[i].cause.cause;

        msgData.bearerContexts[i].s1UEnodebFTeidIePresent = true;
        msgData.bearerContexts[i].s1UEnodebFTeid.ipv4present =
                cb_resp_msg->bearer_ctxt_cb_resp_list.bearer_ctxt[i].s1u_enb_fteid.header.v4;
        msgData.bearerContexts[i].s1UEnodebFTeid.interfaceType =
                cb_resp_msg->bearer_ctxt_cb_resp_list.bearer_ctxt[i].s1u_enb_fteid.header.iface_type;
        msgData.bearerContexts[i].s1UEnodebFTeid.teidGreKey =
                cb_resp_msg->bearer_ctxt_cb_resp_list.bearer_ctxt[i].s1u_enb_fteid.header.teid_gre;
        msgData.bearerContexts[i].s1UEnodebFTeid.ipV4Address.ipValue =
                cb_resp_msg->bearer_ctxt_cb_resp_list.bearer_ctxt[i].s1u_enb_fteid.ip.ipv4.s_addr;

        msgData.bearerContexts[i].s1USgwFTeidIePresent = true;
        msgData.bearerContexts[i].s1USgwFTeid.ipv4present =
                cb_resp_msg->bearer_ctxt_cb_resp_list.bearer_ctxt[i].s1u_sgw_teid.header.v4;
        msgData.bearerContexts[i].s1USgwFTeid.interfaceType =
                cb_resp_msg->bearer_ctxt_cb_resp_list.bearer_ctxt[i].s1u_sgw_teid.header.iface_type;
        msgData.bearerContexts[i].s1USgwFTeid.teidGreKey =
                cb_resp_msg->bearer_ctxt_cb_resp_list.bearer_ctxt[i].s1u_sgw_teid.header.teid_gre;
        msgData.bearerContexts[i].s1USgwFTeid.ipV4Address.ipValue =
                cb_resp_msg->bearer_ctxt_cb_resp_list.bearer_ctxt[i].s1u_sgw_teid.ip.ipv4.s_addr;

        if (cb_resp_msg->bearer_ctxt_cb_resp_list.bearer_ctxt[i].pco_from_ue_opt.pco_length
                > 0)
        {
            msgData.bearerContexts[i].protocolConfigurationOptionsIePresent =
                    true;
            msgData.bearerContexts[i].protocolConfigurationOptions.pcoValue.count =
                    cb_resp_msg->bearer_ctxt_cb_resp_list.bearer_ctxt[i].pco_from_ue_opt.pco_length;
            memcpy(
                    msgData.bearerContexts[i].protocolConfigurationOptions.pcoValue.values,
                    cb_resp_msg->bearer_ctxt_cb_resp_list.bearer_ctxt[i].pco_from_ue_opt.pco_options,
                    cb_resp_msg->bearer_ctxt_cb_resp_list.bearer_ctxt[i].pco_from_ue_opt.pco_length);
        }

	}

	if(cb_resp_msg->pco.pco_length > 0)
	{
	    msgData.protocolConfigurationOptionsIePresent = true;
		msgData.protocolConfigurationOptions.pcoValue.count = cb_resp_msg->pco.pco_length;
	    memcpy(msgData.protocolConfigurationOptions.pcoValue.values, cb_resp_msg->pco.pco_options,
			    cb_resp_msg->pco.pco_length);
	}
	
	GtpV2Stack_buildGtpV2Message(gtpStack_gp, cbRespMsgBuf_p, &gtpHeader, &msgData);

	sendto(g_s11_fd,
			MsgBuffer_getDataPointer(cbRespMsgBuf_p),
			MsgBuffer_getBufLen(cbRespMsgBuf_p), 0,
			(struct sockaddr*)&sgw_ip, g_s11_serv_size);

	log_msg(LOG_INFO, "Create Bearer Resp Sent, len - %d bytes.", MsgBuffer_getBufLen(cbRespMsgBuf_p));
	MsgBuffer_free(cbRespMsgBuf_p);
	return SUCCESS;
}


/**
* Thread function for stage.
*/
void*
create_bearer_resp_handler(void *data)
{
	log_msg(LOG_INFO, "Create bearer Response handler initialized");
	cb_resp_processing((struct CB_RESP_Q_msg *)data);
	return NULL;
}

