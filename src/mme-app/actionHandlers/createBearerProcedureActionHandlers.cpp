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
#include <utils/mmeS1MsgUtils.h>
#include <utils/mmeContextManagerUtils.h>
#include "mmeStatsPromClient.h"
#include <memory>
#include <structs.h>

using namespace mme;
using namespace SM;
using namespace cmn;
using namespace cmn::utils;

/***************************************
 * Action handler : init_ded_bearer_activation
 ***************************************/
ActStatus ActionHandlers::init_ded_bearer_activation(ControlBlock &cb)
{
    log_msg(LOG_DEBUG, "init_ded_bearer_activation : Entry");

    UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
    VERIFY_UE(cb, ue_ctxt, "Invalid UE");

    MmeSmCreateBearerProcCtxt *cbReqProc_p =
            dynamic_cast<MmeSmCreateBearerProcCtxt*>(cb.getTempDataBlock());
    VERIFY(cbReqProc_p, return ActStatus::ABORT,
            " Create Bearer Procedure Context is NULL ");

    cmn::IpcEventMessage *ipcMsg =
            dynamic_cast<cmn::IpcEventMessage*>(cbReqProc_p->getCreateBearerReqEMsgRaw());
    VERIFY(ipcMsg, return ActStatus::ABORT, "Invalid IPC Event Message in CBReq Procedure Context ");

    MsgBuffer *msgBuf = static_cast<MsgBuffer*>(ipcMsg->getMsgBuffer());
    VERIFY(msgBuf, return ActStatus::ABORT, "Invalid message buffer ");

    const cb_req_Q_msg *cb_req =
            static_cast<const cb_req_Q_msg*>(msgBuf->getDataPointer());
    VERIFY(cb_req, return ActStatus::ABORT, "Invalid data buffer ");

    SessionContext *sessionCtxt = ue_ctxt->findSessionContextByLinkedBearerId(
            cb_req->linked_eps_bearer_id);
    VERIFY(sessionCtxt,
            cbReqProc_p->setMmeErrorCause(SESSION_CONTEXT_NOT_FOUND); return ActStatus::ABORT,
            "Session Context is NULL ");

    uint8_t bearerAllocCount = 0;
    for (int i = 0; i < cb_req->bearer_ctx_list.bearers_count; i++)
    {
        bearer_ctxt_cb_resp_t brStatus = { 0 };

        brStatus.cause.cause = GTPV2C_NO_CAUSE;
        brStatus.s1u_sgw_teid = cb_req->bearer_ctx_list.bearer_ctxt[i].s1u_sgw_teid;

        BearerContext *bearerCtxt_p =
                MmeContextManagerUtils::allocateBearerContext(cb, *ue_ctxt,
                        *sessionCtxt);

        if (bearerCtxt_p != NULL)
        {
            log_msg(LOG_DEBUG,
                    "Allocated Bearer Context with eps_bearer_id: %d for s1u sgw teid: %d ",
                    bearerCtxt_p->getBearerId(),
                    cb_req->bearer_ctx_list.bearer_ctxt[i].s1u_sgw_teid.header.teid_gre);

            bearerCtxt_p->setBearerQos(
                    cb_req->bearer_ctx_list.bearer_ctxt[i].bearer_qos);
            bearerCtxt_p->setBearerTft(
                    cb_req->bearer_ctx_list.bearer_ctxt[i].tft.data,
                    cb_req->bearer_ctx_list.bearer_ctxt[i].tft.len);
            bearerCtxt_p->setS1uSgwUserFteid(
                    Fteid(cb_req->bearer_ctx_list.bearer_ctxt[i].s1u_sgw_teid));

            // allocate dedicated bearer activation procedure context
            SmDedActProcCtxt *dedBrActProc_p =
                    MmeContextManagerUtils::allocateDedBrActivationProcedureCtxt(
                            cb, bearerCtxt_p->getBearerId());

            if (dedBrActProc_p)
            {
                brStatus.eps_bearer_id = bearerCtxt_p->getBearerId();
                dedBrActProc_p->setLinkedBearerId(cb_req->linked_eps_bearer_id);
                dedBrActProc_p->setTriggerProc(cbReq_c);

                bearerAllocCount++;

            }
            else
            {
                brStatus.cause.cause = GTPV2C_CAUSE_REQUEST_REJECTED;

                MmeContextManagerUtils::deallocateBearerContext(
                        cb, bearerCtxt_p, sessionCtxt, ue_ctxt);
            }
        }
        else
        {
            log_msg(LOG_ERROR,
                    "Failed to allocate Bearer Context with teid: %d ",
                    cb_req->bearer_ctx_list.bearer_ctxt[i].s1u_sgw_teid.header.teid_gre);

            brStatus.cause.cause = GTPV2C_CAUSE_REQUEST_REJECTED;
        }
        BearerCtxtCBResp brCtxtCbResp(brStatus);
        cbReqProc_p->addBearerStatus(brCtxtCbResp);
    }

    ActStatus actStatus = PROCEED;
    if (bearerAllocCount > 0)
    {
        SM::Event evt(GW_INIT_DED_BEARER_AND_SESSION_SETUP, NULL);
        cb.qInternalEvent(evt);
    }
    else
    {
        actStatus = ABORT;
    }
    return actStatus;
}

