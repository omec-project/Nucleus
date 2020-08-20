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
 * Action handler : send_erab_mod_conf_to_enb
 ***************************************/
ActStatus ActionHandlers::send_erab_mod_conf_to_enb(ControlBlock &cb)
{
    log_msg(LOG_DEBUG, "Inside send_erab_mod_conf_to_enb \n");

    UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
    if (ue_ctxt == NULL)
    {
        log_msg(LOG_INFO, "send_erab_mod_conf_to_enb: ue context is NULL \n");

        MmeContextManagerUtils::deleteUEContext(cb.getCBIndex());
        return ActStatus::HALT;
    }

    ActStatus actStatus = ActStatus::PROCEED;
    MmeErabModIndProcedureCtxt *procCtxt =
            dynamic_cast<MmeErabModIndProcedureCtxt*>(cb.getTempDataBlock());
    if (procCtxt != NULL)
    {
        struct erab_mod_confirm erab_mod_conf;
        memset(&erab_mod_conf, 0, sizeof (struct erab_mod_confirm));

        erab_mod_conf.msg_type = erab_mod_confirmation;
        erab_mod_conf.mme_s1ap_ue_id = ue_ctxt->getContextID();
        erab_mod_conf.enb_s1ap_ue_id = ue_ctxt->getS1apEnbUeId();
        erab_mod_conf.enb_context_id = ue_ctxt->getEnbFd();
        erab_mod_conf.erab_mod_list.count = procCtxt->getErabModifiedListLen();

        const uint8_t *erabModifiedList = procCtxt->getErabModifiedList();
        for (uint32_t i = 0; i < erab_mod_conf.erab_mod_list.count; i++)
        {
            erab_mod_conf.erab_mod_list.erab_id[i] = erabModifiedList[i];
        }

        cmn::ipc::IpcAddress destAddr;
        destAddr.u32 = TipcServiceInstance::s1apAppInstanceNum_c;

        mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_S1AP_ERAB_MODIFICATION_INDICATION);
        MmeIpcInterface &mmeIpcIf =
                static_cast<MmeIpcInterface&>(compDb.getComponent(
                        MmeIpcInterfaceCompId));
        mmeIpcIf.dispatchIpcMsg((char*) &erab_mod_conf, sizeof(erab_mod_conf),
                destAddr);

        ProcedureStats::num_of_erab_mod_conf_sent++;
    }
    else
    {
        log_msg(LOG_INFO, "Procedure Context is NULL \n");
        actStatus = ActStatus::HALT;
    }

    log_msg(LOG_DEBUG, "Leaving send_erab_mod_conf_to_enb \n");

    return actStatus;
}

/***************************************
 * Action handler : erab_mod_ind_complete
 ***************************************/
ActStatus ActionHandlers::erab_mod_ind_complete(ControlBlock &cb)
{
    log_msg(LOG_DEBUG, "Inside erab_mod_ind_complete\n");
    mmeStats::Instance()->increment(mmeStatsCounter::MME_PROCEDURES_ERAB_MOD_IND_PROC_SUCCESS);
    MmeContextManagerUtils::deallocateProcedureCtxt(cb, erabModInd_c);
    return ActStatus::PROCEED;

}

/***************************************
 * Action handler : handle_state_guard_timeout_erab_mod_ind_proc
 ***************************************/
ActStatus ActionHandlers::handle_state_guard_timeouts_for_erab_mod_ind(
        ControlBlock &cb)
{
    log_msg(LOG_DEBUG,
            "Inside handle_state_guard_timeout_erab_mod_ind_proc \n");

    ActStatus actStatus = ActStatus::PROCEED;

    MmeProcedureCtxt *procCtxt =
            dynamic_cast<MmeProcedureCtxt*>(cb.getTempDataBlock());
    if (procCtxt != NULL)
    {
        procCtxt->setMmeErrorCause(networkTimeout_c);
        ProcedureStats::num_of_erab_mod_ind_timeout++;
    }
    else
    {
        log_msg(LOG_DEBUG,
                "State Guard Timer Expiry for ERAB MOD IND: "
                "Procedure context is NULL\n");
        actStatus = ActStatus::HALT;
    }

    return actStatus;
}

/**********************************************
 * Action handler : abort_erab_mod_indication
 ***********************************************/
ActStatus ActionHandlers::abort_erab_mod_indication(SM::ControlBlock &cb)
{
    log_msg(LOG_DEBUG, "Inside abort_erab_mod_indication \n");

    ActStatus actStatus = ActStatus::PROCEED;

    mmeStats::Instance()->increment(mmeStatsCounter::MME_PROCEDURES_ERAB_MOD_IND_PROC_FAILURE);
    MmeContextManagerUtils::deallocateProcedureCtxt(cb, erabModInd_c);

    // Start MME_INIT Detach procedure
    MmeDetachProcedureCtxt *prcdCtxt_p =
            MmeContextManagerUtils::allocateDetachProcedureCtxt(cb,
                    mmeInitDetach_c);
    if (prcdCtxt_p != NULL)
    {
        prcdCtxt_p->setDetachCause(
                MmeCauseUtils::convertToNasEmmCause(
                        prcdCtxt_p->getMmeErrorCause()));

        SM::Event evt(MME_INIT_DETACH, NULL);
        cb.addEventToProcQ(evt);
    }
    else
    {
        log_msg(LOG_ERROR, "Failed to allocate Detach procedure context"
                " for abort_erab_mod_indication cbIndex %d\n", cb.getCBIndex());
        actStatus = ActStatus::HALT;
    }

    ProcedureStats::num_of_abort_erab_mod_indication++;
    return actStatus;
}
