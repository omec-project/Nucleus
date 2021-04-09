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
#include "mmeStatsPromClient.h"

using namespace mme;
using namespace SM;
using namespace cmn;
using namespace cmn::utils;

/***************************************
 * Action handler : process_erab_setup_response
 ***************************************/
ActStatus ActionHandlers::process_erab_setup_response(ControlBlock &cb)
{
    log_msg(LOG_DEBUG, "process_erab_setup_response : Entry");

    UEContext *ueCtxt_p = static_cast<UEContext*>(cb.getPermDataBlock());
    VERIFY_UE(cb, ueCtxt_p, "Invalid UE");

    SmDedActProcCtxt *dedBrActProc_p =
            dynamic_cast<SmDedActProcCtxt*>(cb.getTempDataBlock());
    VERIFY(dedBrActProc_p, return ActStatus::ABORT,
            "Ded Bearer Activation Procedure Context is NULL ");

    MsgBuffer *msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
    VERIFY(msgBuf, dedBrActProc_p->setMmeErrorCause(INVALID_MSG_BUFFER);
    	return ActStatus::ABORT, "process_erab_setup_response : Invalid message buffer ");

    const erabSuResp_Q_msg_t *erab_su_resp =
                static_cast<const erabSuResp_Q_msg_t*>(msgBuf->getDataPointer());
    VERIFY(erab_su_resp, dedBrActProc_p->setMmeErrorCause(INVALID_DATA_BUFFER);
    	return ActStatus::ABORT, "process_erab_setup_response : Invalid data buffer ");

    BearerContext *bearerCtxt_p = MmeContextManagerUtils::findBearerContext(
            dedBrActProc_p->getBearerId(), ueCtxt_p);
    VERIFY(bearerCtxt_p, dedBrActProc_p->setMmeErrorCause(BEARER_CONTEXT_NOT_FOUND);
                return ActStatus::ABORT, "Bearer Context is NULL ");

    ActStatus actStatus = ActStatus::PROCEED;
    bool bearerEntryFound = false;

    for (int i = 0; i < erab_su_resp->erab_su_list.count; i++)
    {
        if (erab_su_resp->erab_su_list.erab_su_item[i].e_RAB_ID ==
                bearerCtxt_p->getBearerId())
        {
            fteid S1uEnbUserFteid;
            S1uEnbUserFteid.header.iface_type = 0;
            S1uEnbUserFteid.header.v4 = 1;
            S1uEnbUserFteid.header.teid_gre =
                    erab_su_resp->erab_su_list.erab_su_item[i].gtp_teid;
            S1uEnbUserFteid.ip.ipv4.s_addr =
                    erab_su_resp->erab_su_list.erab_su_item[i].transportLayerAddress;

            bearerCtxt_p->setS1uEnbUserFteid(Fteid(S1uEnbUserFteid));

            bearerEntryFound = true;
            break;
        }
    }

    if (!bearerEntryFound)
    {
        actStatus = ActStatus::ABORT;
        dedBrActProc_p->setMmeErrorCause(S1AP_FAILURE_IND);

        for (int i = 0; i < erab_su_resp->erab_fail_list.count; i++)
        {
            if (erab_su_resp->erab_fail_list.erab_fail_item[i].e_RAB_ID
                    == bearerCtxt_p->getBearerId())
            {
                dedBrActProc_p->setS1apCause(
                                erab_su_resp->erab_fail_list.erab_fail_item[i].cause);
                bearerEntryFound = true;
            }
        }
    }
    return actStatus;
}

/***************************************
 * Action handler : process_act_ded_bearer_accept
 ***************************************/
