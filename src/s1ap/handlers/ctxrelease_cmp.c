/*
 * Copyright (c) 2019, Infosys Ltd.
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
s1_ctx_release_complete_handler(SuccessfulOutcome_t *msg)
{
	s1_incoming_msg_data_t release_complete= {0};
	struct proto_IE s1_ctx_release_ies;

	log_msg(LOG_INFO, "Parse s1ap context release complete message:--\n");

	 convertUeCtxRelComplToProtoIe(msg, &s1_ctx_release_ies);

	/*TODO: Validate all eNB info*/
     for(int i = 0; i < s1_ctx_release_ies.no_of_IEs; i++)
    {
        switch(s1_ctx_release_ies.data[i].IE_type)
        {
            case S1AP_IE_MME_UE_ID:
                {
	                release_complete.ue_idx = s1_ctx_release_ies.data[i].val.mme_ue_s1ap_id;
                }break;
            default:
                log_msg(LOG_WARNING,"Unhandled IE %d \n",s1_ctx_release_ies.data[i].IE_type);
        }
    }

	release_complete.msg_type = s1_release_complete;
	release_complete.destInstAddr = htonl(mmeAppInstanceNum_c);
	release_complete.srcInstAddr = htonl(s1apAppInstanceNum_c);
	int i = 0;
	i = send_tipc_message(ipc_S1ap_Hndl,mmeAppInstanceNum_c,
				(char *)&release_complete,
				S1_READ_MSG_BUF_SIZE);
	if (i < 0) {

		log_msg(LOG_ERROR,"Error To write in s1_ctx_release_code_handler\n");
	}

	log_msg(LOG_INFO, "Ctx Release complete sent to mme-app."
				"Bytes sent %d\n", i);

	//TODO: free IEs
	return SUCCESS;
}

