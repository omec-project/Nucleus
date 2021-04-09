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
#include "mmeNasUtils.h"
#include <mmeSmDefs.h>
#include <msgBuffer.h>
#include "msgType.h"
#include "procedureStats.h"
#include "state.h"
#include <stateMachineEngine.h>
#include <tipcTypes.h>
#include <utils/mmeCauseUtils.h>
#include <utils/mmeGtpMsgUtils.h>
#include <utils/mmeContextManagerUtils.h>
#include "mmeStatsPromClient.h"
#include <memory>
#include <structs.h>

using namespace mme;
using namespace SM;
using namespace cmn;
using namespace cmn::utils;

/***************************************
 * Action handler : init_ded_bearer_deactivation
 ***************************************/
ActStatus ActionHandlers::init_ded_bearer_deactivation(ControlBlock &cb)
{
    log_msg(LOG_DEBUG, "init_ded_bearer_deactivation : Entry");

    UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
    VERIFY_UE(cb, ue_ctxt, "Invalid UE");

    MmeSmDeleteBearerProcCtxt *dbReqProc_p =
            dynamic_cast<MmeSmDeleteBearerProcCtxt*>(cb.getTempDataBlock());
    VERIFY(dbReqProc_p, return ActStatus::ABORT,
            " Delete Bearer Procedure Context is NULL ");

    const cmn::IpcEventMessage *eMsg =
            dynamic_cast<const cmn::IpcEventMessage*>(dbReqProc_p->getDeleteBearerReqEMsgRaw());
    cmn::IpcEventMessage *ipcMsg = const_cast<cmn::IpcEventMessage*>(eMsg);
    VERIFY(ipcMsg, return ActStatus::ABORT,
            "Invalid IPC Event Message in DBReq Procedure Context ");

    MsgBuffer *msgBuf = static_cast<MsgBuffer*>(ipcMsg->getMsgBuffer());
    VERIFY(msgBuf, return ActStatus::ABORT, "Invalid message buffer ");

    const db_req_Q_msg *db_req =
            static_cast<const db_req_Q_msg*>(msgBuf->getDataPointer());
    VERIFY(db_req, return ActStatus::ABORT, "Invalid data buffer ");

    uint8_t bearerDeAllocCount = 0;
    for (int i = 0; i < db_req->eps_bearer_ids_count; i++)
    {
    	BearerCtxtDBResp brCtxtDbResp;

    	brCtxtDbResp.eps_bearer_id = db_req->eps_bearer_ids[i];
    	brCtxtDbResp.cause.cause = GTPV2C_NO_CAUSE;

        BearerContext *bearerCtxt_p = MmeContextManagerUtils::findBearerContext(
                db_req->eps_bearer_ids[i], ue_ctxt);

        if (bearerCtxt_p)
        {
            // allocate dedicated bearer activation procedure context
            SmDedDeActProcCtxt *dedBrDeActProc_p =
                    MmeContextManagerUtils::allocateDedBrDeActivationProcedureCtxt(
                            cb, bearerCtxt_p->getBearerId());

            if (dedBrDeActProc_p)
            {
            	dedBrDeActProc_p->setLinkedBearerId(dbReqProc_p->getBearerId());
                dedBrDeActProc_p->setTriggerProc(dbReq_c);

                bearerDeAllocCount++;

            }
            else
            {
                /*
                 * Failed to allocate the Procedure Context, yet
                 * we are deleting the bearers, since it is a
                 * PGW Init message. So, set cause to REQUEST_ACCEPTED.
                 */
            	brCtxtDbResp.cause.cause = GTPV2C_CAUSE_REQUEST_ACCEPTED;
		
		SessionContext *sessionCtxt_p = ue_ctxt->findSessionContextByLinkedBearerId(
                                        dbReqProc_p->getBearerId());
		if(sessionCtxt_p)
		{
                    MmeContextManagerUtils::deallocateBearerContext(cb,
                        bearerCtxt_p, sessionCtxt_p, ue_ctxt);
		}
            }
        }
        else
        {
            log_msg(LOG_ERROR,
                    "Failed to find Bearer Context with bearer id: %d ",
                    db_req->eps_bearer_ids[i]);

            brCtxtDbResp.cause.cause = GTPV2C_CAUSE_CONTEXT_NOT_FOUND;
        }
        dbReqProc_p->addBearerStatus(brCtxtDbResp);
    }

    ActStatus actStatus = PROCEED;
    if (bearerDeAllocCount > 0)
    {
        SM::Event evt(DED_BEARER_DEACT_START, NULL);
        cb.qInternalEvent(evt);
    }
    else
    {
        actStatus = ABORT;
    }

    log_msg(LOG_DEBUG, "init_ded_bearer_deactivation : Exit");
    return actStatus;
}

