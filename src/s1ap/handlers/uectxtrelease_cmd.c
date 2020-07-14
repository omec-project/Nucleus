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




/**
* Stage specific message processing.
*/
static int
relcmd_processing(struct s1relcmd_info *g_uectxtrelcmd)
{
	log_msg(LOG_DEBUG,"Process Ctx rel cmd.");
	uint32_t length = 0;
    	uint8_t *buffer = NULL;
	
	Buffer g_ctxrel_buffer = {0};
	struct s1ap_common_req_Q_msg req = {0};
	
        log_msg(LOG_DEBUG,"Inside relcmd processing\n");	

    	req.IE_type = S1AP_CTX_REL_CMD;
    	req.enb_fd = g_uectxtrelcmd->enb_fd;
    	req.mme_s1ap_ue_id = g_uectxtrelcmd->ue_idx;
	req.enb_s1ap_ue_id = g_uectxtrelcmd->enb_s1ap_ue_id;
	req.cause.present = g_uectxtrelcmd->cause.present;
    	req.cause.choice.radioNetwork = g_uectxtrelcmd->cause.choice.radioNetwork; 	
	
	log_msg(LOG_DEBUG,"Before s1ap_encoder\n");

	int ret = s1ap_mme_encode_initiating(&req, &buffer, &length);
	log_msg(LOG_DEBUG,"Invoked s1ap_encoder\n");
    	if(ret == -1)
    	{
        	log_msg(LOG_ERROR, "Encoding ctx rel cmd failed.\n");
        	return E_FAIL;
    	}


	buffer_copy(&g_ctxrel_buffer, buffer, length);
	send_sctp_msg(g_uectxtrelcmd->enb_fd, g_ctxrel_buffer.buf, g_ctxrel_buffer.pos,1);
	log_msg(LOG_INFO, "\n-----S1 Release Command sent to UE. len %d ret %d---\n", length, ret);
	return SUCCESS;
	
}

/**
* Thread function for stage.
*/
void*
s1_release_command_handler(void *data)
{

	log_msg(LOG_INFO, "s1 release command handler ready.\n");
	
	relcmd_processing((struct s1relcmd_info *)data);

	return NULL;
}


