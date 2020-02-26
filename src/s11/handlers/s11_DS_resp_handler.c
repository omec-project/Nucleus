/*
 * Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
 * Copyright (c) 2017 Intel Corporation
 * Copyright (c) 2019, Infosys Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "err_codes.h"
#include "options.h"
#include "ipc_api.h"
#include "message_queues.h"
#include "s11.h"
#include "s11_config.h"
#include "msgType.h"

#include "../../gtpV2Codec/gtpV2StackWrappers.h"
/*Globals and externs*/
extern int g_resp_fd;

/*End : globals and externs*/

int
s11_DS_resp_handler(MsgBuffer* message, GtpV2MessageHeader* hdr)
{


	struct gtp_incoming_msg_data_t dsr_info;
	dsr_info.msg_type = delete_session_response;

	/*****Message structure****/
	log_msg(LOG_INFO, "Parse S11 DS resp message\n");

	//TODO : check cause for the result verification

	/*Check whether has teid flag is set.
	 * Also check whether this check is needed for DSR.
	 * */
	dsr_info.ue_idx = hdr->teid;	

	dsr_info.destInstAddr = htonl(mmeAppInstanceNum_c);
	dsr_info.srcInstAddr = htonl(s11AppInstanceNum_c);

	/*Send CS response msg*/
	send_tipc_message(g_resp_fd, mmeAppInstanceNum_c, (char *)&dsr_info,
			GTP_READ_MSG_BUF_SIZE);
	log_msg(LOG_INFO, "Send DS resp to mme-app stage8.\n");

	return SUCCESS;
}
