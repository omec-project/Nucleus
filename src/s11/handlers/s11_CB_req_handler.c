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

uint64_t conv_uint8_arr_to_uint64(const uint8_t* bit_rate_array)
{
    uint64_t bit_rate_kbps = (*(bit_rate_array) << 32) | (*(bit_rate_array+1) << 24) | (*(bit_rate_array+2) << 16) | 
	    			(*(bit_rate_array+3) << 8) | (*(bit_rate_array+4));
    return bit_rate_kbps;
}


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
s11_CB_req_handler(MsgBuffer* message, GtpV2MessageHeader* hdr, uint32_t sgw_ip)
{

	struct gtp_incoming_msg_data_t cbr_info;

	cbr_info.ue_idx = hdr->teid;
	cbr_info.msg_type = create_bearer_request;

	CreateBearerRequestMsgData msgData;
	memset(&msgData, 0, sizeof(CreateBearerRequestMsgData));

	bool rc = GtpV2Stack_decodeMessage(gtpStack_gp, hdr, message, &msgData);
	if(rc == false)
	{
			log_msg(LOG_ERROR, "s11_CB_req_handler: "
					"Failed to decode Create Bearer Request Msg %d\n",
					hdr->teid);
			return E_PARSING_FAILED;
	}

	cbr_info.msg_data.cb_req_Q_m.linked_eps_bearer_id = msgData.linkedEpsBearerId.epsBearerId;

	cbr_info.msg_data.cb_req_Q_m.pco.pco_length = 0;
	if(msgData.protocolConfigurationOptionsIePresent)
	{
	    cbr_info.msg_data.cb_req_Q_m.pco.pco_length = msgData.protocolConfigurationOptions.pcoValue.count;
	    memcpy(cbr_info.msg_data.cb_req_Q_m.pco.pco_options, &msgData.protocolConfigurationOptions.pcoValue.values[0],
			    msgData.protocolConfigurationOptions.pcoValue.count);
	}

	/*Hard-coding the bearer count as 1, since we support only one bearer Ctxt in the Q msg struct currently.
	  Count value can be replaced with "msgData.bearerContextsCount" while supporting multiple bearers */
	cbr_info.msg_data.cb_req_Q_m.bearerCtxList.bearers_count = 1;
	
	for(int i =0; i < cbr_info.msg_data.cb_req_Q_m.bearerCtxList.bearers_count; i++)
	{
		cbr_info.msg_data.cb_req_Q_m.bearerCtxList.bearerCtxt[i].eps_bearer_id =
			    msgData.bearerContexts[i].epsBearerId.epsBearerId;

		cbr_info.msg_data.cb_req_Q_m.bearerCtxList.bearerCtxt[i].s1u_sgw_teid.header.iface_type =
			    msgData.bearerContexts[i].s1USgwFTeid.interfaceType;
		cbr_info.msg_data.cb_req_Q_m.bearerCtxList.bearerCtxt[i].s1u_sgw_teid.header.teid_gre =
			    msgData.bearerContexts[i].s1USgwFTeid.teidGreKey;
		cbr_info.msg_data.cb_req_Q_m.bearerCtxList.bearerCtxt[i].s1u_sgw_teid.header.v4 =
			    msgData.bearerContexts[i].s1USgwFTeid.ipv4present;
		cbr_info.msg_data.cb_req_Q_m.bearerCtxList.bearerCtxt[i].s1u_sgw_teid.ip.ipv4.s_addr =
			    msgData.bearerContexts[i].s1USgwFTeid.ipV4Address.ipValue;

		cbr_info.msg_data.cb_req_Q_m.bearerCtxList.bearerCtxt[i].tft.len =
			    msgData.bearerContexts[i].tft.tft.count;
		memcpy(cbr_info.msg_data.cb_req_Q_m.bearerCtxList.bearerCtxt[i].tft.data,
				    msgData.bearerContexts[i].tft.tft.values, msgData.bearerContexts[i].tft.tft.count);

		build_bearer_qos(&msgData.bearerContexts[i].bearerLevelQos,
				    &cbr_info.msg_data.cb_req_Q_m.bearerCtxList.bearerCtxt[i].bearer_qos);
	}

	cbr_info.destInstAddr = htonl(mmeAppInstanceNum_c);
	cbr_info.srcInstAddr = htonl(s11AppInstanceNum_c);

	/*Send CB request msg*/
	send_tipc_message(g_resp_fd, mmeAppInstanceNum_c, (char *)&cbr_info, GTP_READ_MSG_BUF_SIZE);
	log_msg(LOG_INFO, "Send CB req to mme-app.\n");
	
	return SUCCESS;
}
