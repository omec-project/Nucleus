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
s1_secmode_resp_handler(struct proto_IE *s1_sec_resp_ies)
{
	//TODO: use static instead of synamic for perf.
	struct s1_incoming_msg_data_t secmode_resp= {0};

	/*****Message structure***
	*/
	log_msg(LOG_INFO, "Parse s1ap sec mode complete message:--\n");

	/*Validate all eNB info*/

	/*Add eNB info to hash*/

	/*Create Q structure for stage 1 to MME.
	  contains init UE information.*/
	secmode_resp.msg_type = sec_mode_complete;

	for(int i = 0; i < s1_sec_resp_ies->no_of_IEs; i++)
    {
        switch(s1_sec_resp_ies->data[i].IE_type)
        {
            case S1AP_IE_MME_UE_ID:
                {
	                secmode_resp.ue_idx = s1_sec_resp_ies->data[i].val.mme_ue_s1ap_id;
                }break;
            case S1AP_IE_NAS_PDU:
                {
                    if(s1_sec_resp_ies->data[i].val.nas.header.message_type != NAS_SEC_MODE_COMPLETE)
                    {
                        secmode_resp.msg_data.secmode_resp_Q_msg_m.status = S1AP_SECMODE_FAILED;//Error in authentication
                    }
                    else
                    {
                        secmode_resp.msg_data.secmode_resp_Q_msg_m.status = SUCCESS;
                    }

                }break;
            default:
                log_msg(LOG_WARNING,"Unhandled IE %d \n",s1_sec_resp_ies->data[i].IE_type);
        }
    }

	/*Copy xres from response, send to mme for verification*/
	//...
	//auth_res.res.len = a1_secmode_resp_ies[2].data.nas.authresp_len;
	//memcpy(&(auth_res.res.val, s1_sec_resp_ies[2].data.nas.RES, authres.res.len);

	secmode_resp.destInstAddr = htonl(mmeAppInstanceNum_c);
	secmode_resp.srcInstAddr = htonl(s1apAppInstanceNum_c);
	send_tipc_message(ipc_S1ap_Hndl, mmeAppInstanceNum_c, (char *)&secmode_resp, S1_READ_MSG_BUF_SIZE);

	/*Send S1Setup response*/
	log_msg(LOG_INFO, "Auth resp send to mme-app stage4.\n");

	//TODO: free IEs
	return SUCCESS;
}

