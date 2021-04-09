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

int s1_handover_cancel_handler(InitiatingMessage_t *msg)
{
    handover_cancel_Q_msg_t ho_cancel = {0};
    struct proto_IE ho_cancel_ies = {0};
    log_msg(LOG_INFO, "Parse s1ap handover cancel message");

    int decode_status = convertUeHoCancelToProtoIe(msg, &ho_cancel_ies);
    if (decode_status < 0)
    {
        log_msg(LOG_ERROR, "Failed to decode HO Cancel");

        if (ho_cancel_ies.data != NULL)
            free(ho_cancel_ies.data);

        return E_FAIL;
    }
    for (int i = 0; i < ho_cancel_ies.no_of_IEs; i++)
    {
        switch (ho_cancel_ies.data[i].IE_type)
        {
        case S1AP_IE_MME_UE_ID:
        {
            log_msg(LOG_INFO, "handover cancel S1AP_IE_MME_UE_ID.");

            ho_cancel.header.ue_idx = ho_cancel_ies.data[i].val.mme_ue_s1ap_id;
        }
            break;
        case S1AP_IE_ENB_UE_ID:
        {
            log_msg(LOG_INFO,"handover cancel S1AP_IE_ENB_UE_ID.");

            ho_cancel.header.s1ap_enb_ue_id = ho_cancel_ies.data[i].val.enb_ue_s1ap_id;

        }
            break;
       case S1AP_IE_CAUSE:
        {
            log_msg(LOG_INFO, "handover cancel S1AP_IE_CAUSE.");

            memcpy(&ho_cancel.cause,
                    &ho_cancel_ies.data[i].val.cause,
                    sizeof(struct s1apCause));
        }
            break;
        default:
            log_msg(LOG_WARNING, "Unhandled IE");
        }
    }

    ho_cancel.header.msg_type = handover_cancel;
    ho_cancel.header.destInstAddr = htonl(mmeAppInstanceNum_c);
    ho_cancel.header.srcInstAddr = htonl(s1apAppInstanceNum_c);

    int i = send_tipc_message(ipc_S1ap_Hndl, mmeAppInstanceNum_c,
            (char*) &ho_cancel,
            sizeof(ho_cancel));

    if (i < 0)
    {
        log_msg(LOG_ERROR, "Error To write in s1_handover_cancel_handler");
    }
    if (ho_cancel_ies.data != NULL)
        free(ho_cancel_ies.data);
    return SUCCESS;
}

