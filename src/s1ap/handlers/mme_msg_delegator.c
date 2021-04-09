/*
 * Copyright (c) 2019, Infosys Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
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
	case tau_reject:
		s1ap_reject_handler(msg);
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
	case erab_mod_confirmation:
	    erab_mod_confirm_handler(msg);
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
	case erab_setup_request:
		erab_setup_req_handler(msg);
		break;
	case erab_release_command:
		erab_release_command_handler(msg);
		break;
	default:
		log_msg(LOG_ERROR,"Unhandled mme-app message");
		break;
	}
	free(data);
}
