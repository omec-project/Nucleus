/*
 * Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
 * Copyright (c) 2017 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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

extern s1ap_config g_s1ap_cfg;
extern ipc_handle ipc_S1ap_Hndl;

int
s1_init_ue_service_req_handler(struct proto_IE *service_req_ies, int enb_fd)
{
	struct  s1_incoming_msg_data_t req= {0};

	/*****Message structure***
	*/
	log_msg(LOG_INFO, "Parse s1ap Service request message\n");

	/*Validate all eNB info*/

	/*Add eNB info to hash*/

	/*Create Q structure for Service req to MME.
	  contains init UE information.*/
	/* TODO : Revisit, in InitialContextSetup Request we are sending
	 * MME UE S1AP Id as M-TMSI.
	 */
    req.msg_type = service_request;
    req.msg_data.service_req_Q_msg_m.enb_fd = enb_fd;
    for(int i = 0; i < service_req_ies->no_of_IEs; i++)
    {
        switch(service_req_ies->data[i].IE_type)
        {
            case S1AP_IE_ENB_UE_ID:
                {
                    log_msg(LOG_INFO, "Service Req S1AP_IE_ENB_UE_ID.\n");
                    req.s1ap_enb_ue_id = service_req_ies->data[i].val.enb_ue_s1ap_id;
                }break;
            case S1AP_IE_NAS_PDU:
                {
                    log_msg(LOG_INFO, "Service Req NAS PDU.\n");
                    req.msg_data.service_req_Q_msg_m.ksi = service_req_ies->data[i].val.nas.header.ksi;;
                    req.msg_data.service_req_Q_msg_m.seq_no = service_req_ies->data[i].val.nas.header.seq_no;
                    memcpy(&req.msg_data.service_req_Q_msg_m.mac, service_req_ies->data[i].val.nas.header.short_mac, sizeof(uint16_t));
                }break;
            case S1AP_IE_TAI:
                {
                    log_msg(LOG_INFO, "Service Req TAI.\n");
                    memcpy(&req.msg_data.service_req_Q_msg_m.tai, 
                        &service_req_ies->data[i].val.tai, 
                        sizeof(struct TAI));
                }break;
            case S1AP_IE_UTRAN_CGI:
                {
                    log_msg(LOG_INFO, "Service Req CGI.\n");
                    memcpy(&req.msg_data.service_req_Q_msg_m.utran_cgi, 
                        &service_req_ies->data[i].val.utran_cgi, 
                        sizeof(struct CGI));
                }break;
            case S1AP_IE_S_TMSI:
                {
                    log_msg(LOG_INFO, "Service Req STMSI.\n");
                    if(service_req_ies->data[i].val.s_tmsi.mme_code 
                       == g_s1ap_cfg.mme_code)
                    {
                        log_msg(LOG_INFO, "Service Req MME Code matched.\n");
                        req.ue_idx = ntohl(service_req_ies->data[i].val.s_tmsi.m_TMSI);
                        memcpy(&req.msg_data.service_req_Q_msg_m.s_tmsi, 
                        &service_req_ies->data[i].val.s_tmsi,
                         sizeof(struct STMSI));
                    }
                    else
                    {
                        log_msg(LOG_ERROR, "MME code mismatch. Send Service Reject. TBD");
                        return -E_FAIL;
                    }
                }break;
            default:
                log_msg(LOG_WARNING,"Unhandled IE %d \n", service_req_ies->data[i].IE_type);
        }
    }

	req.destInstAddr = htonl(mmeAppInstanceNum_c);
        req.srcInstAddr = htonl(s1apAppInstanceNum_c);
        send_tipc_message(ipc_S1ap_Hndl, mmeAppInstanceNum_c, (char *)&req, S1_READ_MSG_BUF_SIZE);

	
	/*Send Service req to mme-app*/
	log_msg(LOG_INFO, "Send to mme-app service req handler.\n");

	return SUCCESS;
}