ActStatus ActionHandlers::process_act_ded_bearer_accept(ControlBlock &cb)
{
    log_msg(LOG_DEBUG, "process_act_ded_bearer_accept : Entry");

    UEContext *ueCtxt_p = static_cast<UEContext*>(cb.getPermDataBlock());
    VERIFY_UE(cb, ueCtxt_p, "Invalid UE");

    SmDedActProcCtxt *dedBrActProc_p =
            dynamic_cast<SmDedActProcCtxt*>(cb.getTempDataBlock());
    VERIFY(dedBrActProc_p, return ActStatus::ABORT,
            "Ded Bearer Activation Procedure Context is NULL ");

    MsgBuffer *msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
    VERIFY(msgBuf, dedBrActProc_p->setMmeErrorCause(INVALID_MSG_BUFFER);
    	return ActStatus::ABORT, "process_act_ded_bearer_accept : Invalid message buffer ");

    const dedicatedBearerContextAccept_Q_msg_t *dedBrAcpt =
            static_cast<const dedicatedBearerContextAccept_Q_msg_t*>(msgBuf->getDataPointer());
    VERIFY(dedBrAcpt, dedBrActProc_p->setMmeErrorCause(INVALID_DATA_BUFFER);
    		return ActStatus::ABORT, "process_act_ded_bearer_accept : Invalid data buffer ");

    BearerContext *bearerCtxt_p = MmeContextManagerUtils::findBearerContext(
            dedBrActProc_p->getBearerId(), ueCtxt_p);
    VERIFY(bearerCtxt_p, dedBrActProc_p->setMmeErrorCause(BEARER_CONTEXT_NOT_FOUND);
                return ActStatus::ABORT, "Bearer Context is NULL ");

    if (dedBrAcpt->pco_opt.pco_length > 0)
    {
        if (dedBrActProc_p->getTriggerProc() == cbReq_c)
        {
            MmeSmCreateBearerProcCtxt *cbReqProc_p =
                    dynamic_cast<MmeSmCreateBearerProcCtxt*>(MmeContextManagerUtils::findProcedureCtxt(
                            cb, dedBrActProc_p->getTriggerProc()));

            auto &bsContainer = cbReqProc_p->getBearerStatusContainer();
            for (auto &entry : bsContainer)
            {
                if (entry.bearer_ctxt_cb_resp_m.eps_bearer_id == dedBrActProc_p->getBearerId())
                {
                    entry.bearer_ctxt_cb_resp_m.pco_from_ue_opt.pco_length =
                            dedBrAcpt->pco_opt.pco_length;
                    memcpy(
                            entry.bearer_ctxt_cb_resp_m.pco_from_ue_opt.pco_options,
                            dedBrAcpt->pco_opt.pco_options,
                            dedBrAcpt->pco_opt.pco_length);

                    break;
                }
            }
        }
    }

    ProcedureStats::num_of_ded_bearer_ctxt_acpt_received++;

    return ActStatus::PROCEED;
}

/***************************************
 * Action handler : process_act_ded_bearer_reject
 ***************************************/
ActStatus ActionHandlers::process_act_ded_bearer_reject(ControlBlock &cb)
{
    log_msg(LOG_DEBUG, "process_act_ded_bearer_reject : Entry");

    UEContext *ueCtxt_p = static_cast<UEContext*>(cb.getPermDataBlock());
    VERIFY_UE(cb, ueCtxt_p, "Invalid UE");

    SmDedActProcCtxt *dedBrActProc_p =
            dynamic_cast<SmDedActProcCtxt*>(cb.getTempDataBlock());
    VERIFY(dedBrActProc_p, return ActStatus::ABORT,
            "Ded Bearer Activation Procedure Context is NULL ");

    MsgBuffer *msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
    VERIFY(msgBuf, dedBrActProc_p->setMmeErrorCause(INVALID_MSG_BUFFER);
    	return ActStatus::ABORT, "process_act_ded_bearer_reject : Invalid message buffer ");

    const dedicatedBearerContextReject_Q_msg_t *dedBrReject =
                static_cast<const dedicatedBearerContextReject_Q_msg_t*>(msgBuf->getDataPointer());
    VERIFY(dedBrReject, dedBrActProc_p->setMmeErrorCause(INVALID_DATA_BUFFER);
    	return ActStatus::ABORT, "process_act_ded_bearer_reject : Invalid data buffer ");

    BearerContext *bearerCtxt_p = MmeContextManagerUtils::findBearerContext(
            dedBrActProc_p->getBearerId(), ueCtxt_p);
    VERIFY(bearerCtxt_p, dedBrActProc_p->setMmeErrorCause(BEARER_CONTEXT_NOT_FOUND);
                return ActStatus::ABORT, "Bearer Context is NULL ");

    dedBrActProc_p->setMmeErrorCause(NAS_ESM_FAILURE_IND);
    dedBrActProc_p->setEsmCause(dedBrReject->esm_cause);

    ProcedureStats::num_of_ded_bearer_ctxt_reject_received++;

    return ActStatus::PROCEED;
}