/***************************************
 * Action handler : send_bearer_setup_and_sess_mgmt_req
 ***************************************/
ActStatus ActionHandlers::send_bearer_setup_and_sess_mgmt_req(ControlBlock &cb)
{
    log_msg(LOG_DEBUG, "send_bearer_setup_and_sess_mgmt_req : Entry");

    UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
    VERIFY_UE(cb, ue_ctxt, "Invalid UE");

    MmeSmCreateBearerProcCtxt *cbReqProc_p = 
            dynamic_cast<MmeSmCreateBearerProcCtxt*>(cb.getTempDataBlock());
    VERIFY(cbReqProc_p, return ActStatus::ABORT,
            "Create Bearer Procedure Context is NULL ");

    erabsu_ctx_req_Q_msg erab_su_req;
    memset(&erab_su_req, 0, sizeof(erabsu_ctx_req_Q_msg));

    ActStatus actStatus = ActStatus::PROCEED;

    bool rc = MmeS1MsgUtils::populateErabSetupAndActDedBrReq(cb, *ue_ctxt,
            *cbReqProc_p, erab_su_req);

    if (rc)
    {
        mmeStats::Instance()->increment(
                mmeStatsCounter::MME_MSG_TX_S1AP_ERAB_SETUP_REQUEST);

        cmn::ipc::IpcAddress destAddr =
        { TipcServiceInstance::s1apAppInstanceNum_c };

        FIND_COMPONENT(MmeIpcInterface,MmeIpcInterfaceCompId).dispatchIpcMsg(
                (char*) &erab_su_req, sizeof(erab_su_req),
                destAddr);

        ProcedureStats::num_of_erab_setup_req_sent++;
        ProcedureStats::num_of_act_ded_br_ctxt_req_sent +=
                erab_su_req.erab_su_list.count;
    }
    else
    {
        log_msg(LOG_INFO, "send_bearer_setup_and_sess_mgmt_req : "
                "Failed to populate eRAB Setup Request");
        actStatus = ActStatus::ABORT;
    }
    return actStatus;
}

/***************************************
 * Action handler : send_create_bearer_response
 ***************************************/
ActStatus ActionHandlers::send_create_bearer_response(ControlBlock &cb)
{
    MmeSmCreateBearerProcCtxt *cbReqProc_p =
            dynamic_cast<MmeSmCreateBearerProcCtxt*>(cb.getTempDataBlock());

    if (cbReqProc_p != NULL)
    {
        struct CB_RESP_Q_msg cb_resp;
        memset(&cb_resp, 0, sizeof(cb_resp));

        bool rc = MmeGtpMsgUtils::populateCreateBearerResponse(cb, *cbReqProc_p,
                cb_resp);
        if (rc)
        {
            cmn::ipc::IpcAddress destAddr =
            { TipcServiceInstance::s11AppInstanceNum_c };

            mmeStats::Instance()->increment(
                    mmeStatsCounter::MME_MSG_TX_S11_CREATE_BEARER_RESPONSE);

            FIND_COMPONENT(MmeIpcInterface,MmeIpcInterfaceCompId).dispatchIpcMsg(
                    (char*) &cb_resp, sizeof(cb_resp), destAddr);

            ProcedureStats::num_of_create_bearer_resp_sent++;
        }
    }
    return ActStatus::PROCEED;
}

/***************************************
 * Action handler : handle_ded_act_cmp_ind
 ***************************************/
