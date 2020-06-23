/*
 * Copyright (c) 2019, Infosys Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <msgHandlers/gtpMsgHandler.h>

#include <contextManager/subsDataGroupManager.h>
#include <event.h>
#include <eventMessage.h>
#include <ipcTypes.h>
#include <log.h>
#include <mmeSmDefs.h>
#include <utils/mmeCommonUtils.h>

using namespace SM;
using namespace mme;
using namespace cmn;

GtpMsgHandler::~GtpMsgHandler() {

}

GtpMsgHandler::GtpMsgHandler() {


}

GtpMsgHandler* GtpMsgHandler::Instance()
{
	static GtpMsgHandler msgHandler;
	return &msgHandler;
}

void GtpMsgHandler::handleGtpMessage_v(IpcEventMessage* eMsg)
{
    if (eMsg == NULL)
        return;

    utils::MsgBuffer *msgBuf = eMsg->getMsgBuffer();
    if (msgBuf == NULL)
    {
        log_msg(LOG_INFO, "GTP Message Buffer is empty \n");

        delete eMsg;
        return;
    }
    if (msgBuf->getLength() < sizeof(gtp_incoming_msg_data_t))
    {
        log_msg(LOG_INFO, "Not enough bytes in gtp message \n");

        delete eMsg;
        return;
    }

    const gtp_incoming_msg_data_t *msgData_p =
            (gtp_incoming_msg_data_t*) (msgBuf->getDataPointer());

	switch (msgData_p->msg_type)
	{
		case msg_type_t::create_session_response:
			log_msg(LOG_DEBUG,"Create Session Response msg rxed\n");
			handleCreateSessionResponseMsg_v(eMsg, msgData_p->ue_idx);
			break;

		case msg_type_t::modify_bearer_response:
			handleModifyBearerResponseMsg_v(eMsg, msgData_p->ue_idx);
			break;

		case msg_type_t::delete_session_response:
			handleDeleteSessionResponseMsg_v(eMsg, msgData_p->ue_idx);
			break;
			
		case msg_type_t::release_bearer_response:
			handleReleaseBearerResponseMsg_v(eMsg, msgData_p->ue_idx);
			break;
		
		case msg_type_t::downlink_data_notification:
			handleDdnMsg_v(eMsg, msgData_p->ue_idx);
			break;

		default:
			log_msg(LOG_INFO, "Unhandled Gtp Message %d \n", msgData_p->msg_type);
			delete eMsg;
	}

}

void GtpMsgHandler::handleCreateSessionResponseMsg_v(IpcEventMessage* eMsg, uint32_t ueIdx)
{
	log_msg(LOG_INFO, "handleCreateSessionResponseMsg_v");

	SM::ControlBlock* controlBlk_p = SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
	if(controlBlk_p == NULL)
	{
		log_msg(LOG_ERROR, "handleCreateSessionResponseMsg_v: "
							"Failed to find UE context using idx %d\n",
							ueIdx);
		return;
	}

	// Fire CS resp from SGW event, insert cb to procedure queue
	SM::Event evt(CS_RESP_FROM_SGW, eMsg);
	controlBlk_p->addEventToProcQ(evt);
}

void GtpMsgHandler::handleModifyBearerResponseMsg_v(IpcEventMessage* eMsg, uint32_t ueIdx)
{
	log_msg(LOG_INFO, "handleModifyBearerResponseMsg_v");

	SM::ControlBlock* controlBlk_p = SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
	if(controlBlk_p == NULL)
	{
		log_msg(LOG_ERROR, "handleModifyBearerResponseMsg_v: "
							"Failed to find UE context using idx %d\n",
							ueIdx);
		return;
	}

	// Fire MB rep from SGW event, insert cb to procedure queue
	SM::Event evt(MB_RESP_FROM_SGW, eMsg);
	controlBlk_p->addEventToProcQ(evt);
}

void GtpMsgHandler::handleDeleteSessionResponseMsg_v(IpcEventMessage* eMsg, uint32_t ueIdx)
{
	log_msg(LOG_INFO, "handleDeleteSessionResponseMsg_v");
	
	SM::ControlBlock* controlBlk_p = SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
	if(controlBlk_p == NULL)
	{
		log_msg(LOG_ERROR, "handleDeleteSessionResponse_v: "
							"Failed to find UE context using idx %d\n",
							ueIdx);
		return;
	}

	SM::Event evt(DEL_SESSION_RESP_FROM_SGW, eMsg);
	controlBlk_p->addEventToProcQ(evt);
}

void GtpMsgHandler::handleReleaseBearerResponseMsg_v(IpcEventMessage* eMsg, uint32_t ueIdx)
{
	log_msg(LOG_INFO, "handleReleaseBearerResponseMsg_v");

	SM::ControlBlock* controlBlk_p = SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
	if(controlBlk_p == NULL)
	{
		log_msg(LOG_ERROR, "handleReleaseBearerResponse_v: "
							"Failed to find UE context using idx %d\n",
							ueIdx);
		return;
	}
	
	// Fire rel bearer response from sgw event, insert cb to procedure queue
	SM::Event evt(REL_AB_RESP_FROM_SGW, eMsg);
	controlBlk_p->addEventToProcQ(evt);
}

void GtpMsgHandler::handleDdnMsg_v(IpcEventMessage* eMsg, uint32_t ueIdx)
{
	log_msg(LOG_INFO,"Inside handle DDN\n");

	SM::ControlBlock* controlBlk_p =
	        MmeCommonUtils::findControlBlockForS11Msg(eMsg->getMsgBuffer());
	if(controlBlk_p == NULL)
	{
		log_msg(LOG_ERROR, "handleDdnMsg_v: "
							"Failed to find UE context using idx %d\n",
							ueIdx);
		return;
	}

	// Fire ddn from sgw event, insert cb to procedure queue
	SM::Event evt(DDN_FROM_SGW, eMsg);
	controlBlk_p->addEventToProcQ(evt);
}

