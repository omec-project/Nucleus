/*
 * Copyright (c) 2019, Infosys Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <msgHandlers/s1MsgHandler.h>
#include <event.h>
#include <ipcTypes.h>
#include <log.h>
#include <utils/mmeCommonUtils.h>
#include <contextManager/subsDataGroupManager.h>
#include <mmeSmDefs.h>
#include <eventMessage.h>
#include "mmeNasUtils.h"
#include "mmeStatsPromClient.h"

using namespace SM;
using namespace mme;
using namespace cmn;

S1MsgHandler::S1MsgHandler()
{

}

S1MsgHandler::~S1MsgHandler()
{

}

S1MsgHandler* S1MsgHandler::Instance()
{
	static S1MsgHandler msgHandler;
	return &msgHandler;
}

// Starting point 
void S1MsgHandler::handleS1Message_v(IpcEventMessage* eMsg)
{
	log_msg(LOG_INFO, "S1 - handleS1Message_v\n");

	if (eMsg == NULL)
		return;

	utils::MsgBuffer* msgBuf = eMsg->getMsgBuffer();
	if (msgBuf == NULL)
	{
		log_msg(LOG_INFO, "S1 Message Buffer is empty \n");

	    	delete eMsg;
	    	return;
	}
    	log_msg(LOG_INFO, "message size %d in s1 ipc message \n",msgBuf->getLength());
	if (msgBuf->getLength() < sizeof (s1_incoming_msg_data_t))
	{
	    log_msg(LOG_INFO, "Not enough bytes in s1 ipc message"
	            "Received %d but should be %d\n", msgBuf->getLength(),
	            sizeof (s1_incoming_msg_data_t));

	    	delete eMsg;
	    	return;
	}

	s1_incoming_msg_data_t* msgData_p = (s1_incoming_msg_data_t*)(msgBuf->getDataPointer());

	struct nasPDU nas={0};
	/* Below function should take care of decryption and integrity check */
	/* Get the control block and pass it to below function */
	if(msgData_p->msg_type == raw_nas_msg)
	{
        s1apMsg_plus_raw_nas *nasMsg = &msgData_p->msg_data.rawMsg;

		if(E_FAIL == MmeNasUtils::parse_nas_pdu(msgData_p,
                                    nasMsg->nasMsgBuf, 
                                    nasMsg->nasMsgSize, &nas))
        {
            log_msg(LOG_ERROR,"NAS pdu parse failed.\n");
            return;
        }
		MmeNasUtils::copy_nas_to_s1msg(&nas, msgData_p);
	}

	log_msg(LOG_INFO, "S1 - handleS1Message_v %d\n",msgData_p->msg_type);
	switch (msgData_p->msg_type)
	{
		case msg_type_t::attach_request:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_NAS_ATTACH_REQUEST);
			handleInitUeAttachRequestMsg_v(eMsg);
			break;

		case msg_type_t::id_response:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_NAS_IDENTITY_RESPONSE);
			handleIdentityResponseMsg_v(eMsg, msgData_p->ue_idx);
			break;

		case msg_type_t::auth_response:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_NAS_AUTHENTICATION_RESPONSE);
			handleAuthResponseMsg_v(eMsg, msgData_p->ue_idx);
			break;

		case msg_type_t::sec_mode_complete:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_NAS_SECURITY_MODE_RESPONSE);
			handleSecurityModeResponse_v(eMsg, msgData_p->ue_idx);
			break;

		case msg_type_t::esm_info_response:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_NAS_ESM_RESPONSE);
			handleEsmInfoResponse_v(eMsg, msgData_p->ue_idx);
			break;

		case msg_type_t::init_ctxt_response:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_S1AP_INIT_CONTEXT_RESPONSE);
			handleInitCtxtResponse_v(eMsg, msgData_p->ue_idx);
			break;

		case msg_type_t::attach_complete:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_NAS_ATTACH_COMPLETE);
			handleAttachComplete_v(eMsg, msgData_p->ue_idx);
			break;
                
		case msg_type_t::detach_request:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_NAS_DETACH_REQUEST);
			handleDetachRequest_v(eMsg, msgData_p->ue_idx);
			break;
					
		case msg_type_t::s1_release_request:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_S1AP_RELEASE_REQUEST);
			handleS1ReleaseRequestMsg_v(eMsg, msgData_p->ue_idx);
			break;
			
		case msg_type_t::s1_release_complete:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_S1AP_RELEASE_COMPLETE);
			handleS1ReleaseComplete_v(eMsg, msgData_p->ue_idx);
			break;
		
		case msg_type_t::detach_accept_from_ue:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_S1AP_DETACH_ACCEPT);
			handleDetachAcceptFromUE_v(eMsg, msgData_p->ue_idx);
			break;

		case  msg_type_t::service_request:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_S1AP_SERVICE_REQUEST);
		    handleServiceRequest_v(eMsg, msgData_p->ue_idx);
		    break;
					
		case msg_type_t::tau_request:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_S1AP_TAU_REQUEST);
			handleTauRequestMsg_v(eMsg, msgData_p->ue_idx);
			break;

		case msg_type_t::handover_request_acknowledge:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_S1AP_HANDOVER_REQUEST_ACK);
		    handleHandoverRequestAckMsg_v(eMsg, msgData_p->ue_idx);
		    break;

		case msg_type_t::handover_notify:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_S1AP_HANDOVER_NOTIFY);
		    handleHandoverNotifyMsg_v(eMsg, msgData_p->ue_idx);
		    break;

		case msg_type_t::handover_required:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_S1AP_HANDOVER_REQUIRED);
		    handleHandoverRequiredMsg_v(eMsg, msgData_p->ue_idx);
		    break;
		
		case msg_type_t::enb_status_transfer:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_S1AP_ENB_STATUS_TRANSFER);
			handleEnbStatusTransferMsg_v(eMsg, msgData_p->ue_idx);
		    break;

		case msg_type_t::handover_cancel:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_S1AP_HANDOVER_CANCEL);
			handleHandoverCancelMsg_v(eMsg, msgData_p->ue_idx);
		    break;

		case msg_type_t::handover_failure:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_S1AP_HANDOVER_FAILURE);
			handleHandoverFailureMsg_v(eMsg, msgData_p->ue_idx);
		    break;
            
		case msg_type_t::erab_mod_indication:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_S1AP_ERAB_MODIFICATION_INDICATION);
			handleErabModificationIndicationMsg_v(eMsg, msgData_p->ue_idx);		
			break;

		default:
			log_msg(LOG_ERROR, "Unhandled S1 Message %d \n", msgData_p->msg_type);
			delete eMsg;
	}
}

