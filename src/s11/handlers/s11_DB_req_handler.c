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

#include "err_codes.h"
#include "s11_options.h"
#include "ipc_api.h"
#include "s11.h"
#include "s11_config.h"
#include "msgType.h"

#include <gtpV2StackWrappers.h>
/*Globals and externs*/
extern int g_resp_fd;
extern struct GtpV2Stack* gtpStack_gp;
/*End : globals and externs*/

int
s11_DB_req_handler(MsgBuffer* message, GtpV2MessageHeader* hdr, uint32_t sgw_ip)
{

	struct gtp_incoming_msg_data_t dbr_info;

	dbr_info.ue_idx = hdr->teid;
	dbr_info.msg_type = delete_bearer_request;

	DeleteBearerRequestMsgData msgData;
	memset(&msgData, 0, sizeof(DeleteBearerRequestMsgData));

	bool rc = GtpV2Stack_decodeMessage(gtpStack_gp, hdr, message, &msgData);
	if(rc == false)
	{
			log_msg(LOG_ERROR, "s11_DB_req_handler: "
					"Failed to decode Delete Bearer Request Msg %d\n",
					hdr->teid);
			return E_PARSING_FAILED;
	}

	if (msgData.linkedEpsBearerIdIePresent)
	{
	    dbr_info.msg_data.db_req_Q_m.linked_bearer_id = msgData.linkedEpsBearerId.epsBearerId;
	}
        else if (msgData.epsBearerIdsCount > 0)
	{
	    /*Rewriting msgData.epsBearerIdsCount as 1, since we support only one bearer ctxt in the Q msg struct currently*/
	    msgData.epsBearerIdsCount = 1;
	    for(int i=0; i < msgData.epsBearerIdsCount; i++)
	    {
	    	dbr_info.msg_data.db_req_Q_m.bearerCtxList.bearerCtxt[i].eps_bearer_id =
	    			    				msgData.epsBearerIds[i].epsBearerId;
	    }
	}

	if(msgData.causeIePresent)
	{
	    dbr_info.msg_data.db_req_Q_m.cause = msgData.cause.causeValue;
	}

	dbr_info.msg_data.db_req_Q_m.pco.pco_length = 0;
        if(msgData.protocolConfigurationOptionsIePresent)
        {
            dbr_info.msg_data.db_req_Q_m.pco.pco_length = msgData.protocolConfigurationOptions.pcoValue.count;
            memcpy(dbr_info.msg_data.db_req_Q_m.pco.pco_options, &msgData.protocolConfigurationOptions.pcoValue.values[0],
                            msgData.protocolConfigurationOptions.pcoValue.count);
        }

	if(msgData.failedBearerContextsCount > 0)
	{
		/*Hard-coding the bearer count as 1, since we support only one bearer Ctxt in the Q msg struct currently.
           	  Count value can be replaced with "msgData.failedBearerContextsCount" while supporting multiple bearers */	
		dbr_info.msg_data.db_req_Q_m.bearerCtxList.bearers_count = 1;
		for(int i=0; i < dbr_info.msg_data.db_req_Q_m.bearerCtxList.bearers_count; i++)
		{
			dbr_info.msg_data.db_req_Q_m.bearerCtxList.bearerCtxt[i].eps_bearer_id =
					msgData.failedBearerContexts[i].epsBearerId.epsBearerId;
			dbr_info.msg_data.db_req_Q_m.bearerCtxList.bearerCtxt[i].cause.data =
					msgData.failedBearerContexts[i].cause.causeValue;
		}
	}


	dbr_info.destInstAddr = htonl(mmeAppInstanceNum_c);
	dbr_info.srcInstAddr = htonl(s11AppInstanceNum_c);

	/*Send CB request msg*/
	send_tipc_message(g_resp_fd, mmeAppInstanceNum_c, (char *)&dbr_info, GTP_READ_MSG_BUF_SIZE);
	log_msg(LOG_INFO, "Send DB req to mme-app.\n");
	
	return SUCCESS;
}
