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

void dump_erab_mod_info(erab_mod_ind_Q_msg_t *msg)
{
   log_msg(LOG_INFO, "MME-UE-S1AP-ID %d", msg->header.ue_idx);
   log_msg(LOG_INFO, "eNB-UE-S1AP-ID %d", msg->header.s1ap_enb_ue_id);
   log_msg(LOG_INFO, "erab_mod_list_count %d", msg->erab_to_be_mod_list.count);
   for(int i = 0; i < msg->erab_to_be_mod_list.count; i++)
   {
       log_msg(LOG_INFO,"erab_to_be_modified_item[%d].erab_id: %d", i, msg->erab_to_be_mod_list.
		       erab_to_be_mod_item[i].e_RAB_ID);
       log_msg(LOG_INFO,"erab_to_be_modified_item[%d].transportLayerAddress: %x", i, msg->erab_to_be_mod_list.
                       erab_to_be_mod_item[i].transportLayerAddress);
       log_msg(LOG_INFO,"erab_to_be_modified_item[%d].dl_gtp_teid: %d", i, msg->erab_to_be_mod_list.
                       erab_to_be_mod_item[i].dl_gtp_teid);
   }

}
int erab_mod_indication_handler(InitiatingMessage_t *msg)
{
    erab_mod_ind_Q_msg_t erab_mod_ind = {0};
    struct proto_IE erab_mod_ind_ies = {0};

    int decode_status = convertErabModIndToProtoIe(msg, &erab_mod_ind_ies);
    if (decode_status < 0)
    {
        log_msg(LOG_ERROR, "Failed to decode ERAB Modification Indication");

        if (erab_mod_ind_ies.data != NULL)
            free(erab_mod_ind_ies.data);

        return E_FAIL;
    }

    for (int i = 0; i < erab_mod_ind_ies.no_of_IEs; i++)
    {
        switch (erab_mod_ind_ies.data[i].IE_type)
        {
        case S1AP_IE_MME_UE_ID:
        {
            log_msg(LOG_INFO,
                    "ERAB Modification Indication S1AP_IE_MME_UE_ID %lu", 
		    erab_mod_ind_ies.data[i].val.mme_ue_s1ap_id);

            erab_mod_ind.header.ue_idx = erab_mod_ind_ies.data[i].val.mme_ue_s1ap_id;
        }
            break;
        case S1AP_IE_ENB_UE_ID:
        {
            log_msg(LOG_INFO,
                    "ERAB Modification Indication S1AP_IE_ENB_UE_ID %lu",
		    erab_mod_ind_ies.data[i].val.enb_ue_s1ap_id);

            erab_mod_ind.header.s1ap_enb_ue_id = erab_mod_ind_ies.data[i].val.enb_ue_s1ap_id;
        }
            break;
        case S1AP_IE_E_RAB_TO_BE_MOD_LIST_BEARER_MOD_IND:
        {
            log_msg(LOG_INFO,
                    "ERAB Modification Indication S1AP_IE_E_RAB_TO_BE_MOD_LIST_BEARER_MOD_IND\
		    received with the count : %d", 
		    erab_mod_ind_ies.data[i].val.erab_to_be_mod_list.count);

            erab_mod_ind.erab_to_be_mod_list.count =
                    erab_mod_ind_ies.data[i].val.erab_to_be_mod_list.count;
            memcpy(
                    &erab_mod_ind.erab_to_be_mod_list.erab_to_be_mod_item,
                    &erab_mod_ind_ies.data[i].val.erab_to_be_mod_list.erab_to_be_mod_item,
                    sizeof(erab_mod_ind_ies.data[i].val.erab_to_be_mod_list.erab_to_be_mod_item));
        }
            break;
        default:
            log_msg(LOG_WARNING, "Unhandled IE %d", erab_mod_ind_ies.data[i].IE_type);
        }
    }

    erab_mod_ind.header.msg_type = erab_mod_indication;
    erab_mod_ind.header.destInstAddr = htonl(mmeAppInstanceNum_c);
    erab_mod_ind.header.srcInstAddr = htonl(s1apAppInstanceNum_c);

    //dump_erab_mod_info(&erab_mod_ind);
    int i = send_tipc_message(ipc_S1ap_Hndl, mmeAppInstanceNum_c,
            (char*) &erab_mod_ind,
            sizeof(erab_mod_ind));
    if (i < 0)
    {
        log_msg(LOG_ERROR, "Error To write in erab_mod_ind_handler %s", strerror(errno));
    }

    log_msg(LOG_INFO, "ERAB Modification Indication sent to mme-app. Bytes sent %d", i);

    if (erab_mod_ind_ies.data != NULL)
        free(erab_mod_ind_ies.data);

    return SUCCESS;
}


