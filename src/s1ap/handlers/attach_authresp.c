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
extern struct time_stat g_attach_stats[];

int
s1_auth_resp_handler(struct proto_IE *s1_auth_resp_ies)
{
	//TODO: use static instead of synamic for perf.
	s1_incoming_msg_data_t auth_resp= {0};

	/*****Message structure***
	*/
	log_msg(LOG_INFO, "Parse s1ap auth resp message:--\n");

	/*Validate all eNB info*/

	/*Add eNB info to hash*/

	/*Create Q structure for stage 1 to MME.
	  contains init UE information.*/
	auth_resp.msg_type = auth_response;	
		
	for(int i = 0; i < s1_auth_resp_ies->no_of_IEs; i++)
    {
        switch(s1_auth_resp_ies->data[i].IE_type)
        {
            case S1AP_IE_MME_UE_ID:
                {
	                auth_resp.ue_idx = s1_auth_resp_ies->data[i].val.mme_ue_s1ap_id;
                }break;
#ifdef S1AP_DECODE_NAS
            case S1AP_IE_NAS_PDU:
                {
                    if(s1_auth_resp_ies->data[i].val.nas.header.message_type != NAS_AUTH_RESP)
                    {
                        auth_resp.msg_data.authresp_Q_msg_m.status = S1AP_AUTH_FAILED;//Error in authentication
                    }
                    else
                    {
                        auth_resp.msg_data.authresp_Q_msg_m.status = SUCCESS;
                    }

                    memcpy(&(auth_resp.msg_data.authresp_Q_msg_m.res), 
                           &(s1_auth_resp_ies->data[i].val.nas.elements[0].pduElement.auth_resp),
                           sizeof(struct XRES));

                }break;
#endif
            default:
                log_msg(LOG_WARNING,"Unhandled IE %d \n",s1_auth_resp_ies->data[i].IE_type);
        }
    }

	auth_resp.destInstAddr = htonl(mmeAppInstanceNum_c);
	auth_resp.srcInstAddr = htonl(s1apAppInstanceNum_c);

	//STIMER_GET_CURRENT_TP(g_attach_stats[s1_auth_resp_ies->data[1].enb_ue_s1ap_id].auth_to_mme);
	send_tipc_message(ipc_S1ap_Hndl, mmeAppInstanceNum_c, (char *)&auth_resp, S1_READ_MSG_BUF_SIZE);

	/*Send S1Setup response*/
	log_msg(LOG_INFO, "Auth resp send to mme-app stage3.\n");

	//TODO: free IEs
	return SUCCESS;
}

int
s1_auth_fail_handler(struct proto_IE *s1_auth_resp_ies)
{
	//TODO: use static instead of synamic for perf.
	s1_incoming_msg_data_t auth_resp={0};

	/*****Message structure***
	*/
	log_msg(LOG_INFO, "Parse s1ap auth fail resp:--\n");

	/*Validate all eNB info*/

	/*Add eNB info to hash*/

	/*Create Q structure for stage 1 to MME.
	  contains init UE information.*/
	  
	/* msg_type for auth_failure
	?auth_resp.msg_type =?;*/
	auth_resp.msg_type = auth_response;	
	  
    for(int i = 0; i < s1_auth_resp_ies->no_of_IEs; i++)
    {
        switch(s1_auth_resp_ies->data[i].IE_type)
        {
            case S1AP_IE_MME_UE_ID:
                {
	                auth_resp.ue_idx = s1_auth_resp_ies->data[i].val.mme_ue_s1ap_id;
                }break;
#ifdef S1AP_DECODE_NAS
            case S1AP_IE_NAS_PDU:
                {
                    auth_resp.msg_data.authresp_Q_msg_m.status = S1AP_AUTH_FAILED;//Error in authentication
	                memcpy(&(auth_resp.msg_data.authresp_Q_msg_m.auts), 
                           &(s1_auth_resp_ies->data[i].val.nas.elements[0].pduElement.auth_fail_resp),
		                   sizeof(struct AUTS));
                }break;
#endif
            default:
                log_msg(LOG_WARNING,"Unhandled IE %d \n",s1_auth_resp_ies->data[i].IE_type);
        }
    }
	
    auth_resp.destInstAddr = htonl(mmeAppInstanceNum_c);
	auth_resp.srcInstAddr = htonl(s1apAppInstanceNum_c);

	//STIMER_GET_CURRENT_TP(g_attach_stats[s1_auth_resp_ies->data[1].enb_ue_s1ap_id].auth_to_mme);
	send_tipc_message(ipc_S1ap_Hndl, mmeAppInstanceNum_c, (char *)&auth_resp, S1_READ_MSG_BUF_SIZE);

	/*Send S1Setup response*/
	log_msg(LOG_INFO, "Sent Auth fail resp indication to mme-app stage3.\n");

	//TODO: free IEs
	return SUCCESS;
}

