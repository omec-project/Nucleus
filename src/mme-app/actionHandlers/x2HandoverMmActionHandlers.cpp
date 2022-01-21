/*
 * Copyright 2021-present Infosys Limited
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
#include "controlBlock.h"
#include "msgType.h"
#include "mme_app.h"
#include "procedureStats.h"
#include "log.h"
#include "secUtils.h"
#include "state.h"
#include <mmeSmDefs.h>
#include "common_proc_info.h"
#include <gtpCauseTypes.h>
#include <ipcTypes.h>
#include <tipcTypes.h>
#include <msgBuffer.h>
#include <interfaces/mmeIpcInterface.h>
#include <event.h>
#include <stateMachineEngine.h>
#include <utils/mmeCommonUtils.h>
#include <utils/mmeS1MsgUtils.h>
#include <utils/mmeGtpMsgUtils.h>
#include <utils/mmeCauseUtils.h>
#include <contextManager/dataBlocks.h>
#include <utils/mmeContextManagerUtils.h>
#include "mmeStatsPromClient.h"

using namespace cmn;
using namespace cmn::utils;
using namespace mme;
using namespace SM;

/***************************************
* Action handler : init_x2_ho
***************************************/
ActStatus ActionHandlers::init_x2_ho(ControlBlock& cb)
{
    log_msg(LOG_DEBUG, "init_x2_ho : Entry");

    UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
    VERIFY_UE(cb, ue_ctxt, "Invalid UE");

    X2HOMmProcedureContext *x2HoMmProc_p =
            dynamic_cast<X2HOMmProcedureContext*>(cb.getTempDataBlock());
    VERIFY(x2HoMmProc_p, return ActStatus::ABORT,
            "X2 HO MM Procedure Context is NULL ");

    auto &sessionCtxtContainer = ue_ctxt->getSessionContextContainer();
    VERIFY(sessionCtxtContainer.size() > 0,
            x2HoMmProc_p->setMmeErrorCause(SESSION_CONTAINER_EMPTY); return ActStatus::ABORT,
            "Sessions Container is empty");

    for (auto &sessCtxt : sessionCtxtContainer)
    {
        if (sessCtxt)
        {
            X2HOSmProcedureContext *x2HoSmProc_p =
                    MmeContextManagerUtils::allocateX2HoSmContext(cb,
                            sessCtxt->getLinkedBearerId());
            if (x2HoSmProc_p)
            {
                PdnHoStatus hoStatus(x2HoSmProc_p->getBearerId(), inProgress_c);
                x2HoMmProc_p->addBearerStatus(hoStatus);
                x2HoSmProc_p->setPathSwitchReqMsg(x2HoMmProc_p->getPathSwitchReqEMsg());
            }
            else
            {
                x2HoMmProc_p->setMmeErrorCause(X2HO_START_FAILURE);
                return ActStatus::ABORT;
            }
        }
    }

    x2HoMmProc_p->resetPathSwitchReqEMsgPtr();

    // No SGW Reloc case
    SM::Event evt(START_X2_HO_SM, NULL);
    cb.qInternalEvent(evt);

    return ActStatus::PROCEED;
}

/***************************************
 * Action handler : handle_x2_ho_sm_cmp_ind
 ***************************************/
