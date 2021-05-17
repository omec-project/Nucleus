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
#include <eventMessage.h>

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
void S1MsgHandler::handleS1Message_v(IpcEMsgUnqPtr eMsg)
{
	log_msg(LOG_INFO, "S1 - handleS1Message_v");

	if (std::move(eMsg).get() == NULL)
		return;

	utils::MsgBuffer* msgBuf = eMsg->getMsgBuffer();
	if (msgBuf == NULL)
	{
		log_msg(LOG_INFO, "S1 Message Buffer is empty ");
		return;
	}

	log_msg(LOG_INFO, "message size %d in s1 ipc message ",msgBuf->getLength());
	if (msgBuf->getLength() < sizeof (s1_incoming_msg_header_t))
	{
	    log_msg(LOG_INFO, "Not enough bytes in s1 ipc message"
	            "Received %d but should be %d", msgBuf->getLength(),
	            sizeof (s1_incoming_msg_header_t));
	    return;
	}

	s1_incoming_msg_header_t* msgData_p = (s1_incoming_msg_header_t*)(msgBuf->getDataPointer());
	log_msg(LOG_INFO, "S1 - handleS1Message_v %d",msgData_p->msg_type);

	/* Below function should take care of decryption and integrity check */
	/* Get the control block and pass it to below function */
    do {
	    struct nasPDU nas={0};
        uint8_t     nasMsgBuf[MAX_NAS_MSG_SIZE] = {'\0'};
        uint16_t    nasMsgSize;
        uint8_t     *startNasPtr;
	    switch(msgData_p->msg_type) {
            case S1AP_INITIAL_UE_MSG_CODE: { 
	            initial_ue_msg_t *initMsg = (initial_ue_msg_t*)(msgBuf->getDataPointer());
                startNasPtr = initMsg->nasMsg.nasMsgBuf;
                nasMsgSize = initMsg->nasMsg.nasMsgSize;
                break;
            }
            case S1AP_UL_NAS_TX_MSG_CODE: { 
	            uplink_nas_t *ulMsg = (uplink_nas_t*)(msgBuf->getDataPointer());
                startNasPtr = ulMsg->nasMsg.nasMsgBuf;
                nasMsgSize = ulMsg->nasMsg.nasMsgSize;
                break;
            }
            default:
                startNasPtr = NULL;
                break;
        }
        if(startNasPtr == NULL)
            break;

        assert(MAX_NAS_MSG_SIZE > nasMsgSize);
        memcpy(nasMsgBuf, startNasPtr, nasMsgSize);

        uint32_t rc = MmeNasUtils::parse_nas_pdu(msgData_p,
                                    nasMsgBuf,
                                    nasMsgSize, &nas);
        if(SUCCESS != rc)
        {
            uint32_t s1ap_enb_ue_id = msgData_p->s1ap_enb_ue_id;
            NasPduParseFailureIndEMsgShPtr eMsg = std::make_shared<
                    NasPduParseFailureIndEMsg>(nas.header.message_type, s1ap_enb_ue_id, rc);
            handleNasPduParseFailureInd_v(eMsg, msgData_p->ue_idx);

            free(nas.elements);
            log_msg(LOG_ERROR, "NAS pdu parse failed.");
            return;
        }

	    MmeNasUtils::copy_nas_to_s1msg(&nas, msgData_p);
        free(nas.elements);
    } while(0);

	log_msg(LOG_INFO, "S1 - handleS1Message_v %d",msgData_p->msg_type);
	switch (msgData_p->msg_type)
	{
		case msg_type_t::attach_request:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_NAS_ATTACH_REQUEST);
			handleInitUeAttachRequestMsg_v(std::move(eMsg));
			break;

		case msg_type_t::id_response:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_NAS_IDENTITY_RESPONSE);
			handleIdentityResponseMsg_v(std::move(eMsg), msgData_p->ue_idx);
			break;

		case msg_type_t::auth_response:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_NAS_AUTHENTICATION_RESPONSE);
			handleAuthResponseMsg_v(std::move(eMsg), msgData_p->ue_idx);
			break;

		case msg_type_t::sec_mode_complete:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_NAS_SECURITY_MODE_RESPONSE);
			handleSecurityModeResponse_v(std::move(eMsg), msgData_p->ue_idx);
			break;

		case msg_type_t::esm_info_response:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_NAS_ESM_RESPONSE);
			handleEsmInfoResponse_v(std::move(eMsg), msgData_p->ue_idx);
			break;

		case msg_type_t::init_ctxt_response:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_S1AP_INIT_CONTEXT_RESPONSE);
			handleInitCtxtResponse_v(std::move(eMsg), msgData_p->ue_idx);
			break;

		case msg_type_t::attach_complete:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_NAS_ATTACH_COMPLETE);
			handleAttachComplete_v(std::move(eMsg), msgData_p->ue_idx);
			break;
                
		case msg_type_t::detach_request:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_NAS_DETACH_REQUEST);
			handleDetachRequest_v(std::move(eMsg), msgData_p->ue_idx);
			break;
					
		case msg_type_t::s1_release_request:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_S1AP_RELEASE_REQUEST);
			handleS1ReleaseRequestMsg_v(std::move(eMsg), msgData_p->ue_idx);
			break;
			
		case msg_type_t::s1_release_complete:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_S1AP_RELEASE_COMPLETE);
			handleS1ReleaseComplete_v(std::move(eMsg), msgData_p->ue_idx);
			break;
		
		case msg_type_t::detach_accept_from_ue:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_S1AP_DETACH_ACCEPT);
			handleDetachAcceptFromUE_v(std::move(eMsg), msgData_p->ue_idx);
			break;

		case  msg_type_t::service_request:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_S1AP_SERVICE_REQUEST);
		    handleServiceRequest_v(std::move(eMsg), msgData_p->ue_idx);
		    break;
					
		case msg_type_t::tau_request:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_S1AP_TAU_REQUEST);
			handleTauRequestMsg_v(std::move(eMsg), msgData_p->ue_idx);
			break;

		case msg_type_t::handover_request_acknowledge:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_S1AP_HANDOVER_REQUEST_ACK);
		    handleHandoverRequestAckMsg_v(std::move(eMsg), msgData_p->ue_idx);
		    break;

		case msg_type_t::handover_notify:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_S1AP_HANDOVER_NOTIFY);
		    handleHandoverNotifyMsg_v(std::move(eMsg), msgData_p->ue_idx);
		    break;

		case msg_type_t::handover_required:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_S1AP_HANDOVER_REQUIRED);
		    handleHandoverRequiredMsg_v(std::move(eMsg), msgData_p->ue_idx);
		    break;
		
		case msg_type_t::enb_status_transfer:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_S1AP_ENB_STATUS_TRANSFER);
			handleEnbStatusTransferMsg_v(std::move(eMsg), msgData_p->ue_idx);
		    break;

		case msg_type_t::handover_cancel:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_S1AP_HANDOVER_CANCEL);
			handleHandoverCancelMsg_v(std::move(eMsg), msgData_p->ue_idx);
		    break;

		case msg_type_t::handover_failure:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_S1AP_HANDOVER_FAILURE);
			handleHandoverFailureMsg_v(std::move(eMsg), msgData_p->ue_idx);
		    break;
            
		case msg_type_t::erab_mod_indication:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_S1AP_ERAB_MODIFICATION_INDICATION);
			handleErabModificationIndicationMsg_v(std::move(eMsg), msgData_p->ue_idx);
			break;

		case msg_type_t::erab_setup_response:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_S1AP_ERAB_SETUP_RESPONSE);
			handleErabSetupResponseMsg_v(std::move(eMsg), msgData_p->ue_idx);
			break;

		case msg_type_t::activate_dedicated_eps_bearer_ctxt_accept:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_NAS_ACT_DED_BR_CTXT_ACPT);
			handleActDedBearerCtxtAcceptMsg_v(std::move(eMsg), msgData_p->ue_idx);
			break;

		case msg_type_t::activate_dedicated_eps_bearer_ctxt_reject:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_NAS_ACT_DED_BR_CTXT_RJCT);
			handleActDedBearerCtxtRejectMsg_v(std::move(eMsg), msgData_p->ue_idx);
			break;

		case msg_type_t::erab_release_response:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_S1AP_ERAB_RELEASE_RESPONSE);
			handleErabRelResponseMsg_v(std::move(eMsg), msgData_p->ue_idx);
			break;

		case msg_type_t::deactivate_eps_bearer_context_accept:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_NAS_DEACT_EPS_BR_CTXT_ACPT);
			handleDeActBearerCtxtAcceptMsg_v(std::move(eMsg), msgData_p->ue_idx);
			break;

		case msg_type_t::enb_status_msg:
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_NAS_DEACT_EPS_BR_CTXT_ACPT);
			handleS1apEnbStatusMsg_v(std::move(eMsg));
			break;
		default:
			log_msg(LOG_ERROR, "Unhandled S1 Message %d ", msgData_p->msg_type);
	}
}

