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
db_resp_processing(struct DB_RESP_Q_msg *db_resp_msg)
{
	struct MsgBuffer*  dbRespMsgBuf_p = createMsgBuffer(S11_MSGBUF_SIZE);
	if(dbRespMsgBuf_p == NULL)
	{
        log_msg(LOG_ERROR, "Error in initializing msg buffers required by gtp codec.");
        return -1;
    }
	
	GtpV2MessageHeader gtpHeader;
	gtpHeader.msgType =  GTP_DELETE_BEARER_RESP;
	gtpHeader.sequenceNumber = db_resp_msg->seq_no;
	gtpHeader.teidPresent = true;
	gtpHeader.teid = db_resp_msg->s11_sgw_c_fteid.header.teid_gre;
    struct sockaddr_in sgw_ip = {0};
    create_sock_addr(&sgw_ip, db_resp_msg->destination_port,
                    db_resp_msg->s11_sgw_c_fteid.ip.ipv4.s_addr);

	DeleteBearerResponseMsgData msgData;
	memset(&msgData, 0, sizeof(DeleteBearerRequestMsgData));
    
	msgData.cause.causeValue = db_resp_msg->cause;

    if (db_resp_msg->bearer_ctxt_db_resp_list.bearers_count > 0)
    {
        msgData.bearerContextsCount =
                db_resp_msg->bearer_ctxt_db_resp_list.bearers_count;

        for (int i = 0; i < msgData.bearerContextsCount; i++)
        {
            msgData.bearerContexts[i].epsBearerId.epsBearerId =
                    db_resp_msg->bearer_ctxt_db_resp_list.bearer_ctxt[i].eps_bearer_id;

            msgData.bearerContexts[i].cause.causeValue =
                    db_resp_msg->bearer_ctxt_db_resp_list.bearer_ctxt[i].cause.cause;

            if (db_resp_msg->bearer_ctxt_db_resp_list.bearer_ctxt[i].pco_from_ue_opt.pco_length
                    > 0)
            {
                msgData.bearerContexts[i].protocolConfigurationOptionsIePresent =
                        true;
                msgData.bearerContexts[i].protocolConfigurationOptions.pcoValue.count =
                        db_resp_msg->bearer_ctxt_db_resp_list.bearer_ctxt[i].pco_from_ue_opt.pco_length;
                memcpy(
                        msgData.bearerContexts[i].protocolConfigurationOptions.pcoValue.values,
                        db_resp_msg->bearer_ctxt_db_resp_list.bearer_ctxt[i].pco_from_ue_opt.pco_options,
                        db_resp_msg->bearer_ctxt_db_resp_list.bearer_ctxt[i].pco_from_ue_opt.pco_length);
            }
        }
    }
    else if(db_resp_msg->linked_bearer_id)
    {
        msgData.linkedEpsBearerIdIePresent = true;
        msgData.linkedEpsBearerId.epsBearerId = db_resp_msg->linked_bearer_id;
    }

	GtpV2Stack_buildGtpV2Message(gtpStack_gp, dbRespMsgBuf_p, &gtpHeader, &msgData);

	sendto(g_s11_fd,
			MsgBuffer_getDataPointer(dbRespMsgBuf_p),
			MsgBuffer_getBufLen(dbRespMsgBuf_p), 0,
			(struct sockaddr*)&sgw_ip, g_s11_serv_size);

	log_msg(LOG_INFO, "Delete Bearer Resp Sent, len - %d bytes.", MsgBuffer_getBufLen(dbRespMsgBuf_p));
	MsgBuffer_free(dbRespMsgBuf_p);
	return SUCCESS;
}


/**
* Thread function for stage.
*/
void*
delete_bearer_resp_handler(void *data)
{
	log_msg(LOG_INFO, "Delete bearer Response handler initialized");
	db_resp_processing((struct DB_RESP_Q_msg *)data);
	return NULL;
}
