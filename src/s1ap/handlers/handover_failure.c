/*
 * Copyright 2020-present, Infosys Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "ipc_api.h"
#include "s1ap.h"
#include "sctp_conn.h"
#include "s1ap_msg_codes.h"
#include "msgType.h"

extern ipc_handle ipc_S1ap_Hndl;

int s1_handover_faliure_handler(UnsuccessfulOutcome_t *msg)
{
    handover_failure_Q_msg_t ho_failure = {0};
    struct proto_IE ho_failure_ies = {0};
    log_msg(LOG_INFO, "Parse s1ap handover failure message");

    int decode_status = convertHoFailureToProtoIe(msg, &ho_failure_ies);
    if (decode_status < 0)
    {
        log_msg(LOG_ERROR, "Failed to decode HO Failure");

        if (ho_failure_ies.data != NULL)
            free(ho_failure_ies.data);

        return E_FAIL;
    }
    for (int i = 0; i < ho_failure_ies.no_of_IEs; i++)
    {
        switch (ho_failure_ies.data[i].IE_type)
        {
        case S1AP_IE_MME_UE_ID:
        {
            log_msg(LOG_INFO, "handover failure S1AP_IE_MME_UE_ID.");

            ho_failure.header.ue_idx = ho_failure_ies.data[i].val.mme_ue_s1ap_id;
        }
            break;
        case S1AP_IE_CAUSE:
        {
            log_msg(LOG_INFO, "handover failure S1AP_IE_CAUSE.");

            memcpy(&ho_failure.cause,
                    &ho_failure_ies.data[i].val.cause,
                    sizeof(struct s1apCause));
        }
            break;
        default:
            log_msg(LOG_WARNING, "Unhandled IE");
        }
    }

    ho_failure.header.msg_type = handover_failure;
    ho_failure.header.destInstAddr = htonl(mmeAppInstanceNum_c);
    ho_failure.header.srcInstAddr = htonl(s1apAppInstanceNum_c);

    int i = send_tipc_message(ipc_S1ap_Hndl, mmeAppInstanceNum_c,
            (char*) &ho_failure,
            sizeof(ho_failure));

    if (i < 0)
    {
        log_msg(LOG_ERROR, "Error To write in s1_handover_faliure_handler");
    }
    if (ho_failure_ies.data != NULL)
        free(ho_failure_ies.data);
    return SUCCESS;
}

