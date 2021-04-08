/*
 * Copyright (c) 2019, Infosys Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include "common_proc_info.h"
#include "log.h"
#include "err_codes.h"
#include "ipc_api.h"
#include "s1ap.h"
#include "s1ap_config.h"
#include "main.h"
#include "msgType.h"


/*static int
get_uectxtrelcmd_protoie_value(struct proto_IE *value, struct s1relcmd_info *g_uectxtrelcmd)
{
	//uint8_t ieCnt = 0;

	value->no_of_IEs = UE_CTX_RELEASE_NO_OF_IES;

	value->data = (proto_IEs *) malloc(UE_CTX_RELEASE_NO_OF_IES *
			sizeof(proto_IEs));

	value->data[0].mme_ue_s1ap_id = g_uectxtrelcmd->ue_idx;
	
	value->data[1].enb_ue_s1ap_id = g_uectxtrelcmd->enb_s1ap_ue_id;
	
	log_msg(LOG_INFO, "mme_ue_s1ap_id %d and enb_ue_s1ap_id %d",
					g_uectxtrelcmd->ue_idx,g_uectxtrelcmd->enb_s1ap_ue_id);
	return SUCCESS;
}*/

/**
* Stage specific message processing.
*/
static int
ics_req_paging_processing(struct ics_req_paging_Q_msg *g_icsreq)
{
	log_msg(LOG_DEBUG,"Process Init Ctxt Req for service Request.");
	uint32_t length = 0;
	uint8_t *buffer = NULL;

	struct s1ap_common_req_Q_msg req = {0};

	log_msg(LOG_DEBUG,"Inside ics_req_paging processing");	

	req.IE_type = S1AP_INIT_CTXT_SETUP_REQ;
	req.ue_idx = g_icsreq->ue_idx;
	req.enb_fd = g_icsreq->enb_fd;
	req.enb_s1ap_ue_id = g_icsreq->enb_s1ap_ue_id;
	req.mme_s1ap_ue_id = g_icsreq->ue_idx;
	req.ueag_max_dl_bitrate = g_icsreq->ueag_max_dl_bitrate;
	req.ueag_max_ul_bitrate = g_icsreq->ueag_max_ul_bitrate;
	req.erab_su_list.count = g_icsreq->erab_su_list.count;
	for(int i = 0; i < g_icsreq->erab_su_list.count; i++)
	{
		req.erab_su_list.erab_su_item[i].e_RAB_ID =
				g_icsreq->erab_su_list.erab_su_item[i].e_RAB_ID;
		req.erab_su_list.erab_su_item[i].e_RAB_QoS_Params.qci =
				g_icsreq->erab_su_list.erab_su_item[i].e_RAB_QoS_Params.qci;
		req.erab_su_list.erab_su_item[i].e_RAB_QoS_Params.arPrio.prioLevel =
				g_icsreq->erab_su_list.erab_su_item[i].e_RAB_QoS_Params.arPrio.prioLevel;
		req.erab_su_list.erab_su_item[i].e_RAB_QoS_Params.arPrio.preEmptionVulnebility =
				g_icsreq->erab_su_list.erab_su_item[i].e_RAB_QoS_Params.arPrio.preEmptionVulnebility;
		req.erab_su_list.erab_su_item[i].e_RAB_QoS_Params.arPrio.preEmptionCapab =
				g_icsreq->erab_su_list.erab_su_item[i].e_RAB_QoS_Params.arPrio.preEmptionCapab;
		req.erab_su_list.erab_su_item[i].gtp_teid =
				g_icsreq->erab_su_list.erab_su_item[i].gtp_teid;
		req.erab_su_list.erab_su_item[i].transportLayerAddress =
				g_icsreq->erab_su_list.erab_su_item[i].transportLayerAddress;
	}
	memcpy(&(req.sec_key), &(g_icsreq->sec_key), KENB_SIZE);	
	
	log_msg(LOG_DEBUG,"Before s1ap_encoder");

	int ret = s1ap_mme_encode_initiating(&req, &buffer, &length);
	log_msg(LOG_DEBUG,"Invoked s1ap_encoder");
	if(ret == -1)
	{
		log_msg(LOG_ERROR, "Encoding ics_req_paging failed.");
		return E_FAIL;
	}

	send_sctp_msg(g_icsreq->enb_fd, buffer, length, 1);
	if(buffer) {
		free(buffer);
	}
	log_msg(LOG_INFO, "----ICS Req for paging sent to UE.---");
	return SUCCESS;
	
}


/**
* Thread function for stage.
*/
void*
ics_req_paging_handler(void *data)
{

	log_msg(LOG_INFO,"ICS Req for paging handler ready.");
	
	ics_req_paging_processing((struct ics_req_paging_Q_msg *)data);

	return NULL;
}


