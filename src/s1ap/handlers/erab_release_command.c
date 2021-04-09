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

static int erab_release_command_processing(
        struct erab_release_command_Q_msg *g_erab_rel_cmd)
{
    log_msg(LOG_DEBUG, "E-RAB release command Processing.");

    uint32_t length = 0;
    uint8_t *buffer = NULL;

    int ret = s1ap_mme_encode_erab_release_command(g_erab_rel_cmd, &buffer,
                                                   &length);
    if(ret == -1)
    {
        log_msg(LOG_ERROR, "Encoding E-RAB release command  failed.");
        return E_FAIL;
    }

    send_sctp_msg(g_erab_rel_cmd->enb_context_id, buffer, length, 1);

    log_msg(LOG_DEBUG,
            "E-RAB release command sent. No. of bytes %d on enb_context_id %d",
            length, g_erab_rel_cmd->enb_context_id);
    if(buffer != NULL) {
        free(buffer);
    }

    return SUCCESS;
}

void*
erab_release_command_handler(void *data)
{
    erab_release_command_processing((struct erab_release_command_Q_msg*) data);
    return NULL;
}

