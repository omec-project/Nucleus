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
handover_command_processing(struct handover_command_Q_msg *g_ho_cmd)
{
	log_msg(LOG_DEBUG,"Process Handover Command.\n");

	uint32_t length = 0;
	uint8_t *buffer = NULL;
	
	int ret = s1ap_mme_encode_handover_command(g_ho_cmd, &buffer, &length);
	if(ret == -1)
	{
		log_msg(LOG_ERROR, "Encoding Handover Command failed.\n");
		return E_FAIL;
	}

	length = send_sctp_msg(
	        g_ho_cmd->src_enb_context_id, buffer, length, 1);

	log_msg(LOG_DEBUG, "HO Command sent. No. of bytes %d\n", length);

	return SUCCESS;
}

void*
handover_command_handler(void *data)
{
	handover_command_processing((struct handover_command_Q_msg *)data);
	return NULL;
}
