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
 * Action handler : send_mb_req_to_sgw_for_x2_ho
 ***************************************/
ActStatus ActionHandlers::send_mb_req_to_sgw_for_x2_ho(ControlBlock &cb)
{
    UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
    VERIFY_UE(cb, ue_ctxt, "Invalid UE");

    log_msg(LOG_DEBUG, "Inside send_mb_req_to_sgw_for_x2_ho  %d",
            ue_ctxt->getContextID());

    X2HOSmProcedureContext *hoCtxt =
            dynamic_cast<X2HOSmProcedureContext*>(cb.getTempDataBlock());
    VERIFY(hoCtxt, return ActStatus::ABORT,
            "X2 HO SM Procedure Context is NULL ");

    SessionContext *sessionCtxt =
            MmeContextManagerUtils::findSessionCtxtForEpsBrId(
                    hoCtxt->getBearerId(), ue_ctxt);
    VERIFY(sessionCtxt,
            hoCtxt->setMmeErrorCause(SESSION_CONTEXT_NOT_FOUND); return ActStatus::ABORT,
            "Session Context is NULL ");

    ActStatus actstatus = ActStatus::PROCEED;

    struct MB_Q_msg mb_msg;
    memset(&mb_msg, 0, sizeof(struct MB_Q_msg));

    bool rc = MmeGtpMsgUtils::populateModifyBearerRequestForX2Ho(cb, *ue_ctxt,
            *sessionCtxt, *hoCtxt, mb_msg);

    if (!rc)
    {
        log_msg(LOG_INFO, "Failed to send Modify Bearer Request ");

        hoCtxt->setMmeErrorCause(X2HO_MBREQ_SEND_FAILURE);

        actstatus = ActStatus::ABORT;
    }
    else
    {
        mmeStats::Instance()->increment(
                mmeStatsCounter::MME_MSG_TX_S11_MODIFY_BEARER_REQUEST);

        cmn::ipc::IpcAddress destAddr =
        { TipcServiceInstance::s11AppInstanceNum_c };
        MmeIpcInterface &mmeIpcIf =
                static_cast<MmeIpcInterface&>(compDb.getComponent(
                        MmeIpcInterfaceCompId));
        mmeIpcIf.dispatchIpcMsg((char*) &mb_msg, sizeof(mb_msg), destAddr);

        ProcedureStats::num_of_mb_req_to_sgw_sent++;
    }

    hoCtxt->resetPathSwitchReqMsgPtr();

    log_msg(LOG_DEBUG, "Leaving send_mb_req_to_sgw_for_x2_ho ");
    return actstatus;
}

/***************************************
 * Action handler : process_mb_resp_for_x2_ho
 ***************************************/
