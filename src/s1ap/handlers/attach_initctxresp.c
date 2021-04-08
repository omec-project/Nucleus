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
	s1_ics_ies.data = NULL;
	struct initctx_resp_Q_msg ics_resp= {0};

	/*****Message structure****/
	log_msg(LOG_INFO, "Parse int ctx s1ap response message:--");
	/*parse_IEs(msg+2, &s1_ics_ies, S1AP_INITIAL_CTX_RESP_CODE);*/
	int decode_status = convertInitCtxRspToProtoIe(msg, &s1_ics_ies);
	if(decode_status < 0) {
		free(s1_ics_ies.data);
		log_msg(LOG_ERROR, "Failed to decode s1ap message");
		return E_FAIL;
	}
    
	
	for(int i = 0; i < s1_ics_ies.no_of_IEs; i++)
    {
        switch(s1_ics_ies.data[i].IE_type)
        {
            case S1AP_IE_MME_UE_ID:
                {
	                ics_resp.header.ue_idx = s1_ics_ies.data[i].val.mme_ue_s1ap_id;
                }break;
            case S1AP_ERAB_SETUP_CTX_SUR:
                {
		    ics_resp.erab_setup_resp_list.count = s1_ics_ies.data[i].val.erab.no_of_elements;
                    for(int j = 0; j < s1_ics_ies.data[i].val.erab.no_of_elements; j++)
                    {
                        /*TBD: Handle multiple erabs in ics rsp*/
			    ics_resp.erab_setup_resp_list.erab_su_res_item[j].e_RAB_ID =
				    s1_ics_ies.data[i].val.erab.elements[j].su_res.eRAB_id;
	                    ics_resp.erab_setup_resp_list.erab_su_res_item[j].transportLayerAddress =
				    s1_ics_ies.data[i].val.erab.elements[j].su_res.transp_layer_addr;
	                    ics_resp.erab_setup_resp_list.erab_su_res_item[j].gtp_teid =
				    s1_ics_ies.data[i].val.erab.elements[j].su_res.gtp_teid;
                    }
                }break;
            default:
                log_msg(LOG_WARNING,"Unhandled IE %d ",s1_ics_ies.data[i].IE_type);
        }
    }
	
	
	ics_resp.header.msg_type = init_ctxt_response;
	ics_resp.header.destInstAddr = htonl(mmeAppInstanceNum_c);
	ics_resp.header.srcInstAddr = htonl(s1apAppInstanceNum_c);
	int i = send_tipc_message(ipc_S1ap_Hndl, mmeAppInstanceNum_c, (char *)&ics_resp, sizeof(struct initctx_resp_Q_msg));

	if (i < 0) {
		log_msg(LOG_ERROR, "Error to write in s1_init_ctx_resp_handler");
	}
	/*Send S1Setup response*/
	log_msg(LOG_INFO, "Init ctx resp send to mme-app stage7. Bytes send %lu", sizeof(struct initctx_resp_Q_msg));

	free(s1_ics_ies.data);
	return SUCCESS;
}
