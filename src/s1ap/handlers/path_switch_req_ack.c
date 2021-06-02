/*
* Copyright 2021-present, Infosys Ltd.
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

static int path_switch_req_ack_processing(
        struct path_switch_req_ack_Q_msg *g_path_sw_req_ack)
{
    log_msg(LOG_DEBUG, "Process Path Switch Request Acknowledge.");

    uint32_t length = 0;
    uint8_t *buffer = NULL;

    int ret = s1ap_mme_encode_path_switch_req_ack(g_path_sw_req_ack, &buffer,
            &length);
    if (ret == -1)
    {
        log_msg(LOG_ERROR, "Encoding Path Switch Request Acknowledge failed.");
        return E_FAIL;
    }

    send_sctp_msg(g_path_sw_req_ack->enb_context_id, buffer, length, 1);

    log_msg(LOG_DEBUG,
            "Path Switch Request Acknowledge Sent. Num of bytes - %d", length);
    if(buffer != NULL) {
        free(buffer);
    }

    return SUCCESS;
}

void*
path_switch_req_ack_handler(void *data)
{
    path_switch_req_ack_processing((struct path_switch_req_ack_Q_msg*) data);
    return NULL;
}
