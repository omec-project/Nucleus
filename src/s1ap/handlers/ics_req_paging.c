/*
 * Copyright (c) 2019, Infosys Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include "common_proc_info.h"
#include "log.h"
#include "err_codes.h"
#include "ipc_api.h"
#include "s1ap.h"
#include "s1ap_config.h"
#include "main.h"
#include "msgType.h"


/*static int
get_uectxtrelcmd_protoie_value(struct proto_IE *value, struct s1relcmd_info *g_uectxtrelcmd)
{
	//uint8_t ieCnt = 0;

	value->no_of_IEs = UE_CTX_RELEASE_NO_OF_IES;

	value->data = (proto_IEs *) malloc(UE_CTX_RELEASE_NO_OF_IES *
			sizeof(proto_IEs));

	value->data[0].mme_ue_s1ap_id = g_uectxtrelcmd->ue_idx;
	
	value->data[1].enb_ue_s1ap_id = g_uectxtrelcmd->enb_s1ap_ue_id;
	
	log_msg(LOG_INFO, "mme_ue_s1ap_id %d and enb_ue_s1ap_id %d\n",
					g_uectxtrelcmd->ue_idx,g_uectxtrelcmd->enb_s1ap_ue_id);
	return SUCCESS;
}*/

/**
* Stage specific message processing.
*/
static int
ics_req_paging_processing(struct ics_req_paging_Q_msg *g_icsreq)
{
	log_msg(LOG_DEBUG,"Process Init Ctxt Req for service Request.");
	uint32_t length = 0;
    	uint8_t *buffer = NULL;
	
	Buffer g_ics_req_buffer = {0};
	struct s1ap_common_req_Q_msg req = {0};
	
        log_msg(LOG_DEBUG,"Inside ics_req_paging processing\n");	
	
    	req.IE_type = S1AP_INIT_CTXT_SETUP_REQ;
    	req.ue_idx = g_icsreq->ue_idx;
	req.enb_fd = g_icsreq->enb_fd;
	req.enb_s1ap_ue_id = g_icsreq->enb_s1ap_ue_id;
    	req.mme_s1ap_ue_id = g_icsreq->ue_idx;
	req.ueag_max_dl_bitrate = g_icsreq->ueag_max_dl_bitrate;
	req.ueag_max_ul_bitrate = g_icsreq->ueag_max_ul_bitrate;
	req.bearer_id = g_icsreq->bearer_id;
	memcpy(&(req.gtp_teid), &(g_icsreq->gtp_teid), sizeof(struct fteid));
	memcpy(&(req.sec_key), &(g_icsreq->sec_key), KENB_SIZE);	
	
	log_msg(LOG_DEBUG,"Before s1ap_encoder\n");

	int ret = s1ap_mme_encode_initiating(&req, &buffer, &length);
	log_msg(LOG_DEBUG,"Invoked s1ap_encoder\n");
    	if(ret == -1)
    	{
        	log_msg(LOG_ERROR, "Encoding ics_req_paging failed.\n");
        	return E_FAIL;
    	}

	buffer_copy(&g_ics_req_buffer, buffer, length);
	send_sctp_msg(g_icsreq->enb_fd, g_ics_req_buffer.buf, g_ics_req_buffer.pos,1);
	log_msg(LOG_INFO, "\n----ICS Req for paging sent to UE.---\n");
	return SUCCESS;
	
}


/**
* Thread function for stage.
*/
void*
ics_req_paging_handler(void *data)
{

	log_msg(LOG_INFO,"ICS Req for paging handler ready.\n");
	
	ics_req_paging_processing((struct ics_req_paging_Q_msg *)data);

	return NULL;
}


