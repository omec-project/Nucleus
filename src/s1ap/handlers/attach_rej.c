/*
 * Copyright (c) 2019, Infosys Ltd.
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
attachrej_processing(struct s1ap_common_req_Q_msg *g_ueattachrej)
{
	log_msg(LOG_DEBUG,"Process Attach Reject.");
	uint32_t length = 0;
    	uint8_t *buffer = NULL;
	
	Buffer g_attach_rej_buffer;
	
        log_msg(LOG_DEBUG,"Inside attach reject processing\n");	

	
	log_msg(LOG_DEBUG,"Before s1ap_encoder\n");

	int ret = s1ap_mme_encode_initiating(g_ueattachrej, &buffer, &length);
	log_msg(LOG_DEBUG,"Invoked s1ap_encoder\n");
    	if(ret == -1)
    	{
        	log_msg(LOG_ERROR, "Encoding Attach Rej failed.\n");
        	return E_FAIL;
    	}

	buffer_copy(&g_attach_rej_buffer, buffer, length);
	send_sctp_msg(g_ueattachrej->enb_fd, g_attach_rej_buffer.buf, 
                  g_attach_rej_buffer.pos,1);
	log_msg(LOG_INFO, "\n-----Attach reject sent to UE.---\n");
	return SUCCESS;
	
}


/**
* Thread function for stage.
*/
void*
attach_rej_command_handler(void *data)
{

	log_msg(LOG_INFO, "attach Reject command handler ready.\n");
	
	attachrej_processing((struct s1ap_common_req_Q_msg *)data);

	return NULL;
}


