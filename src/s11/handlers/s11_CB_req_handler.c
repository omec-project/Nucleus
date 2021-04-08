/*
 * Copyright (c) 2020, Infosys Ltd.
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
#include "s11_config.h"
#include "msgType.h"

#include <gtpV2StackWrappers.h>
/*Globals and externs*/
extern int g_resp_fd;
extern struct GtpV2Stack* gtpStack_gp;
/*End : globals and externs*/

void build_bearer_qos(BearerQosIeData* s11_bearer_qos, bearer_qos_t* mme_bearer_qos)
{
	mme_bearer_qos->arp.prioLevel = s11_bearer_qos->pl;
	mme_bearer_qos->arp.preEmptionCapab = s11_bearer_qos->pci;
	mme_bearer_qos->arp.preEmptionVulnebility = s11_bearer_qos->pvi;

	mme_bearer_qos->qci = s11_bearer_qos->qci;
	
	mme_bearer_qos->mbr_ul = conv_uint8_arr_to_uint64(s11_bearer_qos->maxBitRateUl.values);
	mme_bearer_qos->mbr_dl = conv_uint8_arr_to_uint64(s11_bearer_qos->maxBitRateDl.values);
	mme_bearer_qos->gbr_ul = conv_uint8_arr_to_uint64(s11_bearer_qos->guraranteedBitRateUl.values);
	mme_bearer_qos->gbr_dl = conv_uint8_arr_to_uint64(s11_bearer_qos->guaranteedBitRateDl.values);
}

int
s11_CB_req_handler(MsgBuffer* message, GtpV2MessageHeader* hdr, uint32_t sgw_ip, uint16_t src_port)
{

	struct cb_req_Q_msg cbr_info = {0};

	cbr_info.s11_mme_cp_teid = hdr->teid;
	cbr_info.header.msg_type = create_bearer_request;
	cbr_info.seq_no = hdr->sequenceNumber;
	cbr_info.sgw_ip = sgw_ip;
	cbr_info.source_port = src_port;

	CreateBearerRequestMsgData msgData;
	memset(&msgData, 0, sizeof(CreateBearerRequestMsgData));

	bool rc = GtpV2Stack_decodeMessage(gtpStack_gp, hdr, message, &msgData);
	if(rc == false)
	{
			log_msg(LOG_ERROR, "s11_CB_req_handler: "
					"Failed to decode Create Bearer Request Msg %d",
					hdr->teid);
			return E_PARSING_FAILED;
	}

	cbr_info.linked_eps_bearer_id = msgData.linkedEpsBearerId.epsBearerId;

	if(msgData.protocolConfigurationOptionsIePresent)
	{
	    cbr_info.pco.pco_length = msgData.protocolConfigurationOptions.pcoValue.count;
	    memcpy(cbr_info.pco.pco_options, &msgData.protocolConfigurationOptions.pcoValue.values[0],
			    msgData.protocolConfigurationOptions.pcoValue.count);
	}

	/*Hard-coding the bearer count as 1, since we support only one bearer Ctxt in the Q msg struct currently.
	  Count value can be replaced with "msgData.bearerContextsCount" while supporting multiple bearers */
	if(msgData.bearerContextsCount > 0)
		cbr_info.bearer_ctx_list.bearers_count = 1;
	
	for(int i =0; i < cbr_info.bearer_ctx_list.bearers_count; i++)
	{
		cbr_info.bearer_ctx_list.bearer_ctxt[i].eps_bearer_id =
			    msgData.bearerContexts[i].epsBearerId.epsBearerId;

		cbr_info.bearer_ctx_list.bearer_ctxt[i].s1u_sgw_teid.header.iface_type =
			    msgData.bearerContexts[i].s1USgwFTeid.interfaceType;
		cbr_info.bearer_ctx_list.bearer_ctxt[i].s1u_sgw_teid.header.teid_gre =
			    msgData.bearerContexts[i].s1USgwFTeid.teidGreKey;
		cbr_info.bearer_ctx_list.bearer_ctxt[i].s1u_sgw_teid.header.v4 =
			    msgData.bearerContexts[i].s1USgwFTeid.ipv4present;
		cbr_info.bearer_ctx_list.bearer_ctxt[i].s1u_sgw_teid.ip.ipv4.s_addr =
			    msgData.bearerContexts[i].s1USgwFTeid.ipV4Address.ipValue;

		cbr_info.bearer_ctx_list.bearer_ctxt[i].tft.len =
			    msgData.bearerContexts[i].tft.tft.count;
		memcpy(cbr_info.bearer_ctx_list.bearer_ctxt[i].tft.data,
				    msgData.bearerContexts[i].tft.tft.values, msgData.bearerContexts[i].tft.tft.count);

		build_bearer_qos(&msgData.bearerContexts[i].bearerLevelQos,
				    &cbr_info.bearer_ctx_list.bearer_ctxt[i].bearer_qos);
	}

	cbr_info.header.destInstAddr = htonl(mmeAppInstanceNum_c);
	cbr_info.header.srcInstAddr = htonl(s11AppInstanceNum_c);
	/*Send CB request msg*/
	log_msg(LOG_INFO, "Send CB req to mme-app.");
	send_tipc_message(g_resp_fd, mmeAppInstanceNum_c, (char *)&cbr_info, sizeof(struct cb_req_Q_msg));
	return SUCCESS;
}
