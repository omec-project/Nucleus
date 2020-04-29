/*
* Copyright 2020-present, Infosys Ltd.
*
* SPDX-License-Identifier: Apache-2.0
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#include "log.h"
#include "s1ap.h"
#include "msgType.h"

static int
handover_request_processing(struct handover_request_Q_msg *g_ho_req)
{
	log_msg(LOG_DEBUG,"Process Handover Request.\n");

	uint32_t length = 0;
    uint8_t *buffer = NULL;

	int ret = s1ap_mme_encode_handover_request(g_ho_req, &buffer, &length);
    if (ret == -1)
    {
        log_msg(LOG_ERROR, "Encoding handover request failed.\n");
        return E_FAIL;
    }

	length = send_sctp_msg(g_ho_req->target_enb_context_id, buffer, length, 1);

	log_msg(LOG_INFO,
	        "HO Request Sent. Num of bytes - %d, enb_fd - %d\n",
	        length, g_ho_req->target_enb_context_id);
	
	return SUCCESS;
}

void*
handover_request_handler(void *data)
{
	handover_request_processing((struct handover_request_Q_msg*) data);
	return NULL;
}