ActStatus ActionHandlers::handle_x2_ho_sm_cmp_ind(ControlBlock &cb)
{
    log_msg(LOG_DEBUG, "handle_x2_ho_sm_cmp_ind : Entry");

    X2HOMmProcedureContext *proc_p =
            dynamic_cast<X2HOMmProcedureContext*>(cb.getTempDataBlock());
    VERIFY(proc_p, return ActStatus::ABORT,
            "handle_x2_ho_sm_cmp_ind : Procedure Context is NULL ");

    ActStatus actStatus = ActStatus::PROCEED;

    uint8_t successCount = 0;
    uint8_t pdnHoPendingCount = 0;

    PdnHoStatusEMsgShPtr eMsg =
            std::dynamic_pointer_cast<PdnHoStatusEMsg>(cb.getEventMessage());

    if (eMsg)
    {
        auto &pdnHoStatusContainer = proc_p->getBearerStatusContainer();

        for (auto &pdnHoStatus : pdnHoStatusContainer)
        {
            if (pdnHoStatus.def_bearer_id == eMsg->pdnHoStatus.def_bearer_id)
            {
                pdnHoStatus.failed_br_count = eMsg->pdnHoStatus.failed_br_count;
                if (pdnHoStatus.failed_br_count)
                {
                    memcpy(pdnHoStatus.failed_br_list, eMsg->pdnHoStatus.failed_br_list,
                            pdnHoStatus.failed_br_count * sizeof(erab_list_item));
                }

                pdnHoStatus.ho_status = eMsg->pdnHoStatus.ho_status;
            }

            if (pdnHoStatus.ho_status == inProgress_c)
            {
                pdnHoPendingCount++;
            }
            else if (pdnHoStatus.ho_status == success_c)
            {
                successCount++;
            }
        }
    }

    if (pdnHoPendingCount)
    {
        actStatus = ActStatus::BREAK;
    }
    else if (!successCount)
    {
        proc_p->setMmeErrorCause(X2HO_FAILURE);
        actStatus = ActStatus::ABORT;
    }

    return actStatus;
}

/***************************************
 * Action handler : send_path_switch_ack_to_enb
 ***************************************/
ActStatus ActionHandlers::send_path_switch_ack_to_enb(ControlBlock &cb)
{
    log_msg(LOG_DEBUG, "send_path_switch_ack_to_enb : Entry ");

    UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
    VERIFY_UE(cb, ue_ctxt, "Invalid UE");

    X2HOMmProcedureContext *proc_p =
            dynamic_cast<X2HOMmProcedureContext*>(cb.getTempDataBlock());
    VERIFY(proc_p, return ActStatus::ABORT,
            "send_path_switch_ack_to_enb : Procedure Context is NULL ");

    ActStatus actStatus = ActStatus::PROCEED;

    struct path_switch_req_ack_Q_msg path_sw_ack;
    memset(&path_sw_ack, 0, sizeof(path_switch_req_ack_Q_msg));

    bool rc = MmeS1MsgUtils::populatePathSwitchReqAck(cb, *ue_ctxt, *proc_p,
            path_sw_ack);

    if (rc)
    {
        mmeStats::Instance()->increment(
                mmeStatsCounter::MME_MSG_TX_S1AP_PATH_SWITCH_REQUEST_ACK);

        cmn::ipc::IpcAddress destAddr =
        { TipcServiceInstance::s1apAppInstanceNum_c };

        FIND_COMPONENT(MmeIpcInterface,MmeIpcInterfaceCompId).dispatchIpcMsg(
                (char*) &path_sw_ack, sizeof(path_sw_ack),
                destAddr);
    }
    else
    {
        log_msg(LOG_INFO, "send_path_switch_ack_to_enb : "
                "Failed to populate Path Switch Request Ack");

        proc_p->setMmeErrorCause(X2HO_PSWACK_SEND_FAILURE);
        actStatus = ActStatus::ABORT;
    }

    return actStatus;
}

/***************************************
 * Action handler : send_path_switch_fail_to_enb
 ***************************************/