ActStatus ActionHandlers::handle_ded_act_cmp_ind(ControlBlock &cb)
{
    log_msg(LOG_DEBUG, "handle_ded_act_cmp_ind : Entry");

    MmeSmCreateBearerProcCtxt *cbReqProc_p = 
            dynamic_cast<MmeSmCreateBearerProcCtxt*>(cb.getTempDataBlock());
    VERIFY(cbReqProc_p, return ActStatus::ABORT,
            "Create Bearer Procedure Context is NULL ");

    ActStatus actStatus = ActStatus::PROCEED;

    BearerStatusEMsgShPtr eMsg = std::dynamic_pointer_cast<BearerStatusEMsg>(
            cb.getEventMessage());

    if (eMsg)
    {
        auto &bearerStatusContainer = cbReqProc_p->getBearerStatusContainer();
        uint8_t completedActs = 0;
        for (auto &entry: bearerStatusContainer)
        {
            if (entry.bearer_ctxt_cb_resp_m.eps_bearer_id == eMsg->getBearerId())
            {
                entry.bearer_ctxt_cb_resp_m.cause.cause = eMsg->getCause();
            }

            if (entry.bearer_ctxt_cb_resp_m.cause.cause != GTPV2C_NO_CAUSE)
            {
                completedActs++;
            }
        }

        // We haven't received completion indication for all bearers.
        // Wait for some time.
        if (completedActs < bearerStatusContainer.size())
        {
            actStatus = ActStatus::BREAK;
        }
    }

    return actStatus;
}

/***************************************
 * Action handler : abort_create_bearer_procedure
 ***************************************/
ActStatus ActionHandlers::abort_create_bearer_procedure(ControlBlock &cb)
{
    log_msg(LOG_DEBUG, "abort_create_bearer_procedure : Entry");

    UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
    VERIFY_UE(cb, ue_ctxt, "Invalid UE");

    MmeSmCreateBearerProcCtxt *cbReqProc_p =
            dynamic_cast<MmeSmCreateBearerProcCtxt*>(cb.getTempDataBlock());
    if (cbReqProc_p != NULL)
    {
	SessionContext *sessionCtxt = ue_ctxt->findSessionContextByLinkedBearerId(
            cbReqProc_p->getBearerId());

	auto &bearerStatusContainer = cbReqProc_p->getBearerStatusContainer();

	if(sessionCtxt)
	{
            for (auto &entry: bearerStatusContainer)
            {
                BearerContext *bearerCtxt_p = MmeContextManagerUtils::findBearerContext(
            			entry.bearer_ctxt_cb_resp_m.eps_bearer_id, ue_ctxt, sessionCtxt);

	        if(bearerCtxt_p)
	        {
		    MmeContextManagerUtils::deallocateBearerContext(cb, bearerCtxt_p,
				sessionCtxt, ue_ctxt);
	        }

	        SmDedActProcCtxt *dedActProc_p = dynamic_cast<SmDedActProcCtxt*>
		    (MmeContextManagerUtils::findProcedureCtxt(cb, dedBrActivation_c, 
				    entry.bearer_ctxt_cb_resp_m.eps_bearer_id));

	        if(dedActProc_p)
	        {
		    MmeContextManagerUtils::deallocateProcedureCtxt(cb, dedActProc_p);
	        }

	    }
	}

        MmeContextManagerUtils::deallocateProcedureCtxt(cb, cbReqProc_p);

        mmeStats::Instance()->increment(
                mmeStatsCounter::MME_PROCEDURES_CREATE_BEARER_PROC_FAILURE);
    }

    return ActStatus::PROCEED;
}

/***************************************
 * Action handler : create_bearer_proc_complete
 ***************************************/
ActStatus ActionHandlers::create_bearer_proc_complete(ControlBlock &cb)
{
    log_msg(LOG_DEBUG, "create_bearer_proc_complete : Entry");

    MmeSmCreateBearerProcCtxt *cbReqProc_p =
            dynamic_cast<MmeSmCreateBearerProcCtxt*>(cb.getTempDataBlock());
    if (cbReqProc_p)
    {
        MmeContextManagerUtils::deallocateProcedureCtxt(cb, cbReqProc_p);

        mmeStats::Instance()->increment(
                mmeStatsCounter::MME_PROCEDURES_CREATE_BEARER_PROC_SUCCESS);
    }

    ProcedureStats::num_of_create_bearer_proc_complete++;

    return ActStatus::PROCEED;
}

