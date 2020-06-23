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

static int handover_preparation_failure_processing(
        struct handover_preparation_failure_Q_msg *g_ho_prep_fail)
{
    log_msg(LOG_DEBUG, "Process Handover Preparation Failure.");

    uint32_t length = 0;
    uint8_t *buffer = NULL;

    int ret = s1ap_mme_encode_handover_prep_failure(g_ho_prep_fail, &buffer,
            &length);
    if (ret == -1)
    {
        log_msg(LOG_ERROR, "Encoding Handover Preparation Failure failed.\n");
        return E_FAIL;
    }

    length = send_sctp_msg(g_ho_prep_fail->src_enb_context_id, buffer, length, 1);

    log_msg(LOG_INFO,
            "HO Handover Preparation Failure Sent. Num of bytes - %d\n",
            length);

    return SUCCESS;
}

void*
handover_preparation_failure_handler(void *data)
{
    handover_preparation_failure_processing(
            (struct handover_preparation_failure_Q_msg*) data);
    return NULL;
}
