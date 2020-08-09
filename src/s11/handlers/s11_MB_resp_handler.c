/*
 * Copyright 2019-present Open Networking Foundation
 * Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
 * Copyright (c) 2017 Intel Corporation
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
s11_MB_resp_handler(MsgBuffer* message, GtpV2MessageHeader* hdr, uint32_t sgw_ip)
{
	
	struct gtp_incoming_msg_data_t mbr_info;

	/*****Message structure***
	*/
	log_msg(LOG_INFO, "Parse S11 MB resp message\n");

	//TODO : check cause foor the result verification
	
	/*Check whether has teid flag is set. Also check whether this check is needed for CSR.*/
	if(hdr->teid)
    {
        mbr_info.ue_idx = hdr->teid;
    }
    else
    {
        log_msg(LOG_WARNING, "Unknown Teid in MBR.\n");
        mbr_info.ue_idx = find_gtp_transaction(hdr->sequenceNumber);
    }

    delete_gtp_transaction(hdr->sequenceNumber);

	mbr_info.msg_type = modify_bearer_response;

	ModifyBearerResponseMsgData msgData;
	memset(&msgData, 0, sizeof(ModifyBearerResponseMsgData));

	bool rc = GtpV2Stack_decodeMessage(gtpStack_gp, hdr, message, &msgData);
	if (rc == false)
	{
		log_msg(LOG_ERROR, "s11_MB_resp_handler: "
				"Failed to decode MB_resp Msg %d\n", hdr->teid);
		return E_PARSING_FAILED;
	}
	mbr_info.msg_data.MB_resp_Q_msg_m.cause = msgData.cause.causeValue;

	mbr_info.destInstAddr = htonl(mmeAppInstanceNum_c);
	mbr_info.srcInstAddr = htonl(s11AppInstanceNum_c);

	send_tipc_message(g_resp_fd, mmeAppInstanceNum_c, (char *)&mbr_info, GTP_READ_MSG_BUF_SIZE);
	log_msg(LOG_INFO, "Send MB resp to mme-app stage8.\n");

	return SUCCESS;
}
