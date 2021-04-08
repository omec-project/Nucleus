/*
 * Copyright (c) 2020, Infosys Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "err_codes.h"
#include "s11_options.h"
#include "ipc_api.h"
#include "s11.h"
#include "s11_config.h"
#include "msgType.h"

#include <gtpV2StackWrappers.h>
/*Globals and externs*/
extern int g_resp_fd;
extern struct GtpV2Stack* gtpStack_gp;
/*End : globals and externs*/

int
s11_DB_req_handler(MsgBuffer* message, GtpV2MessageHeader* hdr, uint32_t sgw_ip, uint16_t src_port)
{
    struct db_req_Q_msg dbr_info =
    { 0 };

    dbr_info.s11_mme_cp_teid = hdr->teid;
    dbr_info.header.msg_type = delete_bearer_request;
    dbr_info.seq_no = hdr->sequenceNumber;
    dbr_info.sgw_ip = sgw_ip;
    dbr_info.source_port = src_port;

    DeleteBearerRequestMsgData msgData;
    memset(&msgData, 0, sizeof(DeleteBearerRequestMsgData));

    bool rc = GtpV2Stack_decodeMessage(gtpStack_gp, hdr, message, &msgData);
    if (rc == false)
    {
        log_msg(LOG_ERROR, "s11_DB_req_handler: "
                "Failed to decode Delete Bearer Request Msg %d", hdr->teid);
        return E_PARSING_FAILED;
    }

    if (msgData.epsBearerIdsCount > 0)
    {
        /*Rewriting eps_bearer_ids_count as 1, since we support only one bearer ctxt in the Q msg struct currently*/
        dbr_info.eps_bearer_ids_count = 1;
        for (int i = 0; i < dbr_info.eps_bearer_ids_count; i++)
        {
            dbr_info.eps_bearer_ids[i] = msgData.epsBearerIds[i].epsBearerId;
        }
    }
    else
    {
        dbr_info.linked_bearer_id = msgData.linkedEpsBearerId.epsBearerId;
    }

    if (msgData.causeIePresent)
    {
        dbr_info.cause = msgData.cause.causeValue;
    }

    dbr_info.pco.pco_length = 0;
    if (msgData.protocolConfigurationOptionsIePresent)
    {
        dbr_info.pco.pco_length =
                msgData.protocolConfigurationOptions.pcoValue.count;
        memcpy(dbr_info.pco.pco_options,
                &msgData.protocolConfigurationOptions.pcoValue.values[0],
                msgData.protocolConfigurationOptions.pcoValue.count);
    }

    dbr_info.header.destInstAddr = htonl(mmeAppInstanceNum_c);
    dbr_info.header.srcInstAddr = htonl(s11AppInstanceNum_c);

    /*Send DB request msg*/
    log_msg(LOG_INFO, "Send DB req to mme-app.");
    send_tipc_message(g_resp_fd, mmeAppInstanceNum_c, (char*) &dbr_info,
            sizeof(struct db_req_Q_msg));
    return SUCCESS;
}
