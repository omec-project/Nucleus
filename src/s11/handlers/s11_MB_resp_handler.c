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
	
	struct MB_resp_Q_msg mbr_info;

	/*****Message structure***
	*/
	log_msg(LOG_INFO, "Parse S11 MB resp message");

	//TODO : check cause foor the result verification
	
	/*Check whether has teid flag is set. Also check whether this check is needed for CSR.*/
	if(hdr->teid)
    {
        mbr_info.s11_mme_cp_teid = hdr->teid;
    }
    else
    {
        log_msg(LOG_WARNING, "Unknown Teid in MBR.");
        mbr_info.s11_mme_cp_teid = find_gtp_transaction(hdr->sequenceNumber);
    }

    delete_gtp_transaction(hdr->sequenceNumber);

	mbr_info.header.msg_type = modify_bearer_response;

	ModifyBearerResponseMsgData msgData;
	memset(&msgData, 0, sizeof(ModifyBearerResponseMsgData));

	bool rc = GtpV2Stack_decodeMessage(gtpStack_gp, hdr, message, &msgData);
	if (rc == false)
	{
		log_msg(LOG_ERROR, "s11_MB_resp_handler: "
				"Failed to decode MB_resp Msg %d", hdr->teid);
		return E_PARSING_FAILED;
	}
	mbr_info.cause = msgData.cause.causeValue;

	mbr_info.bearer_ctxt_mb_resp_list.bearers_count = msgData.bearerContextsModifiedCount;

	for(int i = 0; i < msgData.bearerContextsModifiedCount; i++)
	{
		mbr_info.bearer_ctxt_mb_resp_list.bearer_ctxt[i].eps_bearer_id =
				msgData.bearerContextsModified[i].epsBearerId.epsBearerId;
		mbr_info.bearer_ctxt_mb_resp_list.bearer_ctxt[i].cause.cause =
				msgData.bearerContextsModified[i].cause.causeValue;
		if(msgData.bearerContextsModified[i].s1USgwFTeidIePresent)
		{
			mbr_info.bearer_ctxt_mb_resp_list.bearer_ctxt[i].s1u_sgw_teid.header.v4 =
					msgData.bearerContextsModified[i].s1USgwFTeid.ipv4present;
			mbr_info.bearer_ctxt_mb_resp_list.bearer_ctxt[i].s1u_sgw_teid.header.iface_type =
					msgData.bearerContextsModified[i].s1USgwFTeid.interfaceType;
			mbr_info.bearer_ctxt_mb_resp_list.bearer_ctxt[i].s1u_sgw_teid.header.teid_gre =
					msgData.bearerContextsModified[i].s1USgwFTeid.teidGreKey;
			mbr_info.bearer_ctxt_mb_resp_list.bearer_ctxt[i].s1u_sgw_teid.ip.ipv4.s_addr =
					msgData.bearerContextsModified[i].s1USgwFTeid.ipV4Address.ipValue;

		}
	}
	mbr_info.header.destInstAddr = htonl(mmeAppInstanceNum_c);
	mbr_info.header.srcInstAddr = htonl(s11AppInstanceNum_c);

	log_msg(LOG_INFO, "Send MB resp to mme-app stage8.");
	send_tipc_message(g_resp_fd, mmeAppInstanceNum_c, (char *)&mbr_info, sizeof(struct MB_resp_Q_msg));

	return SUCCESS;
}