ActStatus ActionHandlers::send_path_switch_fail_to_enb(ControlBlock &cb)
{
    log_msg(LOG_DEBUG, "send_path_switch_fail_to_enb : Entry ");

    UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
    VERIFY_UE(cb, ue_ctxt, "Invalid UE");

    struct path_switch_req_fail_Q_msg path_sw_fail;
    memset(&path_sw_fail, 0, sizeof(path_switch_req_fail_Q_msg));

    X2HOMmProcedureContext *proc_p =
            dynamic_cast<X2HOMmProcedureContext*>(cb.getTempDataBlock());
    if (proc_p)
    {
        path_sw_fail.msg_type = path_switch_request_fail;
        path_sw_fail.mme_ue_s1ap_id = ue_ctxt->getContextID();
        path_sw_fail.enb_context_id = ue_ctxt->getEnbFd();
        path_sw_fail.enb_s1ap_ue_id = ue_ctxt->getS1apEnbUeId();
        path_sw_fail.cause = (MmeCauseUtils::convertToS1apCause(
                proc_p->getMmeErrorCause())).s1apCause_m;

        mmeStats::Instance()->increment(
                mmeStatsCounter::MME_MSG_TX_S1AP_PATH_SWITCH_REQUEST_FAIL);

        cmn::ipc::IpcAddress destAddr =
        { TipcServiceInstance::s1apAppInstanceNum_c };

        FIND_COMPONENT(MmeIpcInterface,MmeIpcInterfaceCompId).dispatchIpcMsg(
                (char*) &path_sw_fail, sizeof(path_sw_fail),
                destAddr);
    }

    return ActStatus::PROCEED;
}

/***************************************
 * Action handler : abort_x2_ho_mm
 ***************************************/
ActStatus ActionHandlers::abort_x2_ho_mm(ControlBlock &cb)
{
    log_msg(LOG_DEBUG, "abort_x2_ho_mm : Entry ");

    UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
    VERIFY_UE(cb, ue_ctxt, "Invalid UE");

    mmeStats::Instance()->increment(
            mmeStatsCounter::MME_PROCEDURES_X2_ENB_HANDOVER_PROC_FAILURE);

    ERROR_CODES errorCause = SUCCESS;

    X2HOMmProcedureContext *proc_p =
            dynamic_cast<X2HOMmProcedureContext*>(cb.getTempDataBlock());
    if (proc_p)
    {
        errorCause = proc_p->getMmeErrorCause();
        MmeContextManagerUtils::deallocateProcedureCtxt(cb, proc_p);
    }

    switch (errorCause)
    {
    case X2HO_START_FAILURE:
    case X2HO_PSWACK_SEND_FAILURE:
    case X2HO_FAILURE:
    {
        // deallocate any waiting HO contexts on the SM side.
        // Detach is going to cleanup the session
        MmeContextManagerUtils::deallocateAllProcedureCtxts(cb, x2HandoverSm_c);

        // Start MME_INIT Detach procedure
        MmeDetachProcedureCtxt *prcdCtxt_p =
                MmeContextManagerUtils::allocateDetachProcedureCtxt(cb,
                        mmeInitDetach_c);

        prcdCtxt_p->setDetachCause(
                MmeCauseUtils::convertToNasEmmCause(
                        prcdCtxt_p->getMmeErrorCause()));

        SM::Event evt(MME_INIT_DETACH, NULL);
        cb.qInternalEvent(evt);
        break;
    }
    case SESSION_CONTAINER_EMPTY:
    case BEARER_CONTAINER_EMPTY:
    case SESSION_CONTEXT_NOT_FOUND:
    {
        MmeContextManagerUtils::deleteUEContext(cb.getCBIndex());
        break;
    }
    default:
    {
        log_msg(LOG_DEBUG,
                "Unhandled Abort Scenario in X2 HO with errorCause %d",
                errorCause);
    }
    }

    return ActStatus::PROCEED;
}

/***************************************
 * Action handler : x2_ho_mm_complete
 ***************************************/
ActStatus ActionHandlers::x2_ho_mm_complete(ControlBlock &cb)
{
    log_msg(LOG_DEBUG, "x2_ho_mm_complete : Entry ");

    UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
    VERIFY_UE(cb, ue_ctxt, "Invalid UE");

    X2HOMmProcedureContext *proc_p =
            dynamic_cast<X2HOMmProcedureContext*>(cb.getTempDataBlock());
    if (proc_p)
    {
        MmeContextManagerUtils::deallocateProcedureCtxt(cb, proc_p);
    }

    SM::Event evt(X2_HO_COMPLETE, NULL);
    cb.qInternalEvent(evt);

    mmeStats::Instance()->increment(
            mmeStatsCounter::MME_PROCEDURES_X2_ENB_HANDOVER_PROC_SUCCESS);

    return ActStatus::PROCEED;
}
