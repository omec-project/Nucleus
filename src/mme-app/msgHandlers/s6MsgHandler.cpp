/*
 * Copyright (c) 2019, Infosys Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <msgHandlers/s6MsgHandler.h>

#include <contextManager/subsDataGroupManager.h>
#include <event.h>
#include <ipcTypes.h>
#include <log.h>
#include <mmeSmDefs.h>
#include <eventMessage.h>
#include <utils/mmeCommonUtils.h>
#include "mmeStatsPromClient.h"

using namespace SM;
using namespace mme;
using namespace cmn;


S6MsgHandler::~S6MsgHandler() {

}

S6MsgHandler::S6MsgHandler()
{

}

S6MsgHandler* S6MsgHandler::Instance()
{
	static S6MsgHandler msgHandler;
	return &msgHandler;
}

void S6MsgHandler::handleS6Message_v(IpcEMsgUnqPtr eMsg)
{
    if (eMsg.get() == NULL)
        return;

    utils::MsgBuffer *msgBuf = eMsg->getMsgBuffer();
    if (msgBuf == NULL)
    {
        log_msg(LOG_INFO, "S6 Message Buffer is empty ");
        return;
    }

    if (msgBuf->getLength() < sizeof(s6_incoming_msg_header_t))
    {
        log_msg(LOG_INFO, "Not enough bytes in s6 message ");
        return;
    }

	const s6_incoming_msg_header_t* msgData_p = (s6_incoming_msg_header_t*)(msgBuf->getDataPointer());
	switch (msgData_p->msg_type)
	{
		case msg_type_t::auth_info_answer:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_S6A_AUTHENTICATION_INFORMATION_ANSWER);
			handleAuthInfoAnswer_v(std::move(eMsg), msgData_p->ue_idx);
			break;

		case msg_type_t::update_loc_answer:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_S6A_UPDATE_LOCATION_ANSWER);
			handleUpdateLocationAnswer_v(std::move(eMsg),  msgData_p->ue_idx);
			break;

		case msg_type_t::purge_answser:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_S6A_PURGE_ANSWER);
			handlePurgeAnswer_v(std::move(eMsg),  msgData_p->ue_idx);
			break;
		
		case msg_type_t::cancel_location_request:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_S6A_CANCEL_LOCATION_REQUEST);
			handleCancelLocationRequest_v(std::move(eMsg));
			break;

                case msg_type_t::delete_subscriber_data_request:
                        mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_S6A_DELETE_SUBSCRIBER_DATA_REQUEST);
                        handleDeleteSubscriberRequest_v(std::move(eMsg));
                        break;

		default:
			log_msg(LOG_INFO, "Unhandled S6 Message %d ", msgData_p->msg_type);
	}

}

void S6MsgHandler::handleAuthInfoAnswer_v(cmn::IpcEMsgUnqPtr eMsg, uint32_t ueIdx)
{
	log_msg(LOG_INFO, "Inside handleAuthInfoAnswer_v ");

	SM::ControlBlock* controlBlk_p = SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
	if(controlBlk_p == NULL)
	{
		log_msg(LOG_ERROR, "handleAuthInfoAnswer_v: "
							"Failed to find UE context using idx %d",
							ueIdx);
		return;
	}

	// Fire Auth Info Answer event, insert cb to procedure queue
	SM::Event evt(AIA_FROM_HSS, cmn::IpcEMsgShPtr(std::move(eMsg)));
	controlBlk_p->addEventToProcQ(evt);
}

void S6MsgHandler::handleUpdateLocationAnswer_v(cmn::IpcEMsgUnqPtr eMsg, uint32_t ueIdx)
{
	log_msg(LOG_INFO, "Inside handleUpdateLocationAnswer_v ");

	SM::ControlBlock* controlBlk_p = SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
	if(controlBlk_p == NULL)
	{
		log_msg(LOG_ERROR, "handleUpdateLocationAnswer_v: "
							"Failed to find UE context using idx %d",
							ueIdx);
		return;
	}
	// Fire Update Loc Answer event, insert cb to procedure queue
	SM::Event evt(ULA_FROM_HSS, cmn::IpcEMsgShPtr(std::move(eMsg)));
	controlBlk_p->addEventToProcQ(evt);
}

void S6MsgHandler::handlePurgeAnswer_v(cmn::IpcEMsgUnqPtr eMsg, uint32_t ueIdx)
{
	log_msg(LOG_INFO, "Inside handlePurgeAnswer_v ");

	SM::ControlBlock* controlBlk_p = SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
	if(controlBlk_p == NULL)
	{
		log_msg(LOG_ERROR, "handlePurgeAnswer_v: "
							"Failed to find UE context using idx %d",
							ueIdx);
		return;
	}
	// Fire purge-resp event, insert cb to procedure queue
	SM::Event evt(PURGE_RESP_FROM_HSS, cmn::IpcEMsgShPtr(std::move(eMsg)));
	controlBlk_p->addEventToProcQ(evt);
	
}

void S6MsgHandler::handleCancelLocationRequest_v(cmn::IpcEMsgUnqPtr eMsg)
{
	log_msg(LOG_INFO, "Inside handleCancelLocationRequest ");

	SM::ControlBlock *controlBlk_p = MmeCommonUtils::findControlBlockForS6aMsg(
		eMsg->getMsgBuffer());
	if (controlBlk_p == NULL)
	{
	    log_msg(LOG_ERROR,
		    "handleCancelLocationRequest_v: "
		    "Failed to find UE context using IMSI in CLR");
	    return;
	}

	//Fire CLR event, insert CB to Procedure Queue
	SM::Event evt(CLR_FROM_HSS, cmn::IpcEMsgShPtr(std::move(eMsg)));
	controlBlk_p->addEventToProcQ(evt);
}

void S6MsgHandler::handleDeleteSubscriberRequest_v(cmn::IpcEMsgUnqPtr eMsg)
{
    log_msg(LOG_INFO, "Inside handleDeleteSubscriberRequest ");

    SM::ControlBlock *controlBlk_p = MmeCommonUtils::findControlBlockForS6aMsg(
            eMsg->getMsgBuffer());
    if (controlBlk_p == NULL)
    {
        log_msg(LOG_ERROR,
                "handleDeleteSubscriberRequest_v: "
                "Failed to find UE context using IMSI in DSR");
        return;
    }

    //Fire DSR event, insert CB to Procedure Queue
    SM::Event evt(DSR_FROM_HSS, cmn::IpcEMsgShPtr(std::move(eMsg)));
    controlBlk_p->addEventToProcQ(evt);
}

