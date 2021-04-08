/*
 * Copyright (c) 2019, Infosys Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <msgHandlers/gtpMsgHandler.h>

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

GtpMsgHandler::~GtpMsgHandler() {

}

GtpMsgHandler::GtpMsgHandler()
{

}

GtpMsgHandler* GtpMsgHandler::Instance()
{
	static GtpMsgHandler msgHandler;
	return &msgHandler;
}

void GtpMsgHandler::handleGtpMessage_v(IpcEMsgUnqPtr eMsg)
{
    if (eMsg.get() == NULL)
        return;

    utils::MsgBuffer *msgBuf = eMsg->getMsgBuffer();
    if (msgBuf == NULL)
    {
        log_msg(LOG_INFO, "GTP Message Buffer is empty ");
        return;
    }
    if (msgBuf->getLength() < sizeof(gtp_incoming_msg_data_t))
    {
        log_msg(LOG_INFO, "Not enough bytes in gtp message ");
        return;
    }

    const gtp_incoming_msg_data_t *msgData_p =
            (gtp_incoming_msg_data_t*) (msgBuf->getDataPointer());

	switch (msgData_p->msg_type)
	{
		case msg_type_t::create_session_response:
		{
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_S11_CREATE_SESSION_RESPONSE);
			const struct csr_Q_msg* csr_info= (const struct csr_Q_msg*) (msgBuf->getDataPointer());
			handleCreateSessionResponseMsg_v(std::move(eMsg), csr_info->s11_mme_cp_teid);
		}
		break;

		case msg_type_t::modify_bearer_response:
		{
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_S11_MODIFY_BEARER_RESPONSE);
			const struct MB_resp_Q_msg* mbr_info= (const struct MB_resp_Q_msg*) (msgBuf->getDataPointer());
			handleModifyBearerResponseMsg_v(std::move(eMsg), mbr_info->s11_mme_cp_teid);
		}
		break;

		case msg_type_t::delete_session_response:
		{
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_S11_DELETE_SESSION_RESPONSE);
			const struct DS_resp_Q_msg* dsr_info= (const struct DS_resp_Q_msg*) (msgBuf->getDataPointer());
			handleDeleteSessionResponseMsg_v(std::move(eMsg), dsr_info->s11_mme_cp_teid);
		}
		break;
			
		case msg_type_t::release_bearer_response:
		{
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_S11_RELEASE_BEARER_RESPONSE);
			const struct RB_resp_Q_msg* rbr_info= (const struct RB_resp_Q_msg*) (msgBuf->getDataPointer());
			handleReleaseBearerResponseMsg_v(std::move(eMsg), rbr_info->s11_mme_cp_teid);
		}
		break;
		
		case msg_type_t::downlink_data_notification:
		{
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_S11_DOWNLINK_NOTIFICATION_INDICATION);
			const struct ddn_Q_msg* ddn = (const struct ddn_Q_msg*) (msgBuf->getDataPointer());
			handleDdnMsg_v(std::move(eMsg), ddn->s11_mme_cp_teid);
		}
		break;

		case msg_type_t::create_bearer_request:
		{
			mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_S11_CREATE_BEARER_REQUEST);
			const struct cb_req_Q_msg * cbr = (const struct cb_req_Q_msg *) (msgBuf->getDataPointer());
			handleCreateBearerRequestMsg_v(std::move(eMsg), cbr->s11_mme_cp_teid);
		}
		break;

		case msg_type_t::delete_bearer_request:
                {
                        mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_S11_DELETE_BEARER_REQUEST);
                        const struct db_req_Q_msg * dbr = (const struct db_req_Q_msg *) (msgBuf->getDataPointer());
                        handleDeleteBearerRequestMsg_v(std::move(eMsg), dbr->s11_mme_cp_teid);
                }
                break;

		default:
			log_msg(LOG_INFO, "Unhandled Gtp Message %d ", msgData_p->msg_type);
	}

}

void GtpMsgHandler::handleCreateSessionResponseMsg_v(IpcEMsgUnqPtr eMsg, uint32_t ueIdx)
{
	log_msg(LOG_INFO, "handleCreateSessionResponseMsg_v");

	SM::ControlBlock* controlBlk_p = SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
	if(controlBlk_p == NULL)
	{
		log_msg(LOG_ERROR, "handleCreateSessionResponseMsg_v: "
							"Failed to find UE context using idx %d",
							ueIdx);
		return;
	}

	// Fire CS resp from SGW event, insert cb to procedure queue
	SM::Event evt(CS_RESP_FROM_SGW, cmn::IpcEMsgShPtr(std::move(eMsg)));
	controlBlk_p->addEventToProcQ(evt);
}

void GtpMsgHandler::handleModifyBearerResponseMsg_v(IpcEMsgUnqPtr eMsg, uint32_t ueIdx)
{
	log_msg(LOG_INFO, "handleModifyBearerResponseMsg_v");

	SM::ControlBlock* controlBlk_p = SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
	if(controlBlk_p == NULL)
	{
		log_msg(LOG_ERROR, "handleModifyBearerResponseMsg_v: "
							"Failed to find UE context using idx %d",
							ueIdx);
		return;
	}

	// Fire MB rep from SGW event, insert cb to procedure queue
	SM::Event evt(MB_RESP_FROM_SGW, cmn::IpcEMsgShPtr(std::move(eMsg)));
	controlBlk_p->addEventToProcQ(evt);
}

void GtpMsgHandler::handleDeleteSessionResponseMsg_v(IpcEMsgUnqPtr eMsg, uint32_t ueIdx)
{
	log_msg(LOG_INFO, "handleDeleteSessionResponseMsg_v");
	
	SM::ControlBlock* controlBlk_p = SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
	if(controlBlk_p == NULL)
	{
		log_msg(LOG_ERROR, "handleDeleteSessionResponse_v: "
							"Failed to find UE context using idx %d",
							ueIdx);
		return;
	}

	SM::Event evt(DEL_SESSION_RESP_FROM_SGW, cmn::IpcEMsgShPtr(std::move(eMsg)));
	controlBlk_p->addEventToProcQ(evt);
}

void GtpMsgHandler::handleReleaseBearerResponseMsg_v(IpcEMsgUnqPtr eMsg, uint32_t ueIdx)
{
	log_msg(LOG_INFO, "handleReleaseBearerResponseMsg_v");

	SM::ControlBlock* controlBlk_p = SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
	if(controlBlk_p == NULL)
	{
		log_msg(LOG_ERROR, "handleReleaseBearerResponse_v: "
							"Failed to find UE context using idx %d",
							ueIdx);
		return;
	}
	
	// Fire rel bearer response from sgw event, insert cb to procedure queue
	SM::Event evt(REL_AB_RESP_FROM_SGW, cmn::IpcEMsgShPtr(std::move(eMsg)));
	controlBlk_p->addEventToProcQ(evt);
}

void GtpMsgHandler::handleDdnMsg_v(IpcEMsgUnqPtr eMsg, uint32_t ueIdx)
{
	log_msg(LOG_INFO,"Inside handle DDN");

	SM::ControlBlock* controlBlk_p =
	        MmeCommonUtils::findControlBlockForS11Msg(eMsg->getMsgBuffer());
	if(controlBlk_p == NULL)
	{
		log_msg(LOG_ERROR, "handleDdnMsg_v: "
							"Failed to find UE context using idx %d",
							ueIdx);
		return;
	}

	// Fire ddn from sgw event, insert cb to procedure queue
	SM::Event evt(DDN_FROM_SGW, cmn::IpcEMsgShPtr(std::move(eMsg)));
	controlBlk_p->addEventToProcQ(evt);
}

void GtpMsgHandler::handleCreateBearerRequestMsg_v(IpcEMsgUnqPtr eMsg, uint32_t ueIdx)
{
        log_msg(LOG_INFO,"Inside handle Create Bearer Request");

        SM::ControlBlock* controlBlk_p =
                MmeCommonUtils::findControlBlockForS11Msg(eMsg->getMsgBuffer());
        if(controlBlk_p == NULL)
        {
                   /* TODO : generate CBRsp with cause context not found */
                  log_msg(LOG_ERROR, "handleCreateBearerRequestMsg_v: "
                                                        "Failed to find UE context using idx %d",
                                                        ueIdx);
                return;
        }

        // Fire CB Req from gw event, insert cb to procedure queue
        SM::Event evt(CREATE_BEARER_REQ_FROM_GW, cmn::IpcEMsgShPtr(std::move(eMsg)));
        controlBlk_p->addEventToProcQ(evt);
}

void GtpMsgHandler::handleDeleteBearerRequestMsg_v(IpcEMsgUnqPtr eMsg, uint32_t ueIdx)
{
        log_msg(LOG_INFO,"Inside handle Delete Bearer Request");

        SM::ControlBlock* controlBlk_p =
                MmeCommonUtils::findControlBlockForS11Msg(eMsg->getMsgBuffer());
        if(controlBlk_p == NULL)
        {
                log_msg(LOG_ERROR, "handleDeleteBearerRequestMsg_v: "
                                                        "Failed to find UE context using idx %d",
                                                        ueIdx);
                return;
        }

        // Fire DB Req from gw event, insert cb to procedure queue
        SM::Event evt(DELETE_BEARER_REQ_FROM_GW, cmn::IpcEMsgShPtr(std::move(eMsg)));
        controlBlk_p->addEventToProcQ(evt);
}
