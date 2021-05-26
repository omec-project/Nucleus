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
//#include "s11_options.h"
#include "ipc_api.h"
//#include "s11.h"
#include "s10.h"
//#include "s11_config.h"
#include "s10_config.h"
#include "msgType.h"

#include <gtpV2StackWrappers.h>
#include "gtp_cpp_wrapper.h"

/*Globals and externs*/
extern int g_resp_fd;
extern struct GtpV2Stack* gtpStack_gp;
/*End : globals and externs*/


int
s10_FR_relocation_complete_notification(MsgBuffer* message, GtpV2MessageHeader* hdr, uint32_t new_mme_ip)
{

	//struct csr_Q_msg csr_info = {0};
	//struct FR_RES_Q_msg frr_info = {0};
	/*****Message structure***
	*/

	struct forward_relocation_complete_notification_BQ_msg fr_compnotif_info = {0};

	/*Check whether has teid flag is set. Also check whether this check is needed for FRRES.*/
	fr_compnotif_info.s10_mme_cp_teid = hdr->teid;
	fr_compnotif_info.header.msg_type = forward_relocation_complete_notification;

//	===========================

    delete_gtp_transaction(hdr->sequenceNumber);
	//CreateSessionResponseMsgData msgData;
    ForwardRelocationCompleteNotificationMsgData msgData;

	memset(&msgData, 0, sizeof(ForwardRelocationCompleteNotificationMsgData));

	bool rc = GtpV2Stack_decodeMessage(gtpStack_gp, hdr, message, &msgData);
	if(rc == false)
	{
			log_msg(LOG_ERROR, "s10_FR_complete notification_handler: "
								"Failed to decode Forward relocation Response Msg %d",
								hdr->teid);
			return E_PARSING_FAILED;
	}


	//fr_compnotif_info.cause.cause = (unsigned char)msgData.cause.causeValue;
	

	fr_compnotif_info.header.destInstAddr = htonl(mmeAppInstanceNum_c);
	fr_compnotif_info.header.srcInstAddr = htonl(s10AppInstanceNum_c);

	/*Send FR_response msg*/
	log_msg(LOG_INFO, "Send FR resp to mme-app stage6.");
	send_tipc_message(g_resp_fd, mmeAppInstanceNum_c, (char *)&fr_compnotif_info, sizeof(struct forward_relocation_complete_notification_BQ_msg));

	return SUCCESS;
}
