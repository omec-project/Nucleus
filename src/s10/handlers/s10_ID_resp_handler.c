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
s10_ID_resp_handler(MsgBuffer* message, GtpV2MessageHeader* hdr, uint32_t tmme_ip)
{

	struct id_resp_Q_msg idr_info = {0};
	/*****Message structure***
	*/

	/*Check whether has teid flag is set. Also check whether this check is needed for CSR.*/
	idr_info.s10_mme_cp_teid = hdr->teid;
	idr_info.header.msg_type = identification_response;

    delete_gtp_transaction(hdr->sequenceNumber);
    IdentificationResponseMsgData msgData;
	memset(&msgData, 0, sizeof(IdentificationResponseMsgData));

	bool rc = GtpV2Stack_decodeMessage(gtpStack_gp, hdr, message, &msgData);
	if(rc == false)
	{
			log_msg(LOG_ERROR, "s10_ID_resp_handler: "
								"Failed to decode Create Session Response Msg %d",
								hdr->teid);
			return E_PARSING_FAILED;
	}

	idr_info.cause = msgData.cause.causeValue;
	//idr_info.IMSI = msgData.imsi.imsiValue ;
	//trace information
//	idr_info.trace_information = msgData.traceInformation;
	idr_info.integer_number = msgData.ueUsageType.integerNumberValue ;


   	idr_info.header.destInstAddr = htonl(mmeAppInstanceNum_c);
	idr_info.header.srcInstAddr = htonl(s10AppInstanceNum_c);

	/*Send ID response msg*/
	log_msg(LOG_INFO, "Send Id resp to mme-app stage6.");
	send_tipc_message(g_resp_fd, mmeAppInstanceNum_c, (char *)&idr_info, sizeof(struct id_resp_Q_msg));

	return SUCCESS;
}
