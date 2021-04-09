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
	
	struct s1ap_common_req_Q_msg req = {0};

	req.IE_type = S1AP_PAGING_REQ;
	req.enb_fd = g_paging->enb_fd;
	req.m_tmsi = g_paging->ue_idx;
	req.enb_s1ap_ue_id = g_paging->enb_s1ap_ue_id;
	req.cn_domain = g_paging->cn_domain;
	memcpy(req.imsi, g_paging->IMSI, BINARY_IMSI_LEN);
	memcpy(&req.tai, &g_paging->tai, sizeof(struct TAI));

	int ret = s1ap_mme_encode_initiating(&req, &buffer, &length);

	if(ret == -1)
	{
		log_msg(LOG_ERROR, "Encoding paging request failed.");
		return E_FAIL;
	}

	send_sctp_msg(g_paging->enb_fd, buffer, length , 1);
	if(buffer != NULL) {
		free(buffer);
	}
	log_msg(LOG_INFO, "-----Paging sent to UE.---");
	return SUCCESS;
}


/**
* Thread function for stage.
*/
void*
paging_handler(void *data)
{

	log_msg(LOG_INFO, "paging handler ready.");
	
	paging_processing((struct paging_req_Q_msg*)data);

	return NULL;
}


