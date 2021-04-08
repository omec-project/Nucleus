/*
 * Copyright (c) 2019, Infosys Ltd.
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
#include "gtp_cpp_wrapper.h"

/*Globals and externs*/
extern int g_resp_fd;
extern struct GtpV2Stack* gtpStack_gp;

/*End : globals and externs*/

int
s11_RB_resp_handler(MsgBuffer* message, GtpV2MessageHeader* hdr, uint32_t sgw_ip)
{	
	struct RB_resp_Q_msg rbr_info;
	
	/*****Message structure***
	*/
	log_msg(LOG_INFO, "Parse S11 RB resp message");
	
	//TODO : check cause for the result verification
	
	if(hdr->teid)
    {
        rbr_info.s11_mme_cp_teid = hdr->teid;
    }
    else
    {
        log_msg(LOG_WARNING, "Unknown Teid in RABR.");
        rbr_info.s11_mme_cp_teid = find_gtp_transaction(hdr->sequenceNumber);
    }

    delete_gtp_transaction(hdr->sequenceNumber);
	rbr_info.header.msg_type = release_bearer_response;
	
	ReleaseAccessBearersResponseMsgData msgData;
        memset(&msgData, 0, sizeof(ReleaseAccessBearersResponseMsgData));

        bool rc = GtpV2Stack_decodeMessage(gtpStack_gp, hdr, message, &msgData);
        if(rc == false)
        {
                        log_msg(LOG_ERROR, "s11_RB_resp_handler: "
                                                                "Failed to decode Release Access Bearer Response Msg %d",
                                                                hdr->teid);
                        return E_PARSING_FAILED;
        }

			
	rbr_info.header.destInstAddr = htonl(mmeAppInstanceNum_c);
	rbr_info.header.srcInstAddr = htonl(s11AppInstanceNum_c);

	/*Send RAB Release response msg*/
	log_msg(LOG_INFO, "Send RB resp to mme-app stage2.");
	send_tipc_message(g_resp_fd, mmeAppInstanceNum_c, (char *)&rbr_info, sizeof(struct RB_resp_Q_msg));

	return SUCCESS;
}
