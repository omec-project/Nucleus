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
detach_stage1_handler(struct proto_IE *detach_ies, bool retransmit)
{
	s1_incoming_msg_data_t req= {0};

	/*****Message structure***
	*/
	log_msg(LOG_INFO, "Parse s1ap detach request message\n");

	/*Validate all eNB info*/

	/*Add eNB info to hash*/

	/*Create Q structure for detach stage 1 to MME.
	  contains init UE information.*/
	/* TODO : Revisit, in InitialContextSetup Request we are sending
	 * MME UE S1AP Id as M-TMSI.
	 */
	req.msg_type = detach_request;
	req.ue_idx = -1;
	req.msg_data.detachReq_Q_msg_m.ue_m_tmsi = -1;
	for(int i = 0; i < detach_ies->no_of_IEs; i++)
    {
        switch(detach_ies->data[i].IE_type)
        {
            case S1AP_IE_MME_UE_ID:
                {
                    if (!retransmit)
                    {
                        req.ue_idx = detach_ies->data[i].val.mme_ue_s1ap_id;
                    }
                }break;
            case S1AP_IE_ENB_UE_ID:
                {
                    req.s1ap_enb_ue_id = detach_ies->data[i].val.enb_ue_s1ap_id;
                }break;
#ifdef S1AP_DECODE_NAS
            case S1AP_IE_NAS_PDU:
                {
		   			req.msg_data.detachReq_Q_msg_m.ue_m_tmsi = detach_ies->data[i].val.nas.elements[0].pduElement.mi_guti.m_TMSI;
                }break;
#endif
            default:
                log_msg(LOG_WARNING,"Unhandled IE %d \n", detach_ies->data[i].IE_type);
        }
    }

	req.destInstAddr = htonl(mmeAppInstanceNum_c);
	req.srcInstAddr = htonl(s1apAppInstanceNum_c);
	send_tipc_message(ipc_S1ap_Hndl, mmeAppInstanceNum_c, (char *)&req, S1_READ_MSG_BUF_SIZE);

	/*Send S1Setup response*/
	log_msg(LOG_INFO, "Send detach request to mme-app \n");

	return SUCCESS;
}


