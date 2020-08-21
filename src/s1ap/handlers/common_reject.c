/*
* Copyright 2019-present Open Networking Foundation
*
* SPDX-License-Identifier: Apache-2.0
*
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#include "log.h"
#include "err_codes.h"
#include "s1ap.h"
#include "ipc_api.h"
#include "main.h"
#include "sctp_conn.h"
#include "msgType.h"



/**
* message processing for Attach Reject
*/
static int
process_attach_rej(struct commonRej_info *g_mmeS1apInfo)
{
    log_msg(LOG_DEBUG,"Process Attach Reject.");
	uint32_t length = 0;
    uint8_t *buffer = NULL;
    struct s1ap_common_req_Q_msg message_p={0};

    message_p.IE_type = S1AP_ATTACH_REJ; 
    message_p.enb_s1ap_ue_id = g_mmeS1apInfo->s1ap_enb_ue_id;
    message_p.mme_s1ap_ue_id = g_mmeS1apInfo->ue_idx;
    message_p.emm_cause = g_mmeS1apInfo->cause;

    int ret = s1ap_mme_encode_initiating(&message_p, &buffer, &length);
    if(ret == -1)
    {
        log_msg(LOG_ERROR, "Encoding Attach Reject failed.\n");
        return E_FAIL;
    }

    send_sctp_msg(g_mmeS1apInfo->enb_fd, buffer, length, 1);
	log_msg(LOG_INFO, "buffer size is %d\n", length);
    if(buffer)
    {
        free(buffer);
        buffer = NULL;
        length = 0;
    }

	log_msg(LOG_INFO, "\n-----Message handlingcompleted.---\n");

	return SUCCESS;
}

/**
* message processing for Service Reject
*/
static int
process_service_rej(struct commonRej_info *g_mmeS1apInfo)
{
    log_msg(LOG_DEBUG,"Process Service Reject.");
	uint32_t length = 0;
    uint8_t *buffer = NULL;
    struct s1ap_common_req_Q_msg message_p={0};

    message_p.IE_type = S1AP_SERVICE_REJ; 
    message_p.enb_s1ap_ue_id = g_mmeS1apInfo->s1ap_enb_ue_id;
    message_p.mme_s1ap_ue_id = g_mmeS1apInfo->ue_idx;
    message_p.emm_cause = g_mmeS1apInfo->cause;

    int ret = s1ap_mme_encode_initiating(&message_p, &buffer, &length);
    if(ret == -1)
    {
        log_msg(LOG_ERROR, "Encoding Service Reject failed.\n");
        return E_FAIL;
    }

    send_sctp_msg(g_mmeS1apInfo->enb_fd, buffer, length, 1);
	log_msg(LOG_INFO, "buffer size is %d\n", length);
    if(buffer)
    {
        free(buffer);
        buffer = NULL;
        length = 0;
    }

	log_msg(LOG_INFO, "\n-----Message handlingcompleted.---\n");

	return SUCCESS;
}

/**
* message processing for TAU Reject
*/
static int
process_tau_rej(struct commonRej_info *g_mmeS1apInfo)
{
    log_msg(LOG_DEBUG,"Process TAU Reject.");
    uint32_t length = 0;
    uint8_t *buffer = NULL;
    struct s1ap_common_req_Q_msg message_p={0};

    message_p.IE_type = S1AP_TAU_REJ;
    message_p.enb_s1ap_ue_id = g_mmeS1apInfo->s1ap_enb_ue_id;
    message_p.mme_s1ap_ue_id = g_mmeS1apInfo->ue_idx;
    message_p.emm_cause = g_mmeS1apInfo->cause;

    int ret = s1ap_mme_encode_initiating(&message_p, &buffer, &length);
    if(ret == -1)
    {
        log_msg(LOG_ERROR, "Encoding TAU Reject failed.\n");
        return E_FAIL;
    }

    send_sctp_msg(g_mmeS1apInfo->enb_fd, buffer, length, 1);
        log_msg(LOG_INFO, "buffer size is %d\n", length);
    if(buffer)
    {
        free(buffer);
        buffer = NULL;
        length = 0;
    }

        log_msg(LOG_INFO, "\n-----Message handlingcompleted.---\n");

        return SUCCESS;
}

void*
s1ap_reject_handler(void *data)
{
	
	log_msg(LOG_INFO, "NAS Reject Handler.\n");

	struct commonRej_info *nasReject = (struct commonRej_info *)data;

	switch (nasReject->msg_type)
	{
	case attach_reject:
	{
	    log_msg(LOG_INFO, "Send Attach Reject.\n");
        process_attach_rej(nasReject);
	    break;
	}
	case service_reject:
	{
	    log_msg(LOG_INFO, "Send Service Reject.\n");
        process_service_rej(nasReject);
	    break;
	}
	case tau_reject:
	{
	    log_msg(LOG_INFO, "Send TAU Reject.\n");
	    process_tau_rej(nasReject);
	    break;
	}
	default:
	    break;

	}
	return NULL;
}

