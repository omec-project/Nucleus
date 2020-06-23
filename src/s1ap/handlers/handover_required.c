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
#include "s1apContextManager/s1apContextWrapper_c.h"
#include "msgType.h"

extern ipc_handle ipc_S1ap_Hndl;

void dumpHoRequired(struct handover_required_Q_msg *msg)
{
    log_msg(LOG_INFO, "MME-UE-S1AP-ID %d\n", msg->s1ap_mme_ue_id);
    log_msg(LOG_INFO, "eNB-UE-S1AP-ID %d\n", msg->s1ap_enb_ue_id);
    log_msg(LOG_INFO, "HO Type %d\n", msg->handoverType);
    log_msg(LOG_INFO, "S1AP Cause %d\n", msg->cause.choice.radioNetwork);
    log_msg(LOG_INFO, "Transp Cont Size %d\n",
            msg->srcToTargetTranspContainer.count);

    /*char container[4096] = {0};
     int j = 0;
     for (int i= 0; i < msg->srcToTargetTranspContainer.count; i++)
     {
     sprintf(&container[j], "%02x ", (int)msg->srcToTargetTranspContainer.buffer[i]);
     j = j+3;
     }
     log_msg(LOG_INFO, "Container is \n %s \n", container);*/
}

int s1_handover_required_handler(InitiatingMessage_t *msg, int enb_fd)
{
    struct s1_incoming_msg_data_t ho_required = {0};
    struct proto_IE ho_required_ies = {0};
    int enb_id = 0;
    log_msg(LOG_INFO, "Parse s1ap handover required message\n");

    int decode_status = convertUehoReqToProtoIe(msg, &ho_required_ies);
    if (decode_status < 0)
    {
        log_msg(LOG_ERROR, "Failed to decode HO Required\n");

        if (ho_required_ies.data != NULL)
            free(ho_required_ies.data);

        return E_FAIL;
    }

    for (int i = 0; i < ho_required_ies.no_of_IEs; i++)
    {
        switch (ho_required_ies.data[i].IE_type)
        {
        case S1AP_IE_MME_UE_ID:
        {

            ho_required.ue_idx = ho_required_ies.data[i].val.mme_ue_s1ap_id;
            ho_required.msg_data.handover_required_Q_msg_m.s1ap_mme_ue_id =
                    ho_required_ies.data[i].val.mme_ue_s1ap_id;
            log_msg(LOG_INFO, "handover required S1AP_IE_MME_UE_ID %u .\n",ho_required.ue_idx);
        }
            break;
        case S1AP_IE_ENB_UE_ID:
        {

            ho_required.msg_data.handover_required_Q_msg_m.s1ap_enb_ue_id =
                    ho_required_ies.data[i].val.enb_ue_s1ap_id;

            log_msg(LOG_INFO, "handover required S1AP_IE_ENB_UE_ID %u .\n",ho_required.msg_data.handover_required_Q_msg_m.s1ap_enb_ue_id);
        }
            break;
        case S1AP_IE_HANDOVER_TYPE:
        {
            log_msg(LOG_INFO, "handover required S1AP_IE_HANDOVER_TYPE.\n");

            ho_required.msg_data.handover_required_Q_msg_m.handoverType =
                    ho_required_ies.data[i].val.handoverType;
        }
            break;
        case S1AP_IE_CAUSE:
        {
            log_msg(LOG_INFO, "handover required S1AP_IE_CAUSE.\n");

            memcpy(&ho_required.msg_data.handover_required_Q_msg_m.cause,
                    &ho_required_ies.data[i].val.cause,
                    sizeof(struct s1apCause));
        }
            break;
        case S1AP_IE_TARGET_ID:
        {
            log_msg(LOG_INFO, "handover required S1AP_IE_TARGET_ID.\n");

            memcpy(&ho_required.msg_data.handover_required_Q_msg_m.target_id,
                    &ho_required_ies.data[i].val.target_id,
                    sizeof(struct targetId));

            char *enb_id_buf = ho_required_ies.data[i].val.target_id.global_enb_id.macro_enb_id;

            enb_id =
                (enb_id_buf[0] << 12) +
                (enb_id_buf[1] << 4) + ((enb_id_buf[2] & 0xf0) >> 4);

            log_msg(LOG_INFO, "enbId %d\n", enb_id);

        }
            break;
        case S1AP_IE_SOURCE_TOTARGET_TRANSPARENTCONTAINER:
        {
            log_msg(LOG_INFO,
                    "handover required S1AP_IE_SOURCE_TOTARGET_TRANSPARENTCONTAINER.\n");

            ho_required.msg_data.handover_required_Q_msg_m.srcToTargetTranspContainer.count =
                    ho_required_ies.data[i].val.srcToTargetTranspContainer.size;

            memcpy(
                    ho_required.msg_data.handover_required_Q_msg_m.srcToTargetTranspContainer.buffer,
                    ho_required_ies.data[i].val.srcToTargetTranspContainer.buffer_p,
                    ho_required_ies.data[i].val.srcToTargetTranspContainer.size);
        }
            break;
        default:
            log_msg(LOG_WARNING, "Unhandled IE");
        }
    }

    if (enb_id == 0)
    {
        log_msg(LOG_ERROR, "Failed to decode target enb id %d", enb_id);

        if (ho_required_ies.data != NULL)
            free(ho_required_ies.data);

        return E_FAIL;
    }

    uint32_t cbIndex = findControlBlockWithEnbId(enb_id);
    if (cbIndex == INVALID_CB_INDEX)
    {
        log_msg(LOG_ERROR, "Failed to find cb for enb id %d", enb_id);

        if (ho_required_ies.data != NULL)
            free(ho_required_ies.data);

        return E_FAIL;
    }
    ho_required.msg_data.handover_required_Q_msg_m.target_enb_context_id = cbIndex;
    cbIndex = findControlBlockWithEnbFd(enb_fd);
    if (cbIndex == INVALID_CB_INDEX)
    {
	log_msg(LOG_ERROR, "No CB found for enb fd %d.\n", enb_fd);
	return E_FAIL;
    }
    ho_required.msg_data.handover_required_Q_msg_m.src_enb_context_id = cbIndex;

    ho_required.msg_type = handover_required;
    ho_required.destInstAddr = htonl(mmeAppInstanceNum_c);
    ho_required.srcInstAddr = htonl(s1apAppInstanceNum_c);

    int i = send_tipc_message(ipc_S1ap_Hndl, mmeAppInstanceNum_c,
            (char*) &ho_required,
            S1_READ_MSG_BUF_SIZE);

    if (i < 0)
    {
        log_msg(LOG_ERROR, "Error To write in s1_handover_required_handler\n");
    }

    if (ho_required_ies.data != NULL)
        free(ho_required_ies.data);

    return SUCCESS;
}

