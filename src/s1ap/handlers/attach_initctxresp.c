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
s1_init_ctx_resp_handler(SuccessfulOutcome_t *msg)
{
	struct proto_IE s1_ics_ies;
	struct s1_incoming_msg_data_t ics_resp= {0};

	/*****Message structure****/
	log_msg(LOG_INFO, "Parse int ctx s1ap response message:--\n");
	/*parse_IEs(msg+2, &s1_ics_ies, S1AP_INITIAL_CTX_RESP_CODE);*/
	convertInitCtxRspToProtoIe(msg, &s1_ics_ies);
    
	ics_resp.msg_type = init_ctxt_response;
	
	for(int i = 0; i < s1_ics_ies.no_of_IEs; i++)
    {
        switch(s1_ics_ies.data[i].IE_type)
        {
            case S1AP_IE_MME_UE_ID:
                {
	                ics_resp.ue_idx = s1_ics_ies.data[i].val.mme_ue_s1ap_id;
                }break;
            case S1AP_ERAB_SETUP_CTX_SUR:
                {
                    for(int j = 0; j < s1_ics_ies.data[i].val.erab.no_of_elements; j++)
                    {
                        /*TBD: Handle multiple erabs in ics rsp*/
	                    ics_resp.msg_data.initctx_resp_Q_msg_m.transp_layer_addr = s1_ics_ies.data[i].val.erab.elements[j].su_res.transp_layer_addr;
	                    ics_resp.msg_data.initctx_resp_Q_msg_m.gtp_teid = s1_ics_ies.data[i].val.erab.elements[j].su_res.gtp_teid;
                        break;
                    }
                }break;
            default:
                log_msg(LOG_WARNING,"Unhandled IE %d \n",s1_ics_ies.data[i].IE_type);
        }
    }
	
	
	
	ics_resp.destInstAddr = htonl(mmeAppInstanceNum_c);
	ics_resp.srcInstAddr = htonl(s1apAppInstanceNum_c);
	int i = send_tipc_message(ipc_S1ap_Hndl, mmeAppInstanceNum_c, (char *)&ics_resp, S1_READ_MSG_BUF_SIZE);

	if (i < 0) {
		log_msg(LOG_ERROR, "Error to write in s1_init_ctx_resp_handler\n");
	}
	/*Send S1Setup response*/
	log_msg(LOG_INFO, "Init ctx resp send to mme-app stage7. Bytes send %d\n", i);

	//TODO: free IEs
	return SUCCESS;
}
