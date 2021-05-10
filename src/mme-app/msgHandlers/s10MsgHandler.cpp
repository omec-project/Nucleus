/*
 * Copyright (c) 2019, Infosys Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <msgHandlers/s10MsgHandler.h>

#include <contextManager/subsDataGroupManager.h>
#include <event.h>
#include <eventMessage.h>
#include <ipcTypes.h>
#include <log.h>
#include <mmeSmDefs.h>
#include <utils/mmeCommonUtils.h>
#include "mmeStatsPromClient.h"

using namespace SM;
using namespace mme;
using namespace cmn;

S10MsgHandler::~S10MsgHandler() {

}

S10MsgHandler::S10MsgHandler()
{

}

S10MsgHandler* S10MsgHandler::Instance()
{
	static s10MsgHandler msgHandler;
	return &msgHandler;
}

void S10MsgHandler::handleS10Message_v(IpcEMsgUnqPtr eMsg)
{
    if (eMsg.get() == NULL)
        return;

    utils::MsgBuffer *msgBuf = eMsg->getMsgBuffer();
    if (msgBuf == NULL)
    {
        log_msg(LOG_INFO, "s10 Message Buffer is empty ");
        return;
    }
    if (msgBuf->getLength() < sizeof(gtp_incoming_msg_data_t))
    {
        log_msg(LOG_INFO, "Not enough bytes in s10 message ");
        return;
    }

    const gtp_incoming_msg_data_t *msgData_p =
            (gtp_incoming_msg_data_t*) (msgBuf->getDataPointer());

	switch (msgData_p->msg_type)
	{
		case msg_type_t::identification_request:
		{
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_S10_IDENTIFICATION_REQUEST,);
			const struct ID_req_Q_msg* idr_info= (const struct ID_req_Q_msg*) (msgBuf->getDataPointer());
			handleIdentificationRequestMsg_v(std::move(eMsg), idr_info->s10_mme_cp_teid);
		}
		break;

		case msg_type_t::identification_response:
		{
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_S10_IDENTIFICATION_RESPONSE);
			const struct id_resp_Q_msg* id_resp_info= (const struct id_resp_Q_msg*) (msgBuf->getDataPointer());
			handleIdentificationResponseMsg_v(std::move(eMsg), id_resp_info->s10_mme_cp_teid);
		}
		break;

		default:
			log_msg(LOG_INFO, "Unhandled s10 Message %d ", msgData_p->msg_type);
	}

}

void S10MsgHandler::handleIdentificationRequestMsg_v(IpcEMsgUnqPtr eMsg, uint32_t ueIdx)
{
	log_msg(LOG_INFO, "handleIdentificationRequestMsg_v");

	SM::ControlBlock* controlBlk_p = SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
	if(controlBlk_p == NULL)
	{
		log_msg(LOG_ERROR, "handleIdentificationRequestMsg_v: "
							"Failed to find UE context using idx %d",
							ueIdx);
		return;
	}

	// Fire Identification req event, insert cb to procedure queue
	SM::Event evt(IDENTIFICATION_REQ_FROM_UE, cmn::IpcEMsgShPtr(std::move(eMsg)));
	controlBlk_p->addEventToProcQ(evt);
}

void S10MsgHandler::handleIdentificationResponseMsg_v(IpcEMsgUnqPtr eMsg, uint32_t ueIdx)
{
	log_msg(LOG_INFO, "handleIdentificationResponseMsg_v");

	SM::ControlBlock* controlBlk_p = SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
	if(controlBlk_p == NULL)
	{
		log_msg(LOG_ERROR, "handleIdentificationResponseMsg_v: "
							"Failed to find UE context using idx %d",
							ueIdx);
		return;
	}

	// Fire Identification resp from old MME, insert cb to procedure queue
	SM::Event evt(IDENTIFICATION_RESPONSE_FROM_MME, cmn::IpcEMsgShPtr(std::move(eMsg)));
	controlBlk_p->addEventToProcQ(evt);
}
