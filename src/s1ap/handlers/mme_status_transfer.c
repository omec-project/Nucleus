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
mme_status_transfer_processing(struct mme_status_transfer_Q_msg *g_mme_status)
{
    log_msg(LOG_DEBUG, "Process MME Status Transfer.\n");

    uint32_t length = 0;
    uint8_t *buffer = NULL;

    int ret = s1ap_mme_encode_handover_mme_status_transfer(g_mme_status,
            &buffer, &length);

    if (ret == -1)
    {
        log_msg(LOG_ERROR, "Encoding mme status transfer failed.\n");
        return E_FAIL;
    }

    length = send_sctp_msg(
            g_mme_status->target_enb_context_id, buffer, length, 1);

    log_msg(LOG_DEBUG, "MME-Status-Transfer sent. No. of bytes %d\n", length);
    return SUCCESS;

}

void*
mme_status_transfer_handler(void *data)
{
	mme_status_transfer_processing((struct mme_status_transfer_Q_msg *)data);

	return NULL;
}
