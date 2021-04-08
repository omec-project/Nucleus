/*
 * Copyright 2020-present, Infosys Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "ipc_api.h"
#include "s1ap.h"
#include "sctp_conn.h"
#include "s1ap_msg_codes.h"
#include "msgType.h"

extern ipc_handle ipc_S1ap_Hndl;

void dump_erab_rel_resp(struct erab_rel_resp_Q_msg *msg)
{
    log_msg(LOG_INFO, "MME-UE-S1AP-ID %d", msg->header.ue_idx);
    log_msg(LOG_INFO, "eNB-UE-S1AP-ID %d", msg->header.s1ap_enb_ue_id);
    log_msg(LOG_INFO, "erab_release_list_count %d",
            msg->erab_rel_list.count);
    for (int i = 0; i < msg->erab_rel_list.count; i++)
    {
        log_msg(LOG_INFO, "erab_release item %d::erab_id = %d", i,
                msg->erab_rel_list.erab_id[i]);
    }
    log_msg(LOG_INFO, "erab_failed_to_release_list count %d",
            msg->erab_failed_to_release_list.count);
    for (int i = 0; i < msg->erab_failed_to_release_list.count; i++)
    {
        log_msg(LOG_INFO, "erab_failed_to_release item [%d]::erab_id = %d", i,
                msg->erab_failed_to_release_list.erab_item[i].e_RAB_ID);
        log_msg(LOG_INFO, "erab_failed_to_release item [%d]::Cause = %lu", i,
                msg->erab_failed_to_release_list.erab_item[i].cause.choice.misc);
    }

}

int erab_release_response_handler(SuccessfulOutcome_t *msg)
{
    struct erab_rel_resp_Q_msg erab_rel_resp = { 0 };
    struct proto_IE erab_rel_resp_ies = { 0 };

    int decode_status = convertErabRelRespToProtoIe(msg, &erab_rel_resp_ies);
    if(decode_status < 0)
    {
        log_msg(LOG_ERROR, "Failed to decode ERAB Release Response");

        if(erab_rel_resp_ies.data != NULL)
            free(erab_rel_resp_ies.data);

        return E_FAIL;
    }

    for (int i = 0; i < erab_rel_resp_ies.no_of_IEs; i++)
    {
        switch (erab_rel_resp_ies.data[i].IE_type)
        {
            case S1AP_IE_MME_UE_ID:
            {
                log_msg(LOG_INFO,
                        "ERAB Release Response S1AP_IE_MME_UE_ID %lu",
                        erab_rel_resp_ies.data[i].val.mme_ue_s1ap_id);

                erab_rel_resp.header.ue_idx =
                        erab_rel_resp_ies.data[i].val.mme_ue_s1ap_id;
            }
                break;
            case S1AP_IE_ENB_UE_ID:
            {
                log_msg(LOG_INFO,
                        "ERAB Release Response S1AP_IE_ENB_UE_ID %lu",
                        erab_rel_resp_ies.data[i].val.enb_ue_s1ap_id);

                erab_rel_resp.header.s1ap_enb_ue_id =
                        erab_rel_resp_ies.data[i].val.enb_ue_s1ap_id;
            }
                break;
            case S1AP_IE_E_RAB_RELEASE_LIST_BEARER_REL_COMP:
            {
                log_msg(LOG_INFO,
                        "ERAB Release Response S1AP_IE_E_RAB_TO_BE_RELEASED_LIST \
                     received with the count : %d",
                        erab_rel_resp_ies.data[i].val.erab_releaselist.count);

                erab_rel_resp.erab_rel_list.count =
                        erab_rel_resp_ies.data[i].val.erab_releaselist.count;
                memcpy(&erab_rel_resp.erab_rel_list.erab_id,
                       &erab_rel_resp_ies.data[i].val.erab_releaselist.erab_id,
                       sizeof(erab_rel_resp_ies.data[i].val.erab_releaselist.erab_id));
            }
                break;
            case S1AP_IE_E_RAB_FAILED_TO_RELEASED_LIST:
            {
                log_msg(LOG_INFO,
                        "ERAB Release Response S1AP_IE_E_RAB_FAILED_TO_RELEASED_LIST \
                     received with the count : %d",
                        erab_rel_resp_ies.data[i].val.erab_failed_to_release_list.count);

                erab_rel_resp.erab_failed_to_release_list.count =
                        erab_rel_resp_ies.data[i].val.erab_failed_to_release_list.count;
                memcpy(&erab_rel_resp.erab_failed_to_release_list.erab_item,
                       &erab_rel_resp_ies.data[i].val.erab_failed_to_release_list.erab_item,
                       sizeof(erab_rel_resp_ies.data[i].val.erab_failed_to_release_list.erab_item));
            }
                break;
            default:
                log_msg(LOG_WARNING, "Unhandled IE %d",
                        erab_rel_resp_ies.data[i].IE_type);
        }
    }

    erab_rel_resp.header.msg_type = erab_release_response;
    erab_rel_resp.header.destInstAddr = htonl(mmeAppInstanceNum_c);
    erab_rel_resp.header.srcInstAddr = htonl(s1apAppInstanceNum_c);
    dump_erab_rel_resp(&erab_rel_resp);

    int i = send_tipc_message(ipc_S1ap_Hndl, mmeAppInstanceNum_c,
                              (char*) &erab_rel_resp,
                              sizeof(struct erab_rel_resp_Q_msg));
    if(i < 0)
    {
        log_msg(LOG_ERROR, "Error To write in erab_rel_resp_handler %s",
                strerror(errno));
    }

    log_msg(LOG_INFO, "ERAB Release Response sent to mme-app. Bytes sent %d",
            i);

    if(erab_rel_resp_ies.data != NULL)
        free(erab_rel_resp_ies.data);

    return SUCCESS;
}

