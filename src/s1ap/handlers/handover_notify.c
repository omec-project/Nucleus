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

int s1_handover_notify_handler(InitiatingMessage_t *msg)
{
    handover_notify_Q_msg_t notify = {0};
    struct proto_IE ho_notify_ies = {0};
    log_msg(LOG_INFO, "Parse s1ap handover notify message");

    int decode_status = convertHoNotifyToProtoIe(msg, &ho_notify_ies);
    if (decode_status < 0)
    {
        log_msg(LOG_ERROR, "Failed to decode HO Notify");

        if (ho_notify_ies.data != NULL)
            free(ho_notify_ies.data);

        return E_FAIL;
    }

    for (int i = 0; i < ho_notify_ies.no_of_IEs; i++)
    {
        switch (ho_notify_ies.data[i].IE_type)
        {
        case S1AP_IE_MME_UE_ID:
        {
            log_msg(LOG_INFO, "handover notify S1AP_IE_MME_UE_ID.");
            notify.header.ue_idx = ho_notify_ies.data[i].val.mme_ue_s1ap_id;
            notify.s1ap_mme_ue_id =
                    ho_notify_ies.data[i].val.mme_ue_s1ap_id;
        }
            break;
        case S1AP_IE_ENB_UE_ID:
        {
            log_msg(LOG_INFO, "handover notify S1AP_IE_ENB_UE_ID.");
            notify.header.s1ap_enb_ue_id =
                    ho_notify_ies.data[i].val.enb_ue_s1ap_id;
        }
            break;
        case S1AP_IE_UTRAN_CGI:
        {
            log_msg(LOG_INFO, "handover notify S1AP_IE_UTRAN_CGI.");
            memcpy(&notify.utran_cgi,
                    &ho_notify_ies.data[i].val.utran_cgi, sizeof(struct CGI));
        }
            break;
        case S1AP_IE_TAI:
        {
            log_msg(LOG_INFO, "handover notify S1AP_IE_TAI.");
            memcpy(&notify.tai,
                    &ho_notify_ies.data[i].val.tai, sizeof(struct TAI));
        }
            break;

        default:
            log_msg(LOG_WARNING, "Unhandled IE");
        }
    }

    notify.header.msg_type = handover_notify;
    notify.header.destInstAddr = htonl(mmeAppInstanceNum_c);
    notify.header.srcInstAddr = htonl(s1apAppInstanceNum_c);
    int i = send_tipc_message(ipc_S1ap_Hndl, mmeAppInstanceNum_c,
            (char*) &notify,
            sizeof(notify));
    if (i < 0)
    {
        log_msg(LOG_ERROR, "Error To write in s1_handover_notify_handler");
    }

    log_msg(LOG_INFO, "Handover notify complete sent to mme-app."
            "Bytes sent %d", i);

    if (ho_notify_ies.data != NULL)
        free(ho_notify_ies.data);

    return SUCCESS;
}

