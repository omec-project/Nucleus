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

s10MsgHandler::~s10MsgHandler() {

}

s10MsgHandler::s10MsgHandler()
{

}

s10MsgHandler* s10MsgHandler::Instance()
{
	static s10MsgHandler msgHandler;
	return &msgHandler;
}

void s10MsgHandler::handles10Message_v(IpcEMsgUnqPtr eMsg)
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
			log_msg(LOG_INFO, "Unhandled s10 Message %d ", msgData_p->msg_type);
	}

}