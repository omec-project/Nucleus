/*
 * Copyright 2020-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/******************************************************************************
 *
 * This file has both generated and manual code.
 * 
 * File template used for code generation:
 * <TOP-DIR/scripts/SMCodeGen/templates/stateMachineTmpls/actionHandlers.cpp.tt>
 *
 ******************************************************************************/
#include "actionHandlers/actionHandlers.h"
#include "contextManager/subsDataGroupManager.h"
#include "contextManager/dataBlocks.h"
#include "controlBlock.h" 
#include "err_codes.h"
#include "gtpCauseTypes.h"
#include <interfaces/mmeIpcInterface.h>
#include <ipcTypes.h>
#include "log.h"
#include "mme_app.h"
#include <mmeSmDefs.h>
#include <msgBuffer.h>
#include "msgType.h"
#include "procedureStats.h"
#include "state.h"
#include <stateMachineEngine.h>
#include <tipcTypes.h>
#include <utils/mmeCauseUtils.h>
#include <utils/mmeContextManagerUtils.h>
#include <utils/mmeS1MsgUtils.h>
#include "mmeStatsPromClient.h"

using namespace mme;
using namespace SM;
using namespace cmn;
using namespace cmn::utils;

/***************************************
 * Action handler : process_erab_release_response
 ***************************************/
ActStatus ActionHandlers::process_erab_release_response(ControlBlock &cb)
{
    log_msg(LOG_DEBUG, "process_erab_release_response : Entry\n");

    UEContext *ueCtxt_p = static_cast<UEContext*>(cb.getPermDataBlock());
    VERIFY_UE(cb, ueCtxt_p, "Invalid UE\n");

    SmDedDeActProcCtxt *dedBrDeActProc_p =
            dynamic_cast<SmDedDeActProcCtxt*>(cb.getTempDataBlock());
    VERIFY(dedBrDeActProc_p, return ActStatus::ABORT,
            "Ded Bearer DeActivation Procedure Context is NULL \n");

    MsgBuffer *msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
    VERIFY(msgBuf,
            dedBrDeActProc_p->setMmeErrorCause(INVALID_MSG_BUFFER); return ActStatus::ABORT,
            "process_erab_release_response : Invalid message buffer \n");

    const erab_rel_resp_Q_msg_t *erab_rel_resp = static_cast<const erab_rel_resp_Q_msg_t*>(msgBuf->getDataPointer());
    VERIFY(erab_rel_resp,
            dedBrDeActProc_p->setMmeErrorCause(INVALID_DATA_BUFFER); return ActStatus::ABORT,
            "process_erab_release_response : Invalid data buffer \n");

    ActStatus actStatus = ActStatus::PROCEED;
    bool bearerEntryFound = false;

    for (int i = 0; i < erab_rel_resp->erab_rel_list.count; i++)
    {
        if (erab_rel_resp->erab_rel_list.erab_id[i]
                == dedBrDeActProc_p->getBearerId())
        {
            bearerEntryFound = true;
            break;
        }
    }

    if (!bearerEntryFound)
    {
        actStatus = ActStatus::ABORT;
        dedBrDeActProc_p->setMmeErrorCause(S1AP_FAILURE_IND);

        for (int i = 0; i < erab_rel_resp->erab_failed_to_release_list.count;
                i++)
        {
            if (erab_rel_resp->erab_failed_to_release_list.erab_item[i].e_RAB_ID
                    == dedBrDeActProc_p->getBearerId())
            {
                dedBrDeActProc_p->setS1apCause(
                        erab_rel_resp->erab_failed_to_release_list.erab_item[i].cause);
                bearerEntryFound = true;
            }
        }
    }

    log_msg(LOG_DEBUG, "process_erab_release_response : Exit\n");
    return actStatus;
}

/***************************************
 * Action handler : process_deact_ded_bearer_accept
 ***************************************/
