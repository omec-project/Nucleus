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

void dumpHOReqAck(struct handover_req_acknowledge_Q_msg* msg)
{
    log_msg(LOG_INFO, "MME-UE-S1AP-ID %d", msg->s1ap_mme_ue_id);
    log_msg(LOG_INFO, "eNB-UE-S1AP-ID %d", msg->header.s1ap_enb_ue_id);
    log_msg(LOG_INFO, "eRAB Admitted list count %d", msg->erab_admitted_list.count);
    log_msg(LOG_INFO, "eRAB ID %d", msg->erab_admitted_list.erab_admitted[0].e_RAB_ID);
    log_msg(LOG_INFO, "eRAB GTP TEID %d", msg->erab_admitted_list.erab_admitted[0].gtp_teid);
    log_msg(LOG_INFO, "eRAB S1uENB IP %x", msg->erab_admitted_list.erab_admitted[0].transportLayerAddress);
    log_msg(LOG_INFO, "eRAB DL GTP TEID %d", msg->erab_admitted_list.erab_admitted[0].dL_gtp_teid);
    log_msg(LOG_INFO, "eRAB DL IP %x", msg->erab_admitted_list.erab_admitted[0].dL_transportLayerAddress);
    log_msg(LOG_INFO, "TRANS Container Size %d", msg->targetToSrcTranspContainer.count);
}

int s1_handover_ack_handler(SuccessfulOutcome_t *msg)
{
    struct handover_req_acknowledge_Q_msg handover_ack = {0};
    struct proto_IE s1_ho_ack_ies = {0};

    int decode_status = convertHoAcklToProtoIe(msg, &s1_ho_ack_ies); //create
    if (decode_status < 0)
    {
        log_msg(LOG_ERROR, "Failed to decode HO Request Ack");

        if (s1_ho_ack_ies.data != NULL)
            free(s1_ho_ack_ies.data);

        return E_FAIL;
    }

    /*TODO: Validate all eNB info*/
    for (int i = 0; i < s1_ho_ack_ies.no_of_IEs; i++)
    {
        switch (s1_ho_ack_ies.data[i].IE_type)
        {
        case S1AP_IE_MME_UE_ID:
        {
            log_msg(LOG_INFO,
                    "Handover Request Ack S1AP_IE_MME_UE_ID.");

            handover_ack.header.ue_idx = s1_ho_ack_ies.data[i].val.mme_ue_s1ap_id;
            handover_ack.s1ap_mme_ue_id =
                    s1_ho_ack_ies.data[i].val.mme_ue_s1ap_id;
        }
            break;
        case S1AP_IE_ENB_UE_ID:
        {
            log_msg(LOG_INFO,
                    "Handover Request Ack S1AP_IE_ENB_UE_ID.");

            handover_ack.header.s1ap_enb_ue_id =
                    s1_ho_ack_ies.data[i].val.enb_ue_s1ap_id;
        }
            break;
        case S1AP_IE_E_RAB_ADMITTED:
        {
            log_msg(LOG_INFO,
                    "Handover Request Ack S1AP_IE_E_RAB_ADMITTED.");

            handover_ack.erab_admitted_list.count =
                    s1_ho_ack_ies.data[i].val.erab_admittedlist.count;
            memcpy(
                    &handover_ack.erab_admitted_list.erab_admitted,
                    &s1_ho_ack_ies.data[i].val.erab_admittedlist.erab_admitted,
                    sizeof(s1_ho_ack_ies.data[i].val.erab_admittedlist.erab_admitted));
        }
            break;
        case S1AP_IE_TARGET_TOSOURCE_TRANSPARENTCONTAINER:
        {
            log_msg(LOG_INFO,
                    "Handover Request Ack S1AP_IE_TARGET_TOSOURCE_TRANSPARENTCONTAINER.");

            handover_ack.targetToSrcTranspContainer.count =
                    s1_ho_ack_ies.data[i].val.targetToSrcTranspContainer.size;

            memcpy(
                    handover_ack.targetToSrcTranspContainer.buffer,
                    s1_ho_ack_ies.data[i].val.targetToSrcTranspContainer.buffer_p,
                    s1_ho_ack_ies.data[i].val.targetToSrcTranspContainer.size);
        }
            break;

        default:
            log_msg(LOG_WARNING, "Unhandled IE");
        }
    }

    handover_ack.header.msg_type = handover_request_acknowledge;
    handover_ack.header.destInstAddr = htonl(mmeAppInstanceNum_c);
    handover_ack.header.srcInstAddr = htonl(s1apAppInstanceNum_c);

    int i = send_tipc_message(ipc_S1ap_Hndl, mmeAppInstanceNum_c,
            (char*) &handover_ack,
            sizeof(struct handover_req_acknowledge_Q_msg));
    if (i < 0)
    {
        log_msg(LOG_ERROR, "Error To write in s1_handover_ack_handler");
    }

    log_msg(LOG_INFO, "Handover Request Ack sent to mme-app. Bytes sent %d", i);

    if (s1_ho_ack_ies.data != NULL)
        free(s1_ho_ack_ies.data);

    return SUCCESS;
}

