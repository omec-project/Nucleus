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
extern volatile uint32_t g_s11_sequence;

struct thread_pool *g_tpool;

extern struct GtpV2Stack* gtpStack_gp;
extern volatile uint32_t g_s11_sequence;

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
        log_msg(LOG_ERROR, "Error in initializing msg buffers required by gtp codec.\n");
        return -1;
    }
	
	GtpV2MessageHeader gtpHeader;
	gtpHeader.msgType =  GTP_DELETE_BEARER_RESP;
	gtpHeader.sequenceNumber = g_s11_sequence;
	gtpHeader.teidPresent = true;
	gtpHeader.teid = db_resp_msg->s11_sgw_cp_teid;
    struct sockaddr_in sgw_ip = {0};
    create_sock_addr(&sgw_ip, g_s11_cfg.egtp_def_port,
                    db_resp_msg->s11_sgw_cp_teid);

	g_s11_sequence++;
	DeleteBearerResponseMsgData msgData;
	memset(&msgData, 0, sizeof(DeleteBearerRequestMsgData));

	msgData.linkedEpsBearerIdIePresent = true;
	msgData.linkedEpsBearerId.epsBearerId = db_resp_msg->linked_bearer_id;
    
	msgData.cause.causeValue = db_resp_msg->cause;

	if(db_resp_msg->bearerCtxList.bearers_count > 0)
	{
	    msgData.bearerContextsCount = db_resp_msg->bearerCtxList.bearers_count;

	    for(int i=0; i < msgData.bearerContextsCount; i++)
	    {
	        msgData.bearerContexts[i].epsBearerId.epsBearerId = db_resp_msg->bearerCtxList.bearerCtxt[i].eps_bearer_id;

		msgData.bearerContexts[i].cause.causeValue = db_resp_msg->bearerCtxList.bearerCtxt[i].cause.data;

		if(db_resp_msg->bearerCtxList.bearerCtxt[i].pco.pco_length > 0)
		{
		    msgData.bearerContexts[i].protocolConfigurationOptionsIePresent = true;

		    memcpy(msgData.bearerContexts[i].protocolConfigurationOptions.pcoValue.values, 
				    db_resp_msg->bearerCtxList.bearerCtxt[i].pco.pco_options, 
				    db_resp_msg->bearerCtxList.bearerCtxt[i].pco.pco_length);
		}
	    }
	}

	
    add_gtp_transaction(gtpHeader.sequenceNumber, db_resp_msg->ue_idx);
	GtpV2Stack_buildGtpV2Message(gtpStack_gp, dbRespMsgBuf_p, &gtpHeader, &msgData);

	sendto(g_s11_fd,
			MsgBuffer_getDataPointer(dbRespMsgBuf_p),
			MsgBuffer_getBufLen(dbRespMsgBuf_p), 0,
			(struct sockaddr*)&sgw_ip, g_s11_serv_size);

	log_msg(LOG_INFO, "Delete Bearer Resp Sent, len - %d bytes.\n", MsgBuffer_getBufLen(dbRespMsgBuf_p));
	MsgBuffer_free(dbRespMsgBuf_p);
	return SUCCESS;
}


/**
* Thread function for stage.
*/
void*
delete_bearer_resp_handler(void *data)
{
	log_msg(LOG_INFO, "Delete bearer Response handler initialized\n");
	db_resp_processing((struct DB_RESP_Q_msg *)data);
	return NULL;
}