ActStatus ActionHandlers::process_mb_resp_for_x2_ho(ControlBlock &cb)
{
    log_msg(LOG_DEBUG, "process_mb_resp_for_x2_ho : Entry ");

    UEContext *ueCtxt = static_cast<UEContext*>(cb.getPermDataBlock());
    VERIFY_UE(cb, ueCtxt, "Invalid UE");

    X2HOSmProcedureContext *procCtxt =
            dynamic_cast<X2HOSmProcedureContext*>(cb.getTempDataBlock());
    VERIFY(procCtxt, return ActStatus::ABORT, "Procedure Context is NULL ");

    SessionContext *sessionCtxt =
            MmeContextManagerUtils::findSessionCtxtForEpsBrId(
                    procCtxt->getBearerId(), ueCtxt);
    VERIFY(sessionCtxt,
            procCtxt->setMmeErrorCause(SESSION_CONTEXT_NOT_FOUND); return ActStatus::ABORT,
            "Session Context is NULL ");

    MsgBuffer *msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
    VERIFY(msgBuf,
            procCtxt->setMmeErrorCause(INVALID_MSG_BUFFER); return ActStatus::ABORT,
            "process_mb_resp_for_x2_ho : Invalid message buffer ");

    VERIFY(msgBuf->getLength() >= sizeof(struct MB_resp_Q_msg),
            procCtxt->setMmeErrorCause(INVALID_MSG_BUFFER);return ActStatus::ABORT,
                    "Invalid MBResp message length ");

    const struct MB_resp_Q_msg *mbr_info =
            static_cast<const MB_resp_Q_msg*>(msgBuf->getDataPointer());

    ActStatus actStatus = ActStatus::PROCEED;

    erab_list &erabFailedList = procCtxt->getErabFailedList();

    if (mbr_info->cause != GTPV2C_CAUSE_REQUEST_ACCEPTED)
    {
        log_msg(LOG_INFO, "X2HO MB Resp failure: IMSI %s, defBearerId %d",
                ueCtxt->getImsi().getDigitsArray(), procCtxt->getBearerId());

        procCtxt->setMmeErrorCause(X2HO_MBRESP_FAILURE);

        actStatus = ActStatus::ABORT;
    }
    else
    {
        uint8_t bearer_count = 0;
        failed_bearer_ctxt_list_t bearer_ctxt_list = { 0 };
        bearer_ctxt_list.bearers_count =
                mbr_info->bearer_ctxt_mb_resp_list.bearers_count
                        + mbr_info->bearer_ctxt_removed_list.bearers_count;

        for (int i = 0; i < mbr_info->bearer_ctxt_mb_resp_list.bearers_count; i++)
        {
            bearer_ctxt_list.bearer_ctxt[i].eps_bearer_id =
                    mbr_info->bearer_ctxt_mb_resp_list.bearer_ctxt[i].eps_bearer_id;
            bearer_ctxt_list.bearer_ctxt[i].cause.cause =
                    mbr_info->bearer_ctxt_mb_resp_list.bearer_ctxt[i].cause.cause;
            bearer_count++;
        }

        for (int i = 0; i < mbr_info->bearer_ctxt_removed_list.bearers_count; i++)
        {
            bearer_ctxt_list.bearer_ctxt[bearer_count].eps_bearer_id =
                    mbr_info->bearer_ctxt_removed_list.bearer_ctxt[i].eps_bearer_id;
            bearer_ctxt_list.bearer_ctxt[bearer_count].cause.cause =
                    mbr_info->bearer_ctxt_removed_list.bearer_ctxt[i].cause.cause;
            bearer_count++;
        }

        for (int i = 0; i < bearer_ctxt_list.bearers_count; i++)
        {
            failed_bearer_ctxt_t *bearersModified =
                    &(bearer_ctxt_list.bearer_ctxt[i]);

            BearerContext *bearerCtxt =
                    sessionCtxt->findBearerContextByBearerId(
                            bearersModified->eps_bearer_id);

            if (bearerCtxt)
            {
                if (bearersModified->cause.cause != GTPV2C_CAUSE_REQUEST_ACCEPTED)
                {
                    if (bearerCtxt->getBearerId() == sessionCtxt->getLinkedBearerId())
                    {
                        log_msg(LOG_INFO,
                                "X2HO MB Resp failure: IMSI %s, defBearerId %d",
                                ueCtxt->getImsi().getDigitsArray(),
                                procCtxt->getBearerId());

                        procCtxt->setMmeErrorCause(X2HO_MBRESP_FAILURE);
                        actStatus = ActStatus::ABORT;
                    }
                    else
                    {
                        erabFailedList.erab_item[erabFailedList.count].e_RAB_ID =
                                bearerCtxt->getBearerId();
                        erabFailedList.erab_item[erabFailedList.count].cause =
                                MmeCauseUtils::convertToS1apCause(
                                        bearersModified->cause.cause).s1apCause_m;
                        erabFailedList.count++;
                    }
                }
            }
        }
    }

    log_msg(LOG_DEBUG, "process_mb_resp_for_x2_ho : Exit ");
    return actStatus;
}

/***************************************
* Action handler : send_sm_ho_complete
***************************************/
ActStatus ActionHandlers::send_sm_ho_complete(ControlBlock& cb)
{
    log_msg(LOG_DEBUG, "send_sm_ho_complete : Entry ");

    X2HOSmProcedureContext *proc_p =
            dynamic_cast<X2HOSmProcedureContext*>(cb.getTempDataBlock());

    if (proc_p)
    {
        erab_list  &erabFailedList = proc_p->getErabFailedList();

        PdnHoStatusEMsgShPtr eMsg = std::make_shared<PdnHoStatusEMsg>(
                proc_p->getBearerId(), success_c);
        eMsg->pdnHoStatus.failed_br_count = erabFailedList.count;
        memcpy(eMsg->pdnHoStatus.failed_br_list, erabFailedList.erab_item,
                erabFailedList.count * sizeof(erab_list_item));

        SM::Event evt(X2_HO_SM_COMPLETE, eMsg);
        cb.qInternalEvent(evt);
    }
    return ActStatus::PROCEED;
}

