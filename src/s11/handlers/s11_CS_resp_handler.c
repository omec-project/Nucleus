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
#include "s11_config.h"
#include "msgType.h"

#include <gtpV2StackWrappers.h>
#include "gtp_cpp_wrapper.h"

/*Globals and externs*/
extern int g_resp_fd;
extern struct GtpV2Stack* gtpStack_gp;
/*End : globals and externs*/


int
s11_CS_resp_handler(MsgBuffer* message, GtpV2MessageHeader* hdr, uint32_t sgw_ip)
{

	struct csr_Q_msg csr_info = {0};
	/*****Message structure***
	*/

	/*Check whether has teid flag is set. Also check whether this check is needed for CSR.*/
	csr_info.s11_mme_cp_teid = hdr->teid;
	csr_info.header.msg_type = create_session_response;

    delete_gtp_transaction(hdr->sequenceNumber);
	CreateSessionResponseMsgData msgData;
	memset(&msgData, 0, sizeof(CreateSessionResponseMsgData));

	bool rc = GtpV2Stack_decodeMessage(gtpStack_gp, hdr, message, &msgData);
	if(rc == false)
	{
			log_msg(LOG_ERROR, "s11_CS_resp_handler: "
								"Failed to decode Create Session Response Msg %d",
								hdr->teid);
			return E_PARSING_FAILED;
	}
    csr_info.status = msgData.cause.causeValue;
	csr_info.s11_sgw_fteid.header.iface_type = 11;
	csr_info.s11_sgw_fteid.header.teid_gre = msgData.senderFTeidForControlPlane.teidGreKey;
	csr_info.s11_sgw_fteid.header.v4 = 1;
	csr_info.s11_sgw_fteid.ip.ipv4.s_addr = msgData.senderFTeidForControlPlane.ipV4Address.ipValue;

	csr_info.s5s8_pgwc_fteid.header.iface_type = 7;
	csr_info.s5s8_pgwc_fteid.header.teid_gre = msgData.pgwS5S8S2bFTeid.teidGreKey;
	csr_info.s5s8_pgwc_fteid.header.v4 = 1;
	csr_info.s5s8_pgwc_fteid.ip.ipv4.s_addr = msgData.pgwS5S8S2bFTeid.ipV4Address.ipValue;

	csr_info.s1u_sgw_fteid.header.iface_type = 1;
	csr_info.s1u_sgw_fteid.header.teid_gre = msgData.bearerContextsCreated[0].s1USgwFTeid.teidGreKey;
	csr_info.s1u_sgw_fteid.header.v4 = 1;
	csr_info.s1u_sgw_fteid.ip.ipv4.s_addr = msgData.bearerContextsCreated[0].s1USgwFTeid.ipV4Address.ipValue;

	csr_info.s5s8_pgwu_fteid.header.iface_type = 5;
	csr_info.s5s8_pgwu_fteid.header.teid_gre = msgData.bearerContextsCreated[0].s5S8UPgwFTeid.teidGreKey;
	csr_info.s5s8_pgwu_fteid.header.v4 = 1;
	csr_info.s5s8_pgwu_fteid.ip.ipv4.s_addr = msgData.bearerContextsCreated[0].s5S8UPgwFTeid.ipV4Address.ipValue;

	csr_info.pdn_addr.pdn_type = 1;
	csr_info.pdn_addr.ip_type.ipv4.s_addr = msgData.pdnAddressAllocation.ipV4Address.ipValue;

    csr_info.bearerQos.qci = msgData.bearerContextsCreated[0].bearerLevelQos.qci; 	
    csr_info.bearerQos.arp.preEmptionVulnebility = msgData.bearerContextsCreated[0].bearerLevelQos.pvi;
    csr_info.bearerQos.arp.prioLevel = msgData.bearerContextsCreated[0].bearerLevelQos.pl;
    csr_info.bearerQos.arp.preEmptionCapab = msgData.bearerContextsCreated[0].bearerLevelQos.pci;
    csr_info.bearerQos.mbr_ul = conv_uint8_arr_to_uint64(msgData.bearerContextsCreated[0].bearerLevelQos.maxBitRateUl.values);
    csr_info.bearerQos.mbr_dl = conv_uint8_arr_to_uint64(msgData.bearerContextsCreated[0].bearerLevelQos.maxBitRateDl.values);
    csr_info.bearerQos.gbr_ul = conv_uint8_arr_to_uint64(msgData.bearerContextsCreated[0].bearerLevelQos.guraranteedBitRateUl.values);
    csr_info.bearerQos.gbr_dl = conv_uint8_arr_to_uint64(msgData.bearerContextsCreated[0].bearerLevelQos.guaranteedBitRateDl.values);
	
    csr_info.pco_length = 0;
	if(msgData.protocolConfigurationOptionsIePresent == true)
	{
		csr_info.pco_length = msgData.protocolConfigurationOptions.pcoValue.count;
		memcpy(&csr_info.pco_options[0], &msgData.protocolConfigurationOptions.pcoValue.values[0], msgData.protocolConfigurationOptions.pcoValue.count);
	}
	else
	{
		/* Temporary hardcoding so that UE gets min DNS address.*/
		char pco_options[27] = {0x80, 0x80, 0x21, 0x10, 0x03, 0x00, 0x00,0x10, 0x81, 0x06, 0x08,0x08,0x08, 0x08,0x83,0x06,0x08,0x08,0x08,0x04,0x00,0x0d, 0x04,0x08,0x08,0x08,0x08};
		memcpy(&csr_info.pco_options[0], &pco_options[0], 27);
		csr_info.pco_length = 27;
	}



	csr_info.header.destInstAddr = htonl(mmeAppInstanceNum_c);
	csr_info.header.srcInstAddr = htonl(s11AppInstanceNum_c);

	/*Send CS response msg*/
	log_msg(LOG_INFO, "Send CS resp to mme-app stage6.");
	send_tipc_message(g_resp_fd, mmeAppInstanceNum_c, (char *)&csr_info, sizeof(struct csr_Q_msg));

	return SUCCESS;
}
