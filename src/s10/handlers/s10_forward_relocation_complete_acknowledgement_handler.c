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
#include "s10.h"
#include "s11_config.h"
#include "msgType.h"

#include <gtpV2StackWrappers.h>
#include "gtp_cpp_wrapper.h"

/*Globals and externs*/
extern int g_resp_fd;
extern struct GtpV2Stack* gtpStack_gp;
/*End : globals and externs*/


int
s10_FWD_relocation_cmp_ack_handler(MsgBuffer* message, GtpV2MessageHeader* hdr, uint32_t tmme_ip)
{

	struct FWD_REL_CMP_ACK_BQ_msg fwd_rel_cmp_ack_info = {0};
	/*****Message structure***
	*/

	
	fwd_rel_cmp_ack_info.header.msg_type = forward_relocation_complete_acknowledgement;

    delete_gtp_transaction(hdr->sequenceNumber);
    ForwardRelocationCompleteAcknowledgeMsgData msgData;
	memset(&msgData, 0, sizeof(ForwardRelocationCompleteAcknowledgeMsgData));

	bool rc = GtpV2Stack_decodeMessage(gtpStack_gp, hdr, message, &msgData);
	if(rc == false)
	{
			log_msg(LOG_ERROR, "s10_forward_relocation_complete_acknowledgement_handler: "
								"Failed to decode Forward Relocation Complete Acknowledgement Msg");
			return E_PARSING_FAILED;
	}

	fwd_rel_cmp_ack_info.cause = msgData.cause.causeValue;
	

	/*Send ID response msg*/
	log_msg(LOG_INFO, "Send Forward Relocation Complete Acknowledge to mme-app stage6.");
	send_tipc_message(g_resp_fd, mmeAppInstanceNum_c, (char *)&fwd_rel_cmp_ack_info, sizeof(struct FWD_REL_CMP_ACK_BQ_msg));

	return SUCCESS;
}