void S1MsgHandler::handleInitUeAttachRequestMsg_v(IpcEMsgUnqPtr eMsg)
{
	log_msg(LOG_INFO, "S1 - handleInitUeAttachRequestMsg_v");

	utils::MsgBuffer* msgData_p = eMsg->getMsgBuffer();
	SM::ControlBlock* controlBlk_p = MmeCommonUtils::findControlBlock(msgData_p);
	if (controlBlk_p == NULL)
	{
		log_msg(LOG_ERROR, "Failed to allocate ControlBlock ");

        	return;
	}

	// Fire attach-start event, insert cb to procedure queue
	SM::Event evt(ATTACH_REQ_FROM_UE, cmn::IpcEMsgShPtr(std::move(eMsg)));
	controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleIdentityResponseMsg_v(IpcEMsgUnqPtr eMsg, uint32_t ueIdx)
{
	log_msg(LOG_INFO, "S1 - handleIdentityResponseMsg_v");

	SM::ControlBlock* controlBlk_p = SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
	if(controlBlk_p == NULL)
	{
		log_msg(LOG_ERROR, "handleIdentityResponseMsg_v: "
							"Failed to find UE context using idx %d",
							ueIdx);
		return;
	}

	// Fire attach-start event, insert cb to procedure queue
	SM::Event evt(IDENTITY_RESPONSE_FROM_UE, cmn::IpcEMsgShPtr(std::move(eMsg)));
	controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleAuthResponseMsg_v(IpcEMsgUnqPtr eMsg, uint32_t ueIdx)
{
	log_msg(LOG_INFO, "S1 - handleAuthResponseMsg_v");

	SM::ControlBlock* controlBlk_p = SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
	if(controlBlk_p == NULL)
	{
		log_msg(LOG_ERROR, "handleAuthResponseMsg_v: "
							"Failed to find UE context using idx %d",
							ueIdx);
		return;
	}

    // add event in the controBlk
	// Fire attach-start event, insert cb to procedure queue
	SM::Event evt(AUTH_RESP_FROM_UE, cmn::IpcEMsgShPtr(std::move(eMsg)));
	controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleSecurityModeResponse_v(IpcEMsgUnqPtr eMsg, uint32_t ueIdx)
{
	log_msg(LOG_INFO, "S1 - handleSecurityModeResponse_v");

	SM::ControlBlock* controlBlk_p = SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
	if(controlBlk_p == NULL)
	{
		log_msg(LOG_ERROR, "handleSecurityModeResponse_v: "
							"Failed to find UE context using idx %d",
							ueIdx);
		return;
	}

	// Fire attach-start event, insert cb to procedure queue
	SM::Event evt(SEC_MODE_RESP_FROM_UE, cmn::IpcEMsgShPtr(std::move(eMsg)));
	controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleEsmInfoResponse_v(IpcEMsgUnqPtr eMsg, uint32_t ueIdx)
{
	log_msg(LOG_INFO, "S1 - handleEsmInfoResponse_v");

	SM::ControlBlock* controlBlk_p = SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
	if(controlBlk_p == NULL)
	{
		log_msg(LOG_ERROR, "handleEsmInfoResponse_v: "
							"Failed to find UE context using idx %d",
							ueIdx);
		return;
	}

	// Fire attach-start event, insert cb to procedure queue
	SM::Event evt(ESM_INFO_RESP_FROM_UE, cmn::IpcEMsgShPtr(std::move(eMsg)));
	controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleInitCtxtResponse_v(IpcEMsgUnqPtr eMsg, uint32_t ueIdx)
{
	log_msg(LOG_INFO, "S1 - handleInitCtxtResponse_v");

	SM::ControlBlock* controlBlk_p = SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
	if(controlBlk_p == NULL)
	{
		log_msg(LOG_ERROR, "handleInitCtxtResponse_v: "
							"Failed to find UE context using idx %d",
							ueIdx);
		return;
	}

	// Fire attach-start event, insert cb to procedure queue
	SM::Event evt(INIT_CTXT_RESP_FROM_UE, cmn::IpcEMsgShPtr(std::move(eMsg)));
	controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleAttachComplete_v(IpcEMsgUnqPtr eMsg, uint32_t ueIdx)
{
	log_msg(LOG_INFO, "S1 - handleAttachComplete_v");

	SM::ControlBlock* controlBlk_p = SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
	if(controlBlk_p == NULL)
	{
		log_msg(LOG_ERROR, "handleAttachComplete_v: "
							"Failed to find UE context using idx %d",
							ueIdx);
		return;
	}

	// Fire attach-start event, insert cb to procedure queue
	SM::Event evt(ATT_CMP_FROM_UE, cmn::IpcEMsgShPtr(std::move(eMsg)));
	controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleDetachRequest_v(IpcEMsgUnqPtr eMsg, uint32_t ueIdx)
{
	log_msg(LOG_INFO, "S1 - handleDetachRequest_v");

	utils::MsgBuffer* msgData_p = eMsg->getMsgBuffer();
	SM::ControlBlock* controlBlk_p = MmeCommonUtils::findControlBlock(msgData_p);
	if(controlBlk_p == NULL)
	{
		log_msg(LOG_ERROR, "handleDetachRequest_v: "
				"Failed to find UE context using idx %d", ueIdx);
		return;
	}

	// Fire detach request event, insert cb to procedure queue
	SM::Event evt(DETACH_REQ_FROM_UE, cmn::IpcEMsgShPtr(std::move(eMsg)));
	controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleS1ReleaseRequestMsg_v(IpcEMsgUnqPtr eMsg, uint32_t ueIdx)
{
	log_msg(LOG_INFO, "S1 - handleS1ReleaseRequestMsg_v");

	SM::ControlBlock* controlBlk_p = SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
	if(controlBlk_p == NULL)
    	{
		log_msg(LOG_ERROR, ":handleS1ReleaseRequestMsg_v: "
                		"Failed to find UE context using idx %d", ueIdx);
        	return;
    	}

	// Fire s1 release event, insert cb to procedure queue
	SM::Event evt(S1_REL_REQ_FROM_UE, cmn::IpcEMsgShPtr(std::move(eMsg)));
	controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleS1ReleaseComplete_v(IpcEMsgUnqPtr eMsg, uint32_t ueIdx)
{
	log_msg(LOG_INFO, "S1 - handleS1ReleaseComplete_v");

	SM::ControlBlock* controlBlk_p = SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
	if(controlBlk_p == NULL)
    	{
		log_msg(LOG_ERROR, ":handleS1ReleaseComplete_v: "
                		"Failed to find UE context using idx %d", ueIdx);
        	return;
    	}

	// Fire s1 release complete event, insert cb to procedure queue
	SM::Event evt(UE_CTXT_REL_COMP_FROM_ENB, cmn::IpcEMsgShPtr(std::move(eMsg)));
	controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleDetachAcceptFromUE_v(IpcEMsgUnqPtr eMsg, uint32_t ueIdx)
{
	log_msg(LOG_INFO, "S1 - handleDetachAcceptFromUE_v");

	SM::ControlBlock* controlBlk_p = SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
	if(controlBlk_p == NULL)
	{
		log_msg(LOG_ERROR, "handleDetachAcceptFromUE_v: "
				   "Failed to find UE Context using idx %d",
				   ueIdx);
		return;
	}

	//Fire NI_Detach Event, insert CB to procedure queue
	SM::Event evt(DETACH_ACCEPT_FROM_UE, cmn::IpcEMsgShPtr(std::move(eMsg)));
	controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleServiceRequest_v(IpcEMsgUnqPtr eMsg, uint32_t ueIdx)
{
	log_msg(LOG_INFO, "S1 - handleServiceRequest_v");

    utils::MsgBuffer* msgData_p = eMsg->getMsgBuffer();
    SM::ControlBlock* controlBlk_p = MmeCommonUtils::findControlBlock(msgData_p);
	if(controlBlk_p == NULL)
	{
		log_msg(LOG_ERROR, "handleServiceRequest_v: "
				   "Failed to find UE Context using idx %d",
				   ueIdx);
		return;
	}

	//Fire NI_Detach Event, insert CB to procedure queue
	SM::Event evt(SERVICE_REQUEST_FROM_UE, cmn::IpcEMsgShPtr(std::move(eMsg)));
	controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleTauRequestMsg_v(IpcEMsgUnqPtr eMsg, uint32_t ueIdx)
{
	log_msg(LOG_INFO, "S1 - handleTauRequestMsg_v");

    utils::MsgBuffer* msgData_p = eMsg->getMsgBuffer();
    SM::ControlBlock* controlBlk_p = MmeCommonUtils::findControlBlock(msgData_p);
	if(controlBlk_p == NULL)
	{
		log_msg(LOG_ERROR, "handleTauRequestMsg_v: "
				   "Failed to find UE Context using idx %d",
				   ueIdx);
		return;
	}

	// Fire tau-start event, insert cb to procedure queue
	SM::Event evt(TAU_REQUEST_FROM_UE, cmn::IpcEMsgShPtr(std::move(eMsg)));
	controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleHandoverRequiredMsg_v(IpcEMsgUnqPtr eMsg,
        uint32_t ueIdx)
{
    log_msg(LOG_INFO, "S1 - handleHandoverRequiredMsg_v");

    SM::ControlBlock *controlBlk_p =
            SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
    if (controlBlk_p == NULL)
    {
        log_msg(LOG_ERROR, "handleHandoverRequiredMsg_v: "
                "Failed to find UE Context using idx %d", ueIdx);
        return;
    }

    // Fire HO Required event, insert cb to procedure queue
    SM::Event evt(HO_REQUIRED_FROM_ENB, cmn::IpcEMsgShPtr(std::move(eMsg)));
    controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleHandoverRequestAckMsg_v(IpcEMsgUnqPtr eMsg,
        uint32_t ueIdx)
{
    log_msg(LOG_INFO, "S1 - handleHandoverRequestAckMsg_v");

    SM::ControlBlock *controlBlk_p =
            SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
    if (controlBlk_p == NULL)
    {
        log_msg(LOG_ERROR, "handleHandoverRequestAckMsg_v: "
                "Failed to find UE Context using idx %d", ueIdx);
        return;
    }

    // Fire HO Request Ack event, insert cb to procedure queue
    SM::Event evt(HO_REQUEST_ACK_FROM_ENB, cmn::IpcEMsgShPtr(std::move(eMsg)));
    controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleHandoverNotifyMsg_v(IpcEMsgUnqPtr eMsg,
        uint32_t ueIdx)
{
    log_msg(LOG_INFO, "S1 - handleHandoverNotifyMsg_v");

    SM::ControlBlock *controlBlk_p =
            SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
    if (controlBlk_p == NULL)
    {
        log_msg(LOG_ERROR, "handleHandoverNotifyMsg_v: "
                "Failed to find UE Context using idx %d", ueIdx);
        return;
    }

    // Fire HO Notify event, insert cb to procedure queue
    SM::Event evt(HO_NOTIFY_FROM_ENB, cmn::IpcEMsgShPtr(std::move(eMsg)));
    controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleEnbStatusTransferMsg_v(IpcEMsgUnqPtr eMsg,
        uint32_t ueIdx)
{
    log_msg(LOG_INFO, "S1 - handleEnbStatusTransferMsg_v");

    SM::ControlBlock *controlBlk_p =
            SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
    if (controlBlk_p == NULL)
    {
        log_msg(LOG_ERROR, "handleEnbStatusTransferMsg_v: "
                "Failed to find UE Context using idx %d", ueIdx);
        return;
    }

    // Fire Enb Status Transfer event, insert cb to procedure queue
    SM::Event evt(ENB_STATUS_TRANFER_FROM_SRC_ENB, cmn::IpcEMsgShPtr(std::move(eMsg)));
    controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleHandoverCancelMsg_v(IpcEMsgUnqPtr eMsg,
        uint32_t ueIdx)
{
    log_msg(LOG_INFO, "S1 - handleHandoverCancelMsg_v");

    SM::ControlBlock *controlBlk_p =
            SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
    if (controlBlk_p == NULL)
    {
        log_msg(LOG_ERROR, "handleHandoverCancelMsg_v: "
                "Failed to find UE Context using idx %d", ueIdx);
        return;
    }

    // Fire Handover Cancel event, insert cb to procedure queue
    SM::Event evt(HO_CANCEL_REQ_FROM_SRC_ENB, cmn::IpcEMsgShPtr(std::move(eMsg)));
    controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleHandoverFailureMsg_v(IpcEMsgUnqPtr eMsg,
        uint32_t ueIdx)
{
    log_msg(LOG_INFO, "S1 - handleHandoverFailureMsg_v");

    SM::ControlBlock *controlBlk_p =
            SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
    if (controlBlk_p == NULL)
    {
        log_msg(LOG_ERROR, "handleHandoverFailureMsg_v: "
                "Failed to find UE Context using idx %d", ueIdx);
        return;
    }

    // Fire Handover Failure event, insert cb to procedure queue
    SM::Event evt(HO_FAILURE_FROM_TARGET_ENB, cmn::IpcEMsgShPtr(std::move(eMsg)));
    controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleErabModificationIndicationMsg_v(IpcEMsgUnqPtr eMsg,
        uint32_t ueIdx)
{
	log_msg(LOG_INFO, "S1 - handleErabModificationIndicationMsg_v");

    SM::ControlBlock* controlBlk_p = 
            SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
	if(controlBlk_p == NULL)
    {
        log_msg(LOG_ERROR, "handleErabModificationIndicationMsg_v: "
                "Failed to find UE Context using idx %d", ueIdx);
        return;
    }

    // Fire erab_mod_ind_start event, insert cb to procedure queue
    SM::Event evt(ERAB_MOD_INDICATION_FROM_ENB, cmn::IpcEMsgShPtr(std::move(eMsg)));
    controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleErabSetupResponseMsg_v(IpcEMsgUnqPtr eMsg,
        uint32_t ueIdx)
{
    log_msg(LOG_INFO, "S1 - handleErabSetupResponseMsg_v");

    SM::ControlBlock* controlBlk_p =
            SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
    if(controlBlk_p == NULL)
    {
        log_msg(LOG_ERROR, "handleErabSetupResponseMsg_v: "
                "Failed to find UE Context using idx %d", ueIdx);
        return;
    }

    // Fire erab_setup_response event, insert cb to procedure queue
    SM::Event evt(ERAB_SETUP_RESP_FROM_ENB, cmn::IpcEMsgShPtr(std::move(eMsg)));
    controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleActDedBearerCtxtAcceptMsg_v(IpcEMsgUnqPtr eMsg,
        uint32_t ueIdx)
{
    log_msg(LOG_INFO, "S1 - handleActDedBearerCtxtAcceptMsg_v");

    SM::ControlBlock* controlBlk_p =
            SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
    if(controlBlk_p == NULL)
    {
        log_msg(LOG_ERROR, "handleActDedBearerCtxtAcceptMsg_v: "
                "Failed to find UE Context using idx %d", ueIdx);
        return;
    }

    // Fire activate_ded_bearer_ctxt_acpt event, insert cb to procedure queue
    SM::Event evt(ACT_DED_BEARER_ACCEPT_FROM_UE, cmn::IpcEMsgShPtr(std::move(eMsg)));
    controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleActDedBearerCtxtRejectMsg_v(IpcEMsgUnqPtr eMsg,
        uint32_t ueIdx)
{
    log_msg(LOG_INFO, "S1 - handleActDedBearerCtxtRejectMsg_v");

    SM::ControlBlock* controlBlk_p =
            SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
    if(controlBlk_p == NULL)
    {
        log_msg(LOG_ERROR, "handleActDedBearerCtxtRejectMsg_v: "
                "Failed to find UE Context using idx %d", ueIdx);
        return;
    }

    // Fire activate_ded_bearer_ctxt_reject event, insert cb to procedure queue
    SM::Event evt(ACT_DED_BEARER_REJECT_FROM_UE, cmn::IpcEMsgShPtr(std::move(eMsg)));
    controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleErabRelResponseMsg_v(IpcEMsgUnqPtr eMsg,
        uint32_t ueIdx)
{
    log_msg(LOG_INFO, "S1 - handleErabRelResponseMsg_v");

    SM::ControlBlock* controlBlk_p =
            SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
    if(controlBlk_p == NULL)
    {
        log_msg(LOG_ERROR, "handleErabRelResponseMsg_v: "
                "Failed to find UE Context using idx %d", ueIdx);
        return;
    }

    // Fire erab_rel_response event, insert cb to procedure queue
    SM::Event evt(ERAB_REL_RESP_FROM_ENB, cmn::IpcEMsgShPtr(std::move(eMsg)));
    controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleDeActBearerCtxtAcceptMsg_v(IpcEMsgUnqPtr eMsg,
        uint32_t ueIdx)
{
    log_msg(LOG_INFO, "S1 - handleDeActBearerCtxtAcceptMsg_v");

    SM::ControlBlock* controlBlk_p =
            SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
    if(controlBlk_p == NULL)
    {
        log_msg(LOG_ERROR, "handleDeActBearerCtxtAcceptMsg_v: "
                "Failed to find UE Context using idx %d", ueIdx);
        return;
    }

    // Fire deactivate_eps_bearer_ctxt_acpt event, insert cb to procedure queue
    SM::Event evt(DEACT_DED_BEARER_ACCEPT_FROM_UE, cmn::IpcEMsgShPtr(std::move(eMsg)));
    controlBlk_p->addEventToProcQ(evt);
}

void S1MsgHandler::handleS1apEnbStatusMsg_v(IpcEMsgUnqPtr eMsg)
{
    static s1apEnbStatus_Msg_t *enb_details[1024]; // TODO : how manhy eNBs we want to handle ?
	utils::MsgBuffer* msgBuf = eMsg->getMsgBuffer();
    assert(msgBuf != NULL);
	s1apEnbStatus_Msg_t *enb = (s1apEnbStatus_Msg_t*)(msgBuf->getDataPointer());
    log_msg(LOG_INFO, " Received enb Status message for %d ",enb->context_id);
    if(enb->context_id >= 1024) {
        log_msg(LOG_INFO, " Supported only 1024 eNBs ");
        return;
    }
     // new enb found  
     struct s1apEnbStatus_Msg *temp = NULL;
     if(enb->status == 0) {
         temp = enb_details[enb->context_id];
         if(temp == NULL) {
            log_msg(LOG_ERROR, " Received enb Status down message for unknown eNB. Ignore messsage ");
            return;
         }
     } else {
         temp = (s1apEnbStatus_Msg_t *)calloc(1, sizeof(s1apEnbStatus_Msg_t));
         memcpy(temp, enb, sizeof(*temp));
         enb_details[enb->context_id] = temp;
     }
     std::stringstream enbname;
     enbname<<temp->eNbName;
     std::stringstream tac;
     tac<<temp->tacid;
     std::stringstream enbid;
     enbid<<temp->enbId_m;
     if(enb->status == 1)
        mmeStats::Instance()->increment(mmeStatsCounter::ENB_NUM_ACTIVE, {{"enbname",enbname.str()}, {"enbid",enbid.str()},{"tac",tac.str()}});
     else
        mmeStats::Instance()->decrement(mmeStatsCounter::ENB_NUM_ACTIVE, {{"enbname",enbname.str()}, {"enbid",enbid.str()},{"tac",tac.str()}});
    return;

}

void S1MsgHandler::handleNasPduParseFailureInd_v(NasPduParseFailureIndEMsgShPtr eMsg, uint32_t ueIdx)
{
    log_msg(LOG_INFO, "S1 - handleNasPduParseFailureInd_v");

    if (ueIdx == 0) {
        // We havent identified the UE. Nothing left to do.
        return;
    }

    if (eMsg)
    {
        if (eMsg->getNasMsgType() == ServiceRequest) {
            ueIdx = SubsDataGroupManager::Instance()->findCBWithmTmsi(ueIdx);
        }

        SM::ControlBlock* cb =
                SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
        if (cb == NULL) {
            log_msg(LOG_ERROR, "handleNasPduParseFailureInd_v: "
                    "Failed to find UE Context using idx %d", ueIdx);
            return;
        }

        SM::Event evt(NAS_PDU_PARSE_FAILURE, eMsg);
        cb->addEventToProcQ(evt);
    }
    else
    {
        log_msg(LOG_ERROR, "NasPduParseFailureInd Message is NULL");
    }
}