ActStatus ActionHandlers::process_deact_ded_bearer_accept(ControlBlock &cb)
{
    log_msg(LOG_DEBUG, "process_deact_ded_bearer_accept : Entry\n");

    UEContext *ueCtxt_p = static_cast<UEContext*>(cb.getPermDataBlock());
    VERIFY_UE(cb, ueCtxt_p, "Invalid UE\n");

    SmDedDeActProcCtxt *dedBrDeActProc_p =
            dynamic_cast<SmDedDeActProcCtxt*>(cb.getTempDataBlock());
    VERIFY(dedBrDeActProc_p, return ActStatus::ABORT,
            "Ded Bearer DeActivation Procedure Context is NULL \n");

    MsgBuffer *msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
    VERIFY(msgBuf,
            dedBrDeActProc_p->setMmeErrorCause(INVALID_MSG_BUFFER); return ActStatus::ABORT,
            "process_deact_ded_bearer_accept : Invalid message buffer \n");

    const deactivate_epsbearerctx_accept_Q_msg_t *dedBrDeActAcpt =
            static_cast<const deactivate_epsbearerctx_accept_Q_msg*>(msgBuf->getDataPointer());
    VERIFY(dedBrDeActAcpt,
            dedBrDeActProc_p->setMmeErrorCause(INVALID_DATA_BUFFER); return ActStatus::ABORT,
            "process_deact_ded_bearer_accept : Invalid data buffer \n");

    BearerContext *bearerCtxt_p = MmeContextManagerUtils::findBearerContext(
            dedBrDeActProc_p->getBearerId(), ueCtxt_p);
    VERIFY(bearerCtxt_p,
            dedBrDeActProc_p->setMmeErrorCause(BEARER_CONTEXT_NOT_FOUND); return ActStatus::ABORT,
            "Bearer Context is NULL \n");

    if (dedBrDeActAcpt->pco_opt.pco_length > 0)
    {
        if (dedBrDeActProc_p->getTriggerProc() == dbReq_c)
        {
            MmeSmDeleteBearerProcCtxt *dbReqProc_p =
                    dynamic_cast<MmeSmDeleteBearerProcCtxt*>(MmeContextManagerUtils::findProcedureCtxt(
                            cb, dedBrDeActProc_p->getTriggerProc()));

            auto &bsContainer = dbReqProc_p->getBearerStatusContainer();
            for (auto &entry : bsContainer)
            {
                if (entry.eps_bearer_id
                        == dedBrDeActProc_p->getBearerId())
                {
                    entry.pco_from_ue_opt.pco_length =
                            dedBrDeActAcpt->pco_opt.pco_length;
                    memcpy(
                            entry.pco_from_ue_opt.pco_options,
                            dedBrDeActAcpt->pco_opt.pco_options,
                            dedBrDeActAcpt->pco_opt.pco_length);

                    break;
                }
            }
        }
    }

    log_msg(LOG_DEBUG, "process_deact_ded_bearer_accept : Exit\n");
    return ActStatus::PROCEED;
}

/***************************************
 * Action handler : abort_ded_deactivation
 ***************************************/
ActStatus ActionHandlers::abort_ded_deactivation(ControlBlock &cb)
{
    log_msg(LOG_DEBUG, "abort_ded_deactivation : Entry\n");

    UEContext *ueCtxt_p = static_cast<UEContext*>(cb.getPermDataBlock());
    VERIFY_UE(cb, ueCtxt_p, "Invalid UE\n");

    SmDedDeActProcCtxt *dedBrDeActProc_p =
            dynamic_cast<SmDedDeActProcCtxt*>(cb.getTempDataBlock());

    if (dedBrDeActProc_p->getTriggerProc() == dbReq_c)
    {
        uint32_t gtpCause = 0;
        if ((dedBrDeActProc_p->getMmeErrorCause() == S1AP_FAILURE_IND)
                || (dedBrDeActProc_p->getMmeErrorCause() == NAS_ESM_FAILURE_IND))
        {
            gtpCause = GTPV2C_CAUSE_REQUEST_ACCEPTED;
        }
        else
        {
            uint8_t mmeToGtpCause = MmeCauseUtils::convertToGtpCause(
                    dedBrDeActProc_p->getMmeErrorCause());

            if (mmeToGtpCause == GTPV2C_CAUSE_CONTEXT_NOT_FOUND)
                gtpCause = mmeToGtpCause;
            else
                gtpCause = GTPV2C_CAUSE_REQUEST_ACCEPTED;
        }

        BearerStatusEMsgShPtr eMsg = std::make_shared<BearerStatusEMsg>(
                dedBrDeActProc_p->getBearerId(), gtpCause);

        SM::Event evt(DED_DEACT_COMPLETE, eMsg);
        cb.qInternalEvent(evt);

        SessionContext *sessionCtxt_p = ueCtxt_p->findSessionContextByLinkedBearerId(
    	        	    		dedBrDeActProc_p->getLinkedBearerId());
	if(sessionCtxt_p)
	{
            BearerContext *bearerCtxt_p = MmeContextManagerUtils::findBearerContext(
                    dedBrDeActProc_p->getBearerId(), ueCtxt_p, sessionCtxt_p);
            if(bearerCtxt_p)
            {
                MmeContextManagerUtils::deallocateBearerContext(cb,
                                        bearerCtxt_p, sessionCtxt_p, ueCtxt_p);
            }
	}
        MmeContextManagerUtils::deallocateProcedureCtxt(cb, dedBrDeActProc_p);
    }
    mmeStats::Instance()->increment(
            mmeStatsCounter::MME_PROCEDURES_DED_BEARER_DEACTIVATION_PROC_FAILURE);

    log_msg(LOG_DEBUG, "abort_ded_deactivation : Exit\n");
    return ActStatus::PROCEED;
}

