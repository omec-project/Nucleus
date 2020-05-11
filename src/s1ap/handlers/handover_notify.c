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
    struct s1_incoming_msg_data_t notify = {0};
    struct proto_IE ho_notify_ies = {0};
    log_msg(LOG_INFO, "Parse s1ap handover notify message\n");

    int decode_status = convertHoNotifyToProtoIe(msg, &ho_notify_ies);
    if (decode_status < 0)
    {
        log_msg(LOG_ERROR, "Failed to decode HO Notify\n");

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
            log_msg(LOG_INFO, "handover notify S1AP_IE_MME_UE_ID.\n");
            notify.ue_idx = ho_notify_ies.data[i].val.mme_ue_s1ap_id;
            notify.msg_data.handover_notify_Q_msg_m.s1ap_mme_ue_id =
                    ho_notify_ies.data[i].val.mme_ue_s1ap_id;
        }
            break;
        case S1AP_IE_ENB_UE_ID:
        {
            log_msg(LOG_INFO, "handover notify S1AP_IE_ENB_UE_ID.\n");
            notify.msg_data.handover_notify_Q_msg_m.s1ap_enb_ue_id =
                    ho_notify_ies.data[i].val.enb_ue_s1ap_id;
        }
            break;
        case S1AP_IE_UTRAN_CGI:
        {
            log_msg(LOG_INFO, "handover notify S1AP_IE_UTRAN_CGI.\n");
            memcpy(&notify.msg_data.handover_notify_Q_msg_m.utran_cgi,
                    &ho_notify_ies.data[i].val.utran_cgi, sizeof(struct CGI));
        }
            break;
        case S1AP_IE_TAI:
        {
            log_msg(LOG_INFO, "handover notify S1AP_IE_TAI.\n");
            memcpy(&notify.msg_data.handover_notify_Q_msg_m.tai,
                    &ho_notify_ies.data[i].val.tai, sizeof(struct TAI));
        }
            break;

        default:
            log_msg(LOG_WARNING, "Unhandled IE");
        }
    }

    notify.msg_type = handover_notify;
    notify.destInstAddr = htonl(mmeAppInstanceNum_c);
    notify.srcInstAddr = htonl(s1apAppInstanceNum_c);
    int i = send_tipc_message(ipc_S1ap_Hndl, mmeAppInstanceNum_c,
            (char*) &notify,
            S1_READ_MSG_BUF_SIZE);
    if (i < 0)
    {
        log_msg(LOG_ERROR, "Error To write in s1_handover_notify_handler\n");
    }

    log_msg(LOG_INFO, "Handover notify complete sent to mme-app."
            "Bytes sent %d\n", i);

    if (ho_notify_ies.data != NULL)
        free(ho_notify_ies.data);

    return SUCCESS;
}

