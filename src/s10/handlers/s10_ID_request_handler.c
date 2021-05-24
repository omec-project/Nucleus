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
s10_ID_request_handler(MsgBuffer* message, GtpV2MessageHeader* hdr, uint32_t tmme_ip)
{

	struct ID_req_Q_msg idr_info = {0};
	/*****Message structure***
	*/

	/*Check whether has teid flag is set. Also check whether this check is needed for CSR.*/
	idr_info.s10_mme_cp_teid = hdr->teid;
	idr_info.header.msg_type = identification_request;

    delete_gtp_transaction(hdr->sequenceNumber);
    IdentificationRequestMsgData msgData;
	memset(&msgData, 0, sizeof(IdentificationRequestMsgData));

	bool rc = GtpV2Stack_decodeMessage(gtpStack_gp, hdr, message, &msgData);
	if(rc == false)
	{
			log_msg(LOG_ERROR, "s10_ID_req_handler: "
								"Failed to decode Identification request Msg %d",
								hdr->teid);
			return E_PARSING_FAILED;
	}


	idr_info.guti.plmn_id.idx[0] = msgData.guti.mccDigit1;
	idr_info.guti.plmn_id.idx[0] = msgData.guti.mccDigit2;
	idr_info.guti.plmn_id.idx[1] = msgData.guti.mccDigit3;
	idr_info.guti.plmn_id.idx[2] = msgData.guti.mccDigit1;
	idr_info.guti.plmn_id.idx[2] = msgData.guti.mccDigit2;
	idr_info.guti.plmn_id.idx[1] = msgData.guti.mccDigit3;

	idr_info.guti.mme_grp_id = msgData.guti.mmeGroupID ;
	idr_info.guti.mme_code = msgData.guti.mmeCode;
	idr_info.guti.m_TMSI = msgData.guti.mTimsiType;



	/*Send ID response msg*/
	log_msg(LOG_INFO, "Send Id resp to mme-app stage6.");
	send_tipc_message(g_resp_fd, mmeAppInstanceNum_c, (char *)&idr_info, sizeof(struct ID_req_Q_msg));

	return SUCCESS;
}
