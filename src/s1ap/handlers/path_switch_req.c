/*
* Copyright 2021-present, Infosys Ltd.
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
#include "s1apContextManager/s1apContextWrapper_c.h"
#include "msgType.h"

extern ipc_handle ipc_S1ap_Hndl;

void dump_path_switch_req(pathSwitchRequest_Msg_t *msg)
{
   log_msg(LOG_INFO, "MME-UE-S1AP-ID %d", msg->header.ue_idx);
   log_msg(LOG_INFO, "eNB-UE-S1AP-ID %d", msg->header.s1ap_enb_ue_id);
   log_msg(LOG_INFO, "erab_to_be_switched_list_count %d", msg->erab_to_be_switched_dl_list.count);
   for(int i = 0; i < msg->erab_to_be_switched_dl_list.count; i++)
   {
       log_msg(LOG_INFO,"eRAB_to_be_switched_item[%d].erab_id: %d", i, msg->erab_to_be_switched_dl_list.
               erab_item[i].e_RAB_ID);
       struct in_addr tl_address =
                {msg->erab_to_be_switched_dl_list.erab_item[i].transportLayerAddress};
       log_msg(LOG_INFO,"eRAB_setup_item[%d].transportLayerAddress: %s", i, inet_ntoa(tl_address));
       log_msg(LOG_INFO,"eRAB_setup_item[%d].gtp_teid: %d", i, msg->erab_to_be_switched_dl_list.
               erab_item[i].gtp_teid);
   }

}

int path_switch_req_handler(InitiatingMessage_t *msg, int enb_fd)
{
    struct pathSwitchRequest_Msg path_switch_req = {0};
    struct proto_IE path_switch_req_ies = {0};

    int decode_status = convertPathSwitchReqToProtoIe(msg, &path_switch_req_ies);
    if (decode_status < 0)
    {
        log_msg(LOG_ERROR, "Failed to decode Path Switch Request");

        if (path_switch_req_ies.data != NULL)
            free(path_switch_req_ies.data);

        return E_FAIL;
    }

    for (int i = 0; i < path_switch_req_ies.no_of_IEs; i++)
    {
        switch (path_switch_req_ies.data[i].IE_type)
        {
            case S1AP_IE_SRC_MME_UE_ID:
            {
                log_msg(LOG_INFO,
                    "Path_Switch_Request S1AP_IE_MME_UE_ID %lu",
                    path_switch_req_ies.data[i].val.mme_ue_s1ap_id);

                path_switch_req.header.ue_idx = path_switch_req_ies.data[i].val.mme_ue_s1ap_id;
            }
            break;
            case S1AP_IE_ENB_UE_ID:
            {
                log_msg(LOG_INFO,
                    "Path_Switch_Request S1AP_IE_ENB_UE_ID %lu",
                    path_switch_req_ies.data[i].val.enb_ue_s1ap_id);

                path_switch_req.header.s1ap_enb_ue_id = path_switch_req_ies.data[i].val.enb_ue_s1ap_id;
            }
            break;
            case S1AP_IE_E_RAB_TO_BE_SWITCHED_DL_LIST:
            {
                log_msg(LOG_INFO,
                    "Path_Switch_Request S1AP_IE_E_RAB_TO_BE_SWITCHED_LIST \
                     received with the count : %d",
                     path_switch_req_ies.data[i].val.erab_to_be_switched_list.count);

                path_switch_req.erab_to_be_switched_dl_list.count =
                        path_switch_req_ies.data[i].val.erab_to_be_switched_list.count;
                memcpy(
                    &path_switch_req.erab_to_be_switched_dl_list.erab_item,
                    &path_switch_req_ies.data[i].val.erab_to_be_switched_list.erab_item,
                    sizeof(path_switch_req_ies.data[i].val.erab_to_be_switched_list.erab_item));
            }
            break;
            case S1AP_IE_UTRAN_CGI:
            {
                log_msg(LOG_INFO, "Path_Switch_Request S1AP_IE_UTRAN_CGI.");
                memcpy(&path_switch_req.cgi,
                        &path_switch_req_ies.data[i].val.utran_cgi, sizeof(struct CGI));
            }
            break;
            case S1AP_IE_TAI:
            {
                log_msg(LOG_INFO, "Path_Switch_Request S1AP_IE_TAI.");
                memcpy(&path_switch_req.tai,
                        &path_switch_req_ies.data[i].val.tai, sizeof(struct TAI));
            }
            break;
            default:
                log_msg(LOG_WARNING, "Unhandled IE %d", path_switch_req_ies.data[i].IE_type);
        }
    }

    uint32_t cbIndex = findControlBlockWithEnbFd(enb_fd);
    if (cbIndex == INVALID_CB_INDEX)
    {
        log_msg(LOG_ERROR, "No CB found for enb fd %d.", enb_fd);
        if (path_switch_req_ies.data != NULL)
            free(path_switch_req_ies.data);
        return E_FAIL;
    }

    path_switch_req.enb_context_id = cbIndex;
    path_switch_req.header.msg_type = path_switch_request;
    path_switch_req.header.destInstAddr = htonl(mmeAppInstanceNum_c);
    path_switch_req.header.srcInstAddr = htonl(s1apAppInstanceNum_c);

    dump_path_switch_req(&path_switch_req);
    int i = send_tipc_message(ipc_S1ap_Hndl, mmeAppInstanceNum_c,
            (char*) &path_switch_req,
            sizeof(struct pathSwitchRequest_Msg));
    if (i < 0)
    {
        log_msg(LOG_ERROR, "Error To write in path_switch_req_handler %s", strerror(errno));
    }

    log_msg(LOG_INFO, "Path Switch Request sent to mme-app. Bytes sent %d", i);

    free(path_switch_req_ies.data);

    return SUCCESS;
}
