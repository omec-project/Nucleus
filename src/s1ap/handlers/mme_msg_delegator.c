/*
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
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "options.h"
#include "ipc_api.h"
#include "main.h"
#include "s1ap.h"
#include "s1ap_config.h"
#include "sctp_conn.h"
#include "s1ap_structs.h"
#include "s1ap_msg_codes.h"
#include "s1ap_ie.h"
#include "err_codes.h"
#include "msgType.h"

void
handle_mmeapp_message(void * data)
{
	log_msg(LOG_INFO, "handle mme-app message ");
	
	char *msg = ((char *) data) + (sizeof(uint32_t)*2);

	msg_type_t* msg_type = (msg_type_t*)(msg);

	switch(*msg_type)		
	{	 
    case id_request:
        idreq_handler(msg);
        break;
	case auth_request:
		authreq_handler(msg);
		break;
	case sec_mode_command:
		secreq_handler(msg);
		break;
	case esm_info_request:
		esmreq_handler(msg);
		break;
	case init_ctxt_request:
		icsreq_handler(msg);
		break;
	case detach_accept:
		detach_accept_handler(msg);
		break;
	case s1_release_command:
		s1_release_command_handler(msg);
		break;
	case ni_detach_request:
		ni_detach_request_handler(msg);
		break;
	case paging_request:
		paging_handler(msg);
		break;
	case ics_req_paging:
		ics_req_paging_handler(msg);
		break;
	case tau_response:
		tau_response_handler(msg);
		break;
	case attach_reject:
		s1ap_reject_handler(msg);
		break;
	case service_reject:
	    s1ap_reject_handler(msg);
	    break;
	case emm_info_request:
	    emm_info_req_handler(msg);
	    break;
	case s1_reset:
	    gen_reset_request_handler(msg);
	    break;
	case handover_request:
		handover_request_handler(msg);
		break;
	case handover_command:
		handover_command_handler(msg);
		break;
	case mme_status_transfer:
		mme_status_transfer_handler(msg);
		break;
	case handover_preparation_failure:
		handover_preparation_failure_handler(msg);
		break;
	case handover_cancel_ack:
		handover_cancel_ack_handler(msg);
		break;
	default:
		log_msg(LOG_ERROR,"Unhandled mme-app message\n");
		break;
	}
	free(data);
}
