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
#include "s10_options.h"
#include "ipc_api.h"
#include "s10.h"
#include "s10_config.h"
#include "msgType.h"

#include <gtpV2StackWrappers.h>
#include "gtp_cpp_wrapper.h"

/*Globals and externs*/
extern int g_resp_fd;
extern struct GtpV2Stack* gtpStack_gp;
/*End : globals and externs*/


int
s10_Fwd_acc_ctxt_ack_handler(MsgBuffer* message, GtpV2MessageHeader* hdr, uint32_t sgw_ip)
{

	struct Fwd_Acc_Ctxt_Ack_Q_msg fwd_acc_ack = {0};
	/*****Message structure***
	*/

	/*Check whether has teid flag is set. Also check whether this check is needed for CSR.*/
	fwd_acc_ack.s10_mme_cp_teid = hdr->teid;
	fwd_acc_ack.header.msg_type = forward_access_context_ack;

    delete_gtp_transaction(hdr->sequenceNumber);
    ForwardAccessContextAcknowledgeMsgData msgData;
	memset(&msgData, 0, sizeof(ForwardAccessContextAcknowledgeMsgData));

	bool rc = GtpV2Stack_decodeMessage(gtpStack_gp, hdr, message, &msgData);
	if(rc == false)
	{
			log_msg(LOG_ERROR, "s10_Fwd_acc_ctxt_ack_handler: "
								"Failed to decode forward access context ack Msg %d",
								hdr->teid);
			return E_PARSING_FAILED;
	}
	fwd_acc_ack.cause = msgData.cause.causeValue;

	fwd_acc_ack.header.destInstAddr = htonl(mmeAppInstanceNum_c);
	fwd_acc_ack.header.srcInstAddr = htonl(s10AppInstanceNum_c);

	/*Send CS response msg*/
	log_msg(LOG_INFO, "Send Forward Access Context Ack to mme-app stage6.");
	send_tipc_message(g_resp_fd, mmeAppInstanceNum_c, (char *)&fwd_acc_ack, sizeof(struct Fwd_Acc_Ctxt_Ack_Q_msg));

	return SUCCESS;
}