void S1MsgHandler::handleInitUeAttachRequestMsg_v(IpcEventMessage* eMsg)
{
	log_msg(LOG_INFO, "S1 - handleInitUeAttachRequestMsg_v\n");

	utils::MsgBuffer* msgData_p = eMsg->getMsgBuffer();
	SM::ControlBlock* controlBlk_p = MmeCommonUtils::findControlBlock(msgData_p);
	if (controlBlk_p == NULL)
	{
		log_msg(LOG_ERROR, "Failed to allocate ControlBlock \n");

        	return;
	}

	// Fire attach-start event, insert cb to procedure queue
	SM::Event evt(ATTACH_REQ_FROM_UE, eMsg);
	controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleIdentityResponseMsg_v(IpcEventMessage* eMsg, uint32_t ueIdx)
{
	log_msg(LOG_INFO, "S1 - handleIdentityResponseMsg_v\n");

	SM::ControlBlock* controlBlk_p = SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
	if(controlBlk_p == NULL)
	{
		log_msg(LOG_ERROR, "handleIdentityResponseMsg_v: "
							"Failed to find UE context using idx %d\n",
							ueIdx);
		return;
	}

	// Fire attach-start event, insert cb to procedure queue
	SM::Event evt(IDENTITY_RESPONSE_FROM_UE, eMsg);
	controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleAuthResponseMsg_v(IpcEventMessage* eMsg, uint32_t ueIdx)
{
	log_msg(LOG_INFO, "S1 - handleAuthResponseMsg_v\n");

	SM::ControlBlock* controlBlk_p = SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
	if(controlBlk_p == NULL)
	{
		log_msg(LOG_ERROR, "handleAuthResponseMsg_v: "
							"Failed to find UE context using idx %d\n",
							ueIdx);
		return;
	}

    // add event in the controBlk
	// Fire attach-start event, insert cb to procedure queue
	SM::Event evt(AUTH_RESP_FROM_UE, eMsg);
	controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleSecurityModeResponse_v(IpcEventMessage* eMsg, uint32_t ueIdx)
{
	log_msg(LOG_INFO, "S1 - handleSecurityModeResponse_v\n");

	SM::ControlBlock* controlBlk_p = SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
	if(controlBlk_p == NULL)
	{
		log_msg(LOG_ERROR, "handleSecurityModeResponse_v: "
							"Failed to find UE context using idx %d\n",
							ueIdx);
		return;
	}

	// Fire attach-start event, insert cb to procedure queue
	SM::Event evt(SEC_MODE_RESP_FROM_UE, eMsg);
	controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleEsmInfoResponse_v(IpcEventMessage* eMsg, uint32_t ueIdx)
{
	log_msg(LOG_INFO, "S1 - handleEsmInfoResponse_v\n");

	SM::ControlBlock* controlBlk_p = SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
	if(controlBlk_p == NULL)
	{
		log_msg(LOG_ERROR, "handleEsmInfoResponse_v: "
							"Failed to find UE context using idx %d\n",
							ueIdx);
		return;
	}

	// Fire attach-start event, insert cb to procedure queue
	SM::Event evt(ESM_INFO_RESP_FROM_UE, eMsg);
	controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleInitCtxtResponse_v(IpcEventMessage* eMsg, uint32_t ueIdx)
{
	log_msg(LOG_INFO, "S1 - handleInitCtxtResponse_v\n");

	SM::ControlBlock* controlBlk_p = SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
	if(controlBlk_p == NULL)
	{
		log_msg(LOG_ERROR, "handleInitCtxtResponse_v: "
							"Failed to find UE context using idx %d\n",
							ueIdx);
		return;
	}

	// Fire attach-start event, insert cb to procedure queue
	SM::Event evt(INIT_CTXT_RESP_FROM_UE, eMsg);
	controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleAttachComplete_v(IpcEventMessage* eMsg, uint32_t ueIdx)
{
	log_msg(LOG_INFO, "S1 - handleAttachComplete_v\n");

	SM::ControlBlock* controlBlk_p = SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
	if(controlBlk_p == NULL)
	{
		log_msg(LOG_ERROR, "handleAttachComplete_v: "
							"Failed to find UE context using idx %d\n",
							ueIdx);
		return;
	}

	// Fire attach-start event, insert cb to procedure queue
	SM::Event evt(ATT_CMP_FROM_UE, eMsg);
	controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleDetachRequest_v(IpcEventMessage* eMsg, uint32_t ueIdx)
{
	log_msg(LOG_INFO, "S1 - handleDetachRequest_v\n");

	utils::MsgBuffer* msgData_p = eMsg->getMsgBuffer();
	SM::ControlBlock* controlBlk_p = MmeCommonUtils::findControlBlock(msgData_p);
	if(controlBlk_p == NULL)
	{
		log_msg(LOG_ERROR, "handleDetachRequest_v: "
				"Failed to find UE context using idx %d\n", ueIdx);
		return;
	}

	// Fire detach request event, insert cb to procedure queue
	SM::Event evt(DETACH_REQ_FROM_UE, eMsg);
	controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleS1ReleaseRequestMsg_v(IpcEventMessage* eMsg, uint32_t ueIdx)
{
	log_msg(LOG_INFO, "S1 - handleS1ReleaseRequestMsg_v\n");

	SM::ControlBlock* controlBlk_p = SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
	if(controlBlk_p == NULL)
    	{
		log_msg(LOG_ERROR, ":handleS1ReleaseRequestMsg_v: "
                		"Failed to find UE context using idx %d\n", ueIdx);
        	return;
    	}

	// Fire s1 release event, insert cb to procedure queue
	SM::Event evt(S1_REL_REQ_FROM_UE, eMsg);
	controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleS1ReleaseComplete_v(IpcEventMessage* eMsg, uint32_t ueIdx)
{
	log_msg(LOG_INFO, "S1 - handleS1ReleaseComplete_v\n");

	SM::ControlBlock* controlBlk_p = SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
	if(controlBlk_p == NULL)
    	{
		log_msg(LOG_ERROR, ":handleS1ReleaseComplete_v: "
                		"Failed to find UE context using idx %d\n", ueIdx);
        	return;
    	}

	// Fire s1 release complete event, insert cb to procedure queue
	SM::Event evt(UE_CTXT_REL_COMP_FROM_ENB, eMsg);
	controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleDetachAcceptFromUE_v(IpcEventMessage* eMsg, uint32_t ueIdx)
{
	log_msg(LOG_INFO, "S1 - handleDetachAcceptFromUE_v\n");

	SM::ControlBlock* controlBlk_p = SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
	if(controlBlk_p == NULL)
	{
		log_msg(LOG_ERROR, "handleDetachAcceptFromUE_v: "
				   "Failed to find UE Context using idx %d\n",
				   ueIdx);
		return;
	}

	//Fire NI_Detach Event, insert CB to procedure queue
	SM::Event evt(DETACH_ACCEPT_FROM_UE, eMsg);
	controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleServiceRequest_v(IpcEventMessage* eMsg, uint32_t ueIdx)
{
	log_msg(LOG_INFO, "S1 - handleServiceRequest_v\n");

    utils::MsgBuffer* msgData_p = eMsg->getMsgBuffer();
    SM::ControlBlock* controlBlk_p = MmeCommonUtils::findControlBlock(msgData_p);
	if(controlBlk_p == NULL)
	{
		log_msg(LOG_ERROR, "handleServiceRequest_v: "
				   "Failed to find UE Context using idx %d\n",
				   ueIdx);
		return;
	}

	//Fire NI_Detach Event, insert CB to procedure queue
	SM::Event evt(SERVICE_REQUEST_FROM_UE, eMsg);
	controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleTauRequestMsg_v(IpcEventMessage* eMsg, uint32_t ueIdx)
{
	log_msg(LOG_INFO, "S1 - handleTauRequestMsg_v\n");

    utils::MsgBuffer* msgData_p = eMsg->getMsgBuffer();
    SM::ControlBlock* controlBlk_p = MmeCommonUtils::findControlBlock(msgData_p);
	if(controlBlk_p == NULL)
	{
		log_msg(LOG_ERROR, "handleTauRequestMsg_v: "
				   "Failed to find UE Context using idx %d\n",
				   ueIdx);
		return;
	}

	// Fire tau-start event, insert cb to procedure queue
	SM::Event evt(TAU_REQUEST_FROM_UE, eMsg);
	controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleHandoverRequiredMsg_v(IpcEventMessage *eMsg,
        uint32_t ueIdx)
{
    log_msg(LOG_INFO, "S1 - handleHandoverRequiredMsg_v\n");

    SM::ControlBlock *controlBlk_p =
            SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
    if (controlBlk_p == NULL)
    {
        log_msg(LOG_ERROR, "handleHandoverRequiredMsg_v: "
                "Failed to find UE Context using idx %d\n", ueIdx);
        return;
    }

    // Fire HO Required event, insert cb to procedure queue
    SM::Event evt(HO_REQUIRED_FROM_ENB, eMsg);
    controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleHandoverRequestAckMsg_v(IpcEventMessage *eMsg,
        uint32_t ueIdx)
{
    log_msg(LOG_INFO, "S1 - handleHandoverRequestAckMsg_v\n");

    SM::ControlBlock *controlBlk_p =
            SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
    if (controlBlk_p == NULL)
    {
        log_msg(LOG_ERROR, "handleHandoverRequestAckMsg_v: "
                "Failed to find UE Context using idx %d\n", ueIdx);
        return;
    }

    // Fire HO Request Ack event, insert cb to procedure queue
    SM::Event evt(HO_REQUEST_ACK_FROM_ENB, eMsg);
    controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleHandoverNotifyMsg_v(IpcEventMessage *eMsg,
        uint32_t ueIdx)
{
    log_msg(LOG_INFO, "S1 - handleHandoverNotifyMsg_v\n");

    SM::ControlBlock *controlBlk_p =
            SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
    if (controlBlk_p == NULL)
    {
        log_msg(LOG_ERROR, "handleHandoverNotifyMsg_v: "
                "Failed to find UE Context using idx %d\n", ueIdx);
        return;
    }

    // Fire HO Notify event, insert cb to procedure queue
    SM::Event evt(HO_NOTIFY_FROM_ENB, eMsg);
    controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleEnbStatusTransferMsg_v(IpcEventMessage *eMsg,
        uint32_t ueIdx)
{
    log_msg(LOG_INFO, "S1 - handleEnbStatusTransferMsg_v\n");

    SM::ControlBlock *controlBlk_p =
            SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
    if (controlBlk_p == NULL)
    {
        log_msg(LOG_ERROR, "handleEnbStatusTransferMsg_v: "
                "Failed to find UE Context using idx %d\n", ueIdx);
        return;
    }

    // Fire Enb Status Transfer event, insert cb to procedure queue
    SM::Event evt(ENB_STATUS_TRANFER_FROM_SRC_ENB, eMsg);
    controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleHandoverCancelMsg_v(IpcEventMessage *eMsg,
        uint32_t ueIdx)
{
    log_msg(LOG_INFO, "S1 - handleHandoverCancelMsg_v\n");

    SM::ControlBlock *controlBlk_p =
            SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
    if (controlBlk_p == NULL)
    {
        log_msg(LOG_ERROR, "handleHandoverCancelMsg_v: "
                "Failed to find UE Context using idx %d\n", ueIdx);
        return;
    }

    // Fire Handover Cancel event, insert cb to procedure queue
    SM::Event evt(HO_CANCEL_REQ_FROM_SRC_ENB, eMsg);
    controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleHandoverFailureMsg_v(IpcEventMessage *eMsg,
        uint32_t ueIdx)
{
    log_msg(LOG_INFO, "S1 - handleHandoverFailureMsg_v\n");

    SM::ControlBlock *controlBlk_p =
            SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
    if (controlBlk_p == NULL)
    {
        log_msg(LOG_ERROR, "handleHandoverFailureMsg_v: "
                "Failed to find UE Context using idx %d\n", ueIdx);
        return;
    }

    // Fire Handover Failure event, insert cb to procedure queue
    SM::Event evt(HO_FAILURE_FROM_TARGET_ENB, eMsg);
    controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleErabModificationIndicationMsg_v(IpcEventMessage* eMsg, 
        uint32_t ueIdx)
{
	log_msg(LOG_INFO, "S1 - handleErabModificationIndicationMsg_v\n");

    SM::ControlBlock* controlBlk_p = 
            SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
	if(controlBlk_p == NULL)
    {
        log_msg(LOG_ERROR, "handleErabModificationIndicationMsg_v: "
                "Failed to find UE Context using idx %d\n", ueIdx);
        return;
    }

    // Fire erab_mod_ind_start event, insert cb to procedure queue
    SM::Event evt(ERAB_MOD_INDICATION_FROM_ENB, eMsg);
    controlBlk_p->addEventToProcQ(evt);
}
