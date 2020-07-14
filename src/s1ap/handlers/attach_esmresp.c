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
s1_esm_resp_handler(struct proto_IE *s1_esm_resp_ies)
{
	s1_incoming_msg_data_t esm_resp= {0};
	esm_resp.msg_type = esm_info_response;

	/*****Message structure***
	*/
	log_msg(LOG_INFO, "Parse s1ap ESM response message:--\n");

	/*Validate all eNB info*/

	/*Add eNB info to hash*/

	/*Create Q structure for stage 1 to MME.
	  contains init UE information.*/
    for(int i = 0; i < s1_esm_resp_ies->no_of_IEs; i++)
    {
        switch(s1_esm_resp_ies->data[i].IE_type)
        {
            case S1AP_IE_MME_UE_ID:
                {
	                esm_resp.ue_idx = s1_esm_resp_ies->data[i].val.mme_ue_s1ap_id;
                }break;
#ifdef S1AP_DECODE_NAS
            case S1AP_IE_NAS_PDU:
                {
                    if(s1_esm_resp_ies->data[i].val.nas.header.message_type != NAS_ESM_RESP)
                    {
                        esm_resp.msg_data.esm_resp_Q_msg_m.status = S1AP_SECMODE_FAILED;//Error in authentication
                    }
                    else
                    {
                        esm_resp.msg_data.esm_resp_Q_msg_m.status = SUCCESS;
	                    memcpy(&(esm_resp.msg_data.esm_resp_Q_msg_m.apn), &(s1_esm_resp_ies->data[i].val.nas.elements[0].pduElement.apn),
		                       sizeof(struct apn_name));
                    }
                }break;
#endif
            default:
                log_msg(LOG_WARNING,"Unhandled IE %d \n",s1_esm_resp_ies->data[i].IE_type);
        }
    }


        esm_resp.destInstAddr = htonl(mmeAppInstanceNum_c);
        esm_resp.srcInstAddr = htonl(s1apAppInstanceNum_c);

        //STIMER_GET_CURRENT_TP(g_attach_stats[s1_auth_resp_ies->data[1].enb_ue_s1ap_id].auth_to_mme);
        send_tipc_message(ipc_S1ap_Hndl, mmeAppInstanceNum_c, (char *)&esm_resp, S1_READ_MSG_BUF_SIZE);

        /*Send S1Setup response*/
        log_msg(LOG_INFO, "ESM Info resp send to mme-app\n");

	//TODO: free IEs
	return SUCCESS;
}

