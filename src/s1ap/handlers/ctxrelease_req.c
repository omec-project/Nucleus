/*
 * Copyright 2019-present Open Networking Foundation
 * Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "err_codes.h"
#include "options.h"
#include "ipc_api.h"
#include "main.h"
#include "s1ap.h"
#include "s1ap_config.h"
#include "sctp_conn.h"
#include "s1ap_structs.h"
#include "s1ap_msg_codes.h"
#include "msgType.h"


extern ipc_handle ipc_S1ap_Hndl;

int
s1_ctx_release_request_handler(InitiatingMessage_t *msg)
{
	ue_context_rel_req_t release_request= {0};
	struct proto_IE s1_ctx_rel_req_ies;
	s1_ctx_rel_req_ies.data = NULL;

	log_msg(LOG_INFO, "Parse s1ap context release request message:--");

	int decode_status = convertUeCtxRelReqToProtoIe(msg, &s1_ctx_rel_req_ies);
	if(decode_status < 0) {
		free(s1_ctx_rel_req_ies.data);
		log_msg(LOG_ERROR, "Failed to decode s1ap message");
		return E_FAIL;
	}

	/*TODO: Validate all eNB info*/
    for(int i = 0; i < s1_ctx_rel_req_ies.no_of_IEs; i++)
    {
        switch(s1_ctx_rel_req_ies.data[i].IE_type)
        {
            case S1AP_IE_MME_UE_ID:
                {
	                release_request.header.ue_idx = s1_ctx_rel_req_ies.data[i].val.mme_ue_s1ap_id;
                }break;
            case S1AP_IE_ENB_UE_ID:
                {
	                release_request.header.s1ap_enb_ue_id 
                        = s1_ctx_rel_req_ies.data[i].val.enb_ue_s1ap_id;
                }break;
            default:
                log_msg(LOG_WARNING,"Unhandled IE %d",s1_ctx_rel_req_ies.data[i].IE_type);
        }
    }

	
    release_request.header.msg_type = s1_release_request;
	release_request.header.destInstAddr = htonl(mmeAppInstanceNum_c);
	release_request.header.srcInstAddr = htonl(s1apAppInstanceNum_c);
	int i = 0;
	i = send_tipc_message(ipc_S1ap_Hndl,mmeAppInstanceNum_c,
				(char *)&release_request,
				sizeof(release_request));
				
	if (i < 0) {
		log_msg(LOG_ERROR,"Error To write in s1_ctx_release_request_handler");
	}

	log_msg(LOG_INFO, "Ctx Release request sent to mme-app."
				"Bytes send %d", i);

	free(s1_ctx_rel_req_ies.data);
	return SUCCESS;
}
