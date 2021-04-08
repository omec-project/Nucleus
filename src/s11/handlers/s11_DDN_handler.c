/*
 * Copyright (c) 2019, Infosys Ltd.
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


int
s11_DDN_handler(MsgBuffer* message, GtpV2MessageHeader* hdr, uint32_t sgw_ip)
{
	struct ddn_Q_msg ddn_info;
	ddn_info.header.msg_type = downlink_data_notification;
	ddn_info.s11_mme_cp_teid = hdr->teid;
	ddn_info.seq_no = hdr->sequenceNumber;
	ddn_info.sgw_ip = sgw_ip;


	DownlinkDataNotificationMsgData msgData;
	memset(&msgData, 0, sizeof(DownlinkDataNotificationMsgData));

	bool rc = GtpV2Stack_decodeMessage(gtpStack_gp, hdr, message, &msgData);
	if(rc == false)
	{
			
			log_msg(LOG_ERROR, "s11_Ddn_handler: "
					   "Failed to decode ddn Msg %d",
								hdr->teid);
			return E_PARSING_FAILED;
	}
	/*****Message structure****/
	log_msg(LOG_INFO, "Parse S11 Ddn message");

	//TODO : check cause for the result verification
	if (msgData.allocationRetentionPriorityIePresent) {
		ddn_info.arp.prioLevel = msgData.allocationRetentionPriority.pl;
		ddn_info.arp.preEmptionCapab = msgData.allocationRetentionPriority.pci;
		ddn_info.arp.preEmptionVulnebility = msgData.allocationRetentionPriority.pvi;
	}
	if (msgData.causeIePresent)
		ddn_info.cause = msgData.cause.causeValue;
	if (msgData.epsBearerIdIePresent)
		ddn_info.eps_bearer_id = msgData.epsBearerId.epsBearerId;

	ddn_info.header.destInstAddr = htonl(mmeAppInstanceNum_c);
	ddn_info.header.srcInstAddr = htonl(s11AppInstanceNum_c);

	/*Send DDN msg*/
	log_msg(LOG_INFO, "Send ddn to mme-app");
	send_tipc_message(g_resp_fd, mmeAppInstanceNum_c, (char *)&ddn_info, sizeof(struct ddn_Q_msg));

	return SUCCESS;
}
