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
erab_mod_confirm_processing(struct erab_mod_confirm *g_erab_mod_conf)
{
	log_msg(LOG_DEBUG,"E-RAB Modification Confirmation.\n");

	uint32_t length = 0;
	uint8_t *buffer = NULL;
	
	int ret = s1ap_mme_encode_erab_mod_confirmation(g_erab_mod_conf, &buffer, &length);
	if(ret == -1)
	{
		log_msg(LOG_ERROR, "Encoding E-RAB Modification Confirmation failed.\n");
		return E_FAIL;
	}

	length = send_sctp_msg(
	        g_erab_mod_conf->enb_context_id, buffer, length, 1);

	log_msg(LOG_DEBUG, "E-RAB Modification Confirmation sent. No. of bytes %d\n", length);

	return SUCCESS;
}

void*
erab_mod_confirm_handler(void *data)
{
	erab_mod_confirm_processing((struct erab_mod_confirm *)data);
	return NULL;
}

