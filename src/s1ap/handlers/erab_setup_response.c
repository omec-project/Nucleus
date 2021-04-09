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

void dump_erab_su_resp(struct erabSuResp_Q_msg *msg)
{
   log_msg(LOG_INFO, "MME-UE-S1AP-ID %d", msg->s1ap_mme_ue_id);
   log_msg(LOG_INFO, "eNB-UE-S1AP-ID %d", msg->header.s1ap_enb_ue_id);
   log_msg(LOG_INFO, "eRABSetupList_count %d", msg->erab_su_list.count);
   for(int i = 0; i < msg->erab_su_list.count; i++)
   {
       log_msg(LOG_INFO,"eRAB_setup_item[%d].erab_id: %d", i, msg->erab_su_list.
		       erab_su_item[i].e_RAB_ID);
       struct in_addr tl_address = 
       			{msg->erab_su_list.erab_su_item[i].transportLayerAddress};
       log_msg(LOG_INFO,"eRAB_setup_item[%d].transportLayerAddress: %s", i, inet_ntoa(tl_address));
       log_msg(LOG_INFO,"eRAB_setup_item[%d].gtp_teid: %d", i, msg->erab_su_list.
                       erab_su_item[i].gtp_teid);
   }
   for(int i = 0; i < msg->erab_fail_list.count; i++)
   {
       log_msg(LOG_INFO,"eRAB_failed_to_setup_item[%d].erab_id: %d", i, msg->erab_fail_list.
		       erab_fail_item[i].e_RAB_ID);
   }

}

int erab_setup_response_handler(SuccessfulOutcome_t *msg)
{
    struct erabSuResp_Q_msg erab_su_resp = {0};
    struct proto_IE erab_su_resp_ies = {0};

    int decode_status = convertErabSetupRespToProtoIe(msg, &erab_su_resp_ies);
    if (decode_status < 0)
    {
        log_msg(LOG_ERROR, "Failed to decode ERAB Setup Response");

        if (erab_su_resp_ies.data != NULL)
            free(erab_su_resp_ies.data);

        return E_FAIL;
    }

    for (int i = 0; i < erab_su_resp_ies.no_of_IEs; i++)
    {
        switch (erab_su_resp_ies.data[i].IE_type)
        {
            case S1AP_IE_MME_UE_ID:
            {
                log_msg(LOG_INFO,
                    "ERAB Setup Response S1AP_IE_MME_UE_ID %lu", 
                erab_su_resp_ies.data[i].val.mme_ue_s1ap_id);

                erab_su_resp.header.ue_idx = erab_su_resp_ies.data[i].val.mme_ue_s1ap_id;
            }
            break;
            case S1AP_IE_ENB_UE_ID:
            {
                log_msg(LOG_INFO,
                    "ERAB Setup Response S1AP_IE_ENB_UE_ID %lu",
                erab_su_resp_ies.data[i].val.enb_ue_s1ap_id);

                erab_su_resp.header.s1ap_enb_ue_id = erab_su_resp_ies.data[i].val.enb_ue_s1ap_id;
            }
            break;
            case S1AP_IE_E_RAB_SETUP_LIST_BEARER_SU_RES:
            {   
                log_msg(LOG_INFO,
                    "ERAB Setup Response S1AP_IE_E_RAB_SETUP_LIST_BEARER_SU_RES \
		             received with the count : %d", 
                     erab_su_resp_ies.data[i].val.erab_su_list.count);

                erab_su_resp.erab_su_list.count =
                    erab_su_resp_ies.data[i].val.erab_su_list.count;
                memcpy(
                    &erab_su_resp.erab_su_list.erab_su_item,
                    &erab_su_resp_ies.data[i].val.erab_su_list.erab_su_item,
                    sizeof(erab_su_resp_ies.data[i].val.erab_su_list.erab_su_item));
            }
            break;
            case S1AP_IE_E_RAB_FAILED_TO_SETUP_LIST_BEARER_SU_RES:
            {
                log_msg(LOG_INFO,
                    "ERAB Setup Response S1AP_IE_E_RAB_FAILED_TO_SETUP_LIST_BEARER_SU_RES \
		             received with the count : %d", 
                     erab_su_resp_ies.data[i].val.erab_fail_list.count);

                erab_su_resp.erab_fail_list.count =
                    erab_su_resp_ies.data[i].val.erab_fail_list.count;
                memcpy(
                    &erab_su_resp.erab_fail_list.erab_fail_item,
                    &erab_su_resp_ies.data[i].val.erab_fail_list.erab_fail_item,
                    sizeof(erab_su_resp_ies.data[i].val.erab_fail_list.erab_fail_item));
            }
            break;
            default:
                log_msg(LOG_WARNING, "Unhandled IE %d", erab_su_resp_ies.data[i].IE_type);
        }
    }

    erab_su_resp.header.msg_type = erab_setup_response;
    erab_su_resp.header.destInstAddr = htonl(mmeAppInstanceNum_c);
    erab_su_resp.header.srcInstAddr = htonl(s1apAppInstanceNum_c);

    //dump_erab_su_resp(&erab_su_resp);
    int i = send_tipc_message(ipc_S1ap_Hndl, mmeAppInstanceNum_c,
            (char*) &erab_su_resp,
            sizeof(struct erabSuResp_Q_msg));
    if (i < 0)
    {
        log_msg(LOG_ERROR, "Error To write in erab_su_resp_handler %s", strerror(errno));
    }

    log_msg(LOG_INFO, "ERAB Setup Response sent to mme-app. Bytes sent %d", i);

    free(erab_su_resp_ies.data);

    return SUCCESS;
}