/***************************************
 * Action handler : init_ue_detach_procedure
 ***************************************/
ActStatus ActionHandlers::init_ue_detach_procedure(ControlBlock &cb)
{
    // Start PGW_INIT Detach procedure
    MmeDetachProcedureCtxt *prcdCtxt_p =
            MmeContextManagerUtils::allocateDetachProcedureCtxt(cb,
                    pgwInitDetach_c);
    if(prcdCtxt_p != NULL)
    {
        /*
         * Is the Detach Type "reattachRequired" correct
         * or should we set it based on the cause in DB Req?
         */
        prcdCtxt_p->setNasDetachType(reattachRequired);
        SM::Event evt(PGW_INIT_DETACH, NULL);
        cb.qInternalEvent(evt);
    }
    else
    {
        log_msg(LOG_DEBUG, "Detach Procedure Context is NULL ");
        SM::Event evt(DETACH_FAILURE, NULL);
        cb.qInternalEvent(evt);
    }

    return ActStatus::PROCEED;
}

/***************************************
 * Action handler : send_delete_bearer_response
 ***************************************/
ActStatus ActionHandlers::send_delete_bearer_response(ControlBlock &cb)
{
    log_msg(LOG_DEBUG, "send_delete_bearer_response : Entry");

    MmeSmDeleteBearerProcCtxt *dbReqProc_p =
            dynamic_cast<MmeSmDeleteBearerProcCtxt*>(cb.getTempDataBlock());

    if (dbReqProc_p != NULL)
    {
        struct DB_RESP_Q_msg db_resp;
        memset(&db_resp, 0, sizeof(db_resp));

        bool rc = MmeGtpMsgUtils::populateDeleteBearerResponse(cb, *dbReqProc_p,
                db_resp);
        if (rc)
        {
            cmn::ipc::IpcAddress destAddr =
            { TipcServiceInstance::s11AppInstanceNum_c };

            mmeStats::Instance()->increment(
                    mmeStatsCounter::MME_MSG_TX_S11_DELETE_BEARER_RESPONSE);

            FIND_COMPONENT(MmeIpcInterface,MmeIpcInterfaceCompId).dispatchIpcMsg(
                    (char*) &db_resp, sizeof(db_resp), destAddr);
        }
    }

    log_msg(LOG_DEBUG, "send_delete_bearer_response : Exit");
    return ActStatus::PROCEED;
}

/***************************************
 * Action handler : abort_delete_bearer_procedure
 ***************************************/
ActStatus ActionHandlers::abort_delete_bearer_procedure(ControlBlock &cb)
{
    log_msg(LOG_DEBUG, "abort_delete_bearer_procedure : Entry");

    UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
    VERIFY_UE(cb, ue_ctxt, "Invalid UE");

    MmeSmDeleteBearerProcCtxt *dbReqProc_p =
            dynamic_cast<MmeSmDeleteBearerProcCtxt*>(cb.getTempDataBlock());
    if (dbReqProc_p != NULL)
    {
    	SessionContext *sessionCtxt_p = ue_ctxt->findSessionContextByLinkedBearerId(
    	        	    		dbReqProc_p->getBearerId());

    	if (dbReqProc_p->getLbiPresent())
    	{
    	    MmeContextManagerUtils::deallocateSessionContext(cb,
                        sessionCtxt_p, ue_ctxt);
    	}

        auto &bearerStatusContainer = dbReqProc_p->getBearerStatusContainer();

        if(bearerStatusContainer.size() > 0)
        {
        	for (auto &entry : bearerStatusContainer)
            {
                BearerContext *bearerCtxt_p =
                        MmeContextManagerUtils::findBearerContext(
                                entry.eps_bearer_id, ue_ctxt, sessionCtxt_p);

                /*TBD: Shall we deallocate the bearer context even in case of abort?
                  Is this handling correct? */
                if (bearerCtxt_p)
                {
                    MmeContextManagerUtils::deallocateBearerContext(cb,
                            bearerCtxt_p, sessionCtxt_p, ue_ctxt);
                }

                SmDedDeActProcCtxt *dedDeActProc_p =
                            dynamic_cast<SmDedDeActProcCtxt*>(MmeContextManagerUtils::findProcedureCtxt(
                                cb, dedBrDeActivation_c,
                                entry.eps_bearer_id));

                if (dedDeActProc_p)
                {
                    MmeContextManagerUtils::deallocateProcedureCtxt(cb,
                            dedDeActProc_p);
                }

            }
        }

        MmeContextManagerUtils::deallocateProcedureCtxt(cb, dbReqProc_p);

        mmeStats::Instance()->increment(
                mmeStatsCounter::MME_PROCEDURES_DELETE_BEARER_PROC_FAILURE);
    }

    log_msg(LOG_DEBUG, "abort_delete_bearer_procedure : Exit");
    return ActStatus::PROCEED;
}