/***************************************
 * Action handler : abort_x2_ho_sm
 ***************************************/
ActStatus ActionHandlers::abort_x2_ho_sm(ControlBlock &cb)
{
    log_msg(LOG_DEBUG, "abort_x2_ho_sm : Entry ");

    UEContext *ueCtxt = static_cast<UEContext*>(cb.getPermDataBlock());

    X2HOSmProcedureContext *proc_p =
            dynamic_cast<X2HOSmProcedureContext*>(cb.getTempDataBlock());

    if (proc_p)
    {
        bool deallocateHoCtxt = true;

        PdnHoStatusEMsgShPtr eMsg = std::make_shared<PdnHoStatusEMsg>(
                proc_p->getBearerId(), failure_c);

        switch(proc_p->getMmeErrorCause())
        {
        case X2HO_MBREQ_SEND_FAILURE:
        case X2HO_MBRESP_FAILURE:
        case INVALID_MSG_BUFFER:
        {
            if (ueCtxt)
            {
                SessionContext *sessionCtxt =
                        MmeContextManagerUtils::findSessionCtxtForEpsBrId(
                                proc_p->getBearerId(), ueCtxt);
                if (sessionCtxt)
                {
                    auto &bearerContainer = sessionCtxt->getBearerContextContainer();

                    uint8_t count = 0;
                    for (auto bearer_p: bearerContainer)
                    {
                        eMsg->pdnHoStatus.failed_br_list[count].e_RAB_ID =
                                bearer_p->getBearerId();
                        eMsg->pdnHoStatus.failed_br_list[count].cause.present =
                                s1apCause_PR_misc;
                        eMsg->pdnHoStatus.failed_br_list[count].cause.choice.misc =
                                s1apCauseMisc_unspecified;
                        count++;
                    }
                    eMsg->pdnHoStatus.failed_br_count = count;
                    deallocateHoCtxt = false;
                }
            }
        }
        break;
        default:
        {
        }
        }

        if (deallocateHoCtxt)
        {
            MmeContextManagerUtils::deallocateProcedureCtxt(cb, proc_p);
        }

        SM::Event evt(X2_HO_SM_COMPLETE, eMsg);
        cb.qInternalEvent(evt);
    }

    return ActStatus::PROCEED;
}


/***************************************
 * Action handler : x2_ho_sm_complete
 ***************************************/
ActStatus ActionHandlers::x2_ho_sm_complete(ControlBlock &cb)
{
    log_msg(LOG_DEBUG, "x2_ho_sm_complete : Entry ");

    UEContext *ueCtxt = static_cast<UEContext*>(cb.getPermDataBlock());
    VERIFY_UE(cb, ueCtxt, "Invalid UE");

    X2HOSmProcedureContext *proc_p =
            dynamic_cast<X2HOSmProcedureContext*>(cb.getTempDataBlock());

    if (proc_p)
    {
        for(int i = 0; i < proc_p->getErabFailedList().count; i++)
        {
            uint8_t bearerId = proc_p->getErabFailedList().erab_item[i].e_RAB_ID;

            SessionContext* sess_p =
                    MmeContextManagerUtils::findSessionCtxtForEpsBrId(bearerId,
                            ueCtxt);

            BearerContext* bearer_p = MmeContextManagerUtils::findBearerContext(
                    bearerId, ueCtxt, sess_p);

            /* When MME Init Dedicated Bearer Deactivation is supported, start
             * deactivation from here. For now, just deallocate the bearer Context*/
            MmeContextManagerUtils::deallocateBearerContext(cb, bearer_p,
                    sess_p, ueCtxt);
        }
        MmeContextManagerUtils::deallocateProcedureCtxt(cb, proc_p);
    }

    log_msg(LOG_DEBUG, "x2_ho_sm_complete : Exit ");
    return ActStatus::PROCEED;
}



