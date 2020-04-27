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

#include "ProtocolIE-Container.h"
#include "ProtocolIE-ID.h"
#include "ProtocolIE-Field.h"
#include "log.h"
#include "err_codes.h"
#include "ipc_api.h"
#include "s1ap.h"
#include "s1ap_config.h"
#include "main.h"
#include "msgType.h"
#include "common_proc_info.h"


/*Stage specific message processing.
*/
static int
paging_processing(struct paging_req_Q_msg *g_paging)
{
	log_msg(LOG_DEBUG,"Process paging.");
	uint32_t length = 0;
    	uint8_t *buffer = NULL;
	
	Buffer g_paging_buffer = {0};
	struct s1ap_common_req_Q_msg req = {0};

	req.IE_type = S1AP_PAGING_REQ;
        req.enb_fd = g_paging->enb_fd;
        req.ue_idx = g_paging->ue_idx;
        req.enb_s1ap_ue_id = g_paging->enb_s1ap_ue_id;
	req.cn_domain = g_paging->cn_domain;
	memcpy(req.imsi, g_paging->IMSI, BINARY_IMSI_LEN);
	memcpy(&req.tai, &g_paging->tai, sizeof(struct TAI));

	

	int ret = s1ap_mme_encode_initiating(&req, &buffer, &length);

    	if(ret == -1)
    	{
	        log_msg(LOG_ERROR, "Encoding paging request failed.\n");
        	return E_FAIL;
   	}


	buffer_copy(&g_paging_buffer, buffer, length);
	send_sctp_msg(g_paging->enb_fd, g_paging_buffer.buf, g_paging_buffer.pos,1);
	log_msg(LOG_INFO, "\n-----Paging sent to UE.---\n");
	return SUCCESS;
}


/**
* Thread function for stage.
*/
void*
paging_handler(void *data)
{

	log_msg(LOG_INFO, "paging handler ready.\n");
	
	paging_processing((struct paging_req_Q_msg*)data);

	return NULL;
}


