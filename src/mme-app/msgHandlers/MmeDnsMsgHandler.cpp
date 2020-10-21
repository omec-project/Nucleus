/*
 * Copyright (c) 2019, Infosys Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <msgHandlers/MmeDnsMsgHandler.h>
#include "epc/epcdns.h"
#include "epc/epcdns.h"

#include <contextManager/subsDataGroupManager.h>
#include <event.h>
#include <ipcTypes.h>
#include <log.h>
#include <mmeSmDefs.h>
#include <eventMessage.h>

using namespace SM;
using namespace mme;
using namespace cmn;


MmeDnsMsgHandler::~MmeDnsMsgHandler() {

}
MmeDnsMsgHandler::MmeDnsMsgHandler() {

}

MmeDnsMsgHandler* MmeDnsMsgHandler::Instance()
{
	static MmeDnsMsgHandler msgHandler;
	return &msgHandler;
}

void MmeDnsMsgHandler::handleDnsResponse(uint64_t sgw_ip, uint32_t ueIdx)
{
	log_msg(LOG_INFO, "Inside handleDnsResponse\n");
	SM::ControlBlock* controlBlk_p = SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
	if(controlBlk_p == NULL)
	{
		log_msg(LOG_ERROR, "handleDnsResponse: "
							"Failed to find UE context using idx %d\n",
							ueIdx);
		return;
	}
	DnsEventMessage *dnsEventMsg = new DnsEventMessage(sgw_ip);
	SM::Event evt(DNS_RESPONSE, dnsEventMsg);
	controlBlk_p->addEventToProcQ(evt);

}