/***************************************
 * Action handler : check_and_init_deactivation
 ***************************************/
ActStatus ActionHandlers::check_and_init_deactivation(ControlBlock &cb)
{
    log_msg(LOG_DEBUG, "check_and_init_deactivation : Entry");

    UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
    VERIFY_UE(cb, ue_ctxt, "Invalid UE");

    MmeSmDeleteBearerProcCtxt *dbReqProc_p =
            dynamic_cast<MmeSmDeleteBearerProcCtxt*>(cb.getTempDataBlock());
    if (dbReqProc_p != NULL)
    {
        if (dbReqProc_p->getLbiPresent())
        {
            SessionContext *sessionCtxt =
                    ue_ctxt->findSessionContextByLinkedBearerId(
                            dbReqProc_p->getBearerId());
            VERIFY(sessionCtxt,
                    dbReqProc_p->setMmeErrorCause(SESSION_CONTEXT_NOT_FOUND); return ActStatus::ABORT,
                    "Session Context is NULL ");
            if (ue_ctxt->getSessionContextContainer().size() == 1)
            {
                SM::Event evt(START_UE_DETACH, NULL);
                cb.qInternalEvent(evt);
            }
        }
        else
        {
            SM::Event evt(START_DED_DEACTIVATION, NULL);
            cb.qInternalEvent(evt);
        }
    }

    log_msg(LOG_DEBUG, "check_and_init_deactivation : Exit");
    return ActStatus::PROCEED;
}

/***************************************
 * Action handler : handle_ded_deact_cmp_ind
 ***************************************/
ActStatus ActionHandlers::handle_ded_deact_cmp_ind(ControlBlock &cb)
{
    log_msg(LOG_DEBUG, "handle_ded_deact_cmp_ind : Entry");

    MmeSmDeleteBearerProcCtxt *dbReqProc_p =
            dynamic_cast<MmeSmDeleteBearerProcCtxt*>(cb.getTempDataBlock());
    VERIFY(dbReqProc_p, return ActStatus::ABORT,
            "Delete Bearer Procedure Context is NULL ");

    ActStatus actStatus = ActStatus::PROCEED;

    BearerStatusEMsgShPtr eMsg = std::dynamic_pointer_cast<BearerStatusEMsg>(
            cb.getEventMessage());

    if (eMsg)
    {
        auto &bearerStatusContainer = dbReqProc_p->getBearerStatusContainer();
        uint8_t completedDeActs = 0;
        for (auto &entry : bearerStatusContainer)
        {
            if (entry.eps_bearer_id
                    == eMsg->getBearerId())
            {
                entry.cause.cause = eMsg->getCause();
            }

            if (entry.cause.cause != GTPV2C_NO_CAUSE)
            {
                completedDeActs++;
            }
        }

        // We haven't received completion indication for all bearers.
        // Wait for some time.
        if (completedDeActs < bearerStatusContainer.size())
        {
            actStatus = ActStatus::BREAK;
        }
    }

    log_msg(LOG_DEBUG, "handle_ded_deact_cmp_ind : Exit");
    return actStatus;
}

/***************************************
 * Action handler : delete_bearer_proc_complete
 ***************************************/
ActStatus ActionHandlers::delete_bearer_proc_complete(ControlBlock &cb)
{
    log_msg(LOG_DEBUG, "delete_bearer_proc_complete : Entry");

    UEContext *ueCtxt_p = static_cast<UEContext*>(cb.getPermDataBlock());
    if(ueCtxt_p != NULL)
    {
        MmeSmDeleteBearerProcCtxt *dbReqProc_p =
                dynamic_cast<MmeSmDeleteBearerProcCtxt*>(cb.getTempDataBlock());
        if (dbReqProc_p)
        {
            SessionContext *sessionCtxt_p = ueCtxt_p->findSessionContextByLinkedBearerId(
                    dbReqProc_p->getBearerId());

            if(dbReqProc_p->getLbiPresent())
            {
                MmeContextManagerUtils::deallocateSessionContext(cb,
                            sessionCtxt_p, ueCtxt_p);
            }

            MmeContextManagerUtils::deallocateProcedureCtxt(cb, dbReqProc_p);

            mmeStats::Instance()->increment(
                    mmeStatsCounter::MME_PROCEDURES_DELETE_BEARER_PROC_SUCCESS);
        }
    }

    log_msg(LOG_DEBUG, "delete_bearer_proc_complete : Exit");
    return ActStatus::PROCEED;
}

