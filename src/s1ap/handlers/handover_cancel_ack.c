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

static int handover_cancel_ack_processing(
        struct handover_cancel_ack_Q_msg *g_ho_cancel_ack)
{
    log_msg(LOG_DEBUG, "Process Handover Cancel Acknowledge.");

    uint32_t length = 0;
    uint8_t *buffer = NULL;

    int ret = s1ap_mme_encode_handover_cancel_ack(g_ho_cancel_ack, &buffer,
            &length);
    if (ret == -1)
    {
        log_msg(LOG_ERROR, "Encoding Handover Cancel Acknowledge failed.");
        return E_FAIL;
    }

    send_sctp_msg(g_ho_cancel_ack->src_enb_context_id, buffer, length, 1);

    log_msg(LOG_DEBUG,
            "HO Handover Cancel Acknowledge Sent. Num of bytes - %d", length);
	if(buffer != NULL) {
		free(buffer);
	}

    return SUCCESS;
}

void*
handover_cancel_ack_handler(void *data)
{
    handover_cancel_ack_processing((struct handover_cancel_ack_Q_msg*) data);
    return NULL;
}