/***************************************
 * Action handler : ded_deact_complete
 ***************************************/
ActStatus ActionHandlers::ded_deact_complete(ControlBlock &cb)
{
    log_msg(LOG_DEBUG, "ded_deact_complete : Entry\n");

    UEContext *ueCtxt_p = static_cast<UEContext*>(cb.getPermDataBlock());
    VERIFY_UE(cb, ueCtxt_p, "Invalid UE\n");

    SmDedDeActProcCtxt *dedBrDeActProc_p =
            dynamic_cast<SmDedDeActProcCtxt*>(cb.getTempDataBlock());

    if (dedBrDeActProc_p)
    {
        if (dedBrDeActProc_p->getTriggerProc() == dbReq_c)
        {
            BearerStatusEMsgShPtr eMsg = std::make_shared<BearerStatusEMsg>(
                    dedBrDeActProc_p->getBearerId(),
                    GTPV2C_CAUSE_REQUEST_ACCEPTED);

            SM::Event evt(DED_DEACT_COMPLETE, eMsg);
            cb.qInternalEvent(evt);
        }

	SessionContext *sessionCtxt_p = ueCtxt_p->findSessionContextByLinkedBearerId(
                                        dedBrDeActProc_p->getLinkedBearerId());

        if(sessionCtxt_p)
        {
            BearerContext *bearerCtxt_p = MmeContextManagerUtils::findBearerContext(
                    dedBrDeActProc_p->getBearerId(), ueCtxt_p, sessionCtxt_p);
            if(bearerCtxt_p)
            {
                MmeContextManagerUtils::deallocateBearerContext(cb,
                                        bearerCtxt_p, sessionCtxt_p, ueCtxt_p);
            }
        }

        MmeContextManagerUtils::deallocateProcedureCtxt(cb, dedBrDeActProc_p);
    }

    mmeStats::Instance()->increment(
            mmeStatsCounter::MME_PROCEDURES_DED_BEARER_DEACTIVATION_PROC_SUCCESS);

    log_msg(LOG_DEBUG, "ded_deact_complete : Entry\n");
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : send_erab_rel_cmd_and_deact_eps_br_ctxt_req
***************************************/
ActStatus ActionHandlers::send_erab_rel_cmd_and_deact_eps_br_ctxt_req(ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "send_erab_rel_cmd_and_deact_eps_br_ctxt_req : Entry\n");

	UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ue_ctxt, "Invalid UE\n");

	SmDedDeActProcCtxt *dedBrDeActProc_p =
	        dynamic_cast<SmDedDeActProcCtxt*>(cb.getTempDataBlock());
	VERIFY(dedBrDeActProc_p, return ActStatus::ABORT,
	            "Delete Bearer Procedure Context is NULL \n");

	erab_release_command_Q_msg erab_rel_cmd;
	memset(&erab_rel_cmd, 0, sizeof(erab_release_command_Q_msg));

	ActStatus actStatus = ActStatus::PROCEED;

	BearerContext *bearerCtxt_p =
			MmeContextManagerUtils::findBearerContext(
					dedBrDeActProc_p->getBearerId(), ue_ctxt);
	if (bearerCtxt_p != NULL)
	{
		bool rc = MmeS1MsgUtils::populateErabRelAndDeActDedBrReq(cb,
					*ue_ctxt, *bearerCtxt_p, erab_rel_cmd);

		if (rc)
		{
			mmeStats::Instance()->increment(
					mmeStatsCounter::MME_MSG_TX_S1AP_ERAB_RELEASE_COMMAND);
			mmeStats::Instance()->increment(
					mmeStatsCounter::MME_MSG_TX_NAS_DEACT_EPS_BR_CTXT_REQUEST);

			cmn::ipc::IpcAddress destAddr =
			{ TipcServiceInstance::s1apAppInstanceNum_c };

			FIND_COMPONENT(MmeIpcInterface,MmeIpcInterfaceCompId).dispatchIpcMsg(
					(char*) &erab_rel_cmd, sizeof(erab_rel_cmd), destAddr);
		}
		else
		{
			log_msg(LOG_INFO, "send_del_and_deact_bearer_req : "
					"Failed to populate eRAB Release Command\n");
			actStatus = ActStatus::ABORT;
		}
	}
	else
	{
		dedBrDeActProc_p->setMmeErrorCause(BEARER_CONTEXT_NOT_FOUND);
	}

	log_msg(LOG_DEBUG, "send_erab_rel_cmd_and_deact_eps_br_ctxt_req : Exit\n");

    return actStatus;
}

