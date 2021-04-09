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

int s1_enb_status_transfer_handler(InitiatingMessage_t *msg)
{
    enb_status_transfer_Q_msg_t ho_enb_status_transfer = {0};
    struct proto_IE ho_enb_status_transfer_ies = {0};
    log_msg(LOG_INFO, "Parse s1ap handover enb_status_transfer message");

    int decode_status = convertEnbStatusTransferToProtoIe(msg, &ho_enb_status_transfer_ies);
    if (decode_status < 0)
    {
        log_msg(LOG_ERROR, "Failed to decode enb-status-transfer message");

        if (ho_enb_status_transfer_ies.data != NULL)
            free(ho_enb_status_transfer_ies.data);

        return E_FAIL;
    }

    for (int i = 0; i < ho_enb_status_transfer_ies.no_of_IEs; i++)
    {
        switch (ho_enb_status_transfer_ies.data[i].IE_type)
        {
        case S1AP_IE_MME_UE_ID:
        {
            log_msg(LOG_INFO,
                    "handover enb_status_transfer S1AP_IE_MME_UE_ID.");
            ho_enb_status_transfer.header.ue_idx =
                    ho_enb_status_transfer_ies.data[i].val.mme_ue_s1ap_id;
            ho_enb_status_transfer.s1ap_mme_ue_id =
                    ho_enb_status_transfer_ies.data[i].val.mme_ue_s1ap_id;
        }
            break;
        case S1AP_IE_ENB_UE_ID:
        {
            log_msg(LOG_INFO,
                    "handover enb_status_transfer S1AP_IE_ENB_UE_ID.");
            ho_enb_status_transfer.header.s1ap_enb_ue_id =
                    ho_enb_status_transfer_ies.data[i].val.enb_ue_s1ap_id;
        }
            break;

        case S1AP_IE_ENB_STATUS_TRANSFER_TRANSPARENTCONTAINER:
        {
            log_msg(LOG_INFO, "handover enb_status_transfer S1AP_IE_TAI.");
            memcpy(
                    &ho_enb_status_transfer.enB_status_transfer_transparent_containerlist,
                    &ho_enb_status_transfer_ies.data[i].val.enB_status_transfer_transparent_containerlist,
                    sizeof(struct enB_status_transfer_transparent_container_list));
        }
            break;

        default:
            log_msg(LOG_WARNING, "Unhandled IE");
        }
    }

    ho_enb_status_transfer.header.msg_type = enb_status_transfer;
    ho_enb_status_transfer.header.destInstAddr = htonl(mmeAppInstanceNum_c);
    ho_enb_status_transfer.header.srcInstAddr = htonl(s1apAppInstanceNum_c);

    int i = send_tipc_message(ipc_S1ap_Hndl, mmeAppInstanceNum_c,
            (char*) &ho_enb_status_transfer,
            sizeof(ho_enb_status_transfer));
    if (i < 0)
    {
        log_msg(LOG_ERROR,
                "Error To write in s1_enb_status_transfer_handler");
    }

    log_msg(LOG_INFO, "enb_status_transfer sent to mme-app."
            "Bytes sent %d", i);

    if (ho_enb_status_transfer_ies.data != NULL)
        free(ho_enb_status_transfer_ies.data);

    return SUCCESS;
}