/***************************************
 * Action handler : abort_ded_activation
 ***************************************/
ActStatus ActionHandlers::abort_ded_activation(ControlBlock &cb)
{
    log_msg(LOG_DEBUG, "abort_ded_activation : Entry");

    UEContext *ueCtxt_p = static_cast<UEContext*>(cb.getPermDataBlock());
    VERIFY_UE(cb, ueCtxt_p, "Invalid UE");

    SmDedActProcCtxt *dedBrActProc_p =
            dynamic_cast<SmDedActProcCtxt*>(cb.getTempDataBlock());

    if (dedBrActProc_p)
    {
        SessionContext *sessCtxt = ueCtxt_p->findSessionContextByLinkedBearerId(
                dedBrActProc_p->getLinkedBearerId());

        if (sessCtxt != NULL)
        {
            BearerContext *bearer_p = sessCtxt->findBearerContextByBearerId(
                    dedBrActProc_p->getBearerId());
            if (bearer_p)
            {
                MmeContextManagerUtils::deallocateBearerContext(cb, bearer_p,
                        sessCtxt, ueCtxt_p);
            }
        }
        if (dedBrActProc_p->getTriggerProc() == cbReq_c)
        {
            uint32_t gtpCause = 0;
            if (dedBrActProc_p->getMmeErrorCause() == S1AP_FAILURE_IND)
            {
                gtpCause = MmeCauseUtils::convertToGtpCause(
                        dedBrActProc_p->getS1apCause().s1apCause_m);
            }
            else if (dedBrActProc_p->getMmeErrorCause() == NAS_ESM_FAILURE_IND)
            {
                gtpCause = MmeCauseUtils::convertToGtpCause(
                        dedBrActProc_p->getEsmCause());
            }
            else
            {
                gtpCause = MmeCauseUtils::convertToGtpCause(
                        dedBrActProc_p->getMmeErrorCause());
            }
            BearerStatusEMsgShPtr eMsg = std::make_shared<BearerStatusEMsg>(
                    dedBrActProc_p->getBearerId(), gtpCause);

            SM::Event evt(DED_ACT_COMPLETE, eMsg);
            cb.qInternalEvent(evt);
        }
        MmeContextManagerUtils::deallocateProcedureCtxt(cb, dedBrActProc_p);
    }
    mmeStats::Instance()->increment(
            mmeStatsCounter::MME_PROCEDURES_DED_BEARER_ACTIVATION_PROC_FAILURE);

    ProcedureStats::num_of_abort_ded_activation++;

    return ActStatus::PROCEED;
}

/***************************************
 * Action handler : ded_act_complete
 ***************************************/
ActStatus ActionHandlers::ded_act_complete(ControlBlock &cb)
{
    log_msg(LOG_DEBUG, "ded_act_complete : Entry");

    UEContext *ueCtxt_p = static_cast<UEContext*>(cb.getPermDataBlock());
    VERIFY_UE(cb, ueCtxt_p, "Invalid UE");

    SmDedActProcCtxt *dedBrActProc_p =
            dynamic_cast<SmDedActProcCtxt*>(cb.getTempDataBlock());

    if (dedBrActProc_p)
    {
        if (dedBrActProc_p->getTriggerProc() == cbReq_c)
        {
            BearerStatusEMsgShPtr eMsg = std::make_shared<BearerStatusEMsg>(
                    dedBrActProc_p->getBearerId(), GTPV2C_CAUSE_REQUEST_ACCEPTED);

            SM::Event evt(DED_ACT_COMPLETE, eMsg);
            cb.qInternalEvent(evt);
        }
        MmeContextManagerUtils::deallocateProcedureCtxt(cb, dedBrActProc_p);
    }

    ProcedureStats::num_of_ded_act_complete++;
    mmeStats::Instance()->increment(
            mmeStatsCounter::MME_PROCEDURES_DED_BEARER_ACTIVATION_PROC_SUCCESS);

    return ActStatus::PROCEED;
}

