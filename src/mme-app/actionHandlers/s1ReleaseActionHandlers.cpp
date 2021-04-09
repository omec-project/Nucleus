/*
 * Copyright 2019-present Infosys Limited
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

#include <typeinfo>
#include "actionHandlers/actionHandlers.h"
#include "controlBlock.h"
#include "mme_app.h"
#include "msgType.h"
#include "contextManager/subsDataGroupManager.h"
#include "contextManager/dataBlocks.h"
#include "procedureStats.h"
#include "log.h"
#include "secUtils.h"
#include "state.h"
#include <string.h>
#include <sstream>
#include <mmeSmDefs.h>

#include <ipcTypes.h>
#include <tipcTypes.h>
#include <msgBuffer.h>
#include <interfaces/mmeIpcInterface.h>
#include <utils/mmeContextManagerUtils.h>
#include <utils/mmeCauseUtils.h>
#include "mmeStatsPromClient.h"

using namespace SM;
using namespace mme;
using namespace cmn;
using namespace cmn::utils;

ActStatus ActionHandlers:: send_rel_ab_req_to_sgw(SM::ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside send_rel_ab_req_to_sgw ");

	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ue_ctxt, "Invalid UE");
	
    	MmeS1RelProcedureCtxt *procCtxt = dynamic_cast<MmeS1RelProcedureCtxt*>(cb.getTempDataBlock());
    	VERIFY(procCtxt, return ActStatus::ABORT, "Procedure Context is NULL ");

	auto& sessionCtxtContainer = ue_ctxt->getSessionContextContainer();
	VERIFY(sessionCtxtContainer.size() > 0,
	        procCtxt->setMmeErrorCause(SESSION_CONTAINER_EMPTY); return ActStatus::ABORT,
	        "Sessions Container is empty");

	SessionContext* sessionCtxt = sessionCtxtContainer.front();
    	VERIFY(sessionCtxt,
            procCtxt->setMmeErrorCause(SESSION_CONTEXT_NOT_FOUND); return ActStatus::ABORT,
            "Sessions Context is NULL");

	BearerContext* bearerCtxt = sessionCtxt->findBearerContextByBearerId(sessionCtxt->getLinkedBearerId());
	VERIFY(bearerCtxt,
	        procCtxt->setMmeErrorCause(BEARER_CONTEXT_NOT_FOUND);return ActStatus::ABORT,
	          "Bearer Context is NULL ");

    if(ue_ctxt->getS1apEnbUeId() != procCtxt->getS1apEnbUeId())
    {
        log_msg(LOG_DEBUG, "S1 Release req with wrong enb_s1ap_ue_id. UE Context has %u " 
                           "and proc context has %u ",ue_ctxt->getS1apEnbUeId(), procCtxt->getS1apEnbUeId());
        return ActStatus::ABORT;
    }

	struct RB_Q_msg rb_msg;
	rb_msg.msg_type = release_bearer_request;
	rb_msg.ue_idx = ue_ctxt->getContextID();
	memset(rb_msg.indication, 0 , S11_RB_INDICATION_FLAG_SIZE);
	rb_msg.bearer_id = bearerCtxt->getBearerId();
	memcpy(&(rb_msg.s11_sgw_c_fteid), 
           &(sessionCtxt->getS11SgwCtrlFteid().fteid_m), 
           sizeof(struct fteid));
	memcpy(&(rb_msg.s1u_enb_fteid), 
           &(bearerCtxt->getS1uEnbUserFteid().fteid_m),
			sizeof(struct fteid));
			
	cmn::ipc::IpcAddress destAddr;
	destAddr.u32 = TipcServiceInstance::s11AppInstanceNum_c;
	
    	mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_S11_RELEASE_BEARER_REQUEST);
	MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));
	mmeIpcIf.dispatchIpcMsg((char *) &rb_msg, sizeof(rb_msg), destAddr);

	ProcedureStats::num_of_rel_access_bearer_req_sent ++;
	
	log_msg(LOG_DEBUG, "Inside send_rel_ab_req_to_sgw ");

	return ActStatus::PROCEED;
}

ActStatus ActionHandlers:: process_rel_ab_resp_from_sgw(SM::ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "process_rel_ab_resp_from_sgw ");

	ProcedureStats::num_of_rel_access_bearer_resp_received ++;

	return ActStatus::PROCEED;
}

ActStatus ActionHandlers:: send_s1_rel_cmd_to_ue(SM::ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside send_s1_rel_cmd_to_ue");

	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ue_ctxt, "Invalid UE");
	S1apCause s1apCause;
	MmeProcedureCtxt* prcdCtxt_p = 
		dynamic_cast<MmeProcedureCtxt*>(cb.getTempDataBlock());

	if(prcdCtxt_p)
	    s1apCause = prcdCtxt_p->getS1apCause();

	if(s1apCause.s1apCause_m.present == s1apCause_PR_NOTHING)
	{
	    s1apCause = MmeCauseUtils::convertToS1apCause(prcdCtxt_p->getMmeErrorCause());
	}	
	struct s1relcmd_info s1relcmd;
	s1relcmd.msg_type = s1_release_command;
	s1relcmd.ue_idx = ue_ctxt->getContextID();
	s1relcmd.enb_fd = ue_ctxt->getEnbFd();
	s1relcmd.enb_s1ap_ue_id = ue_ctxt->getS1apEnbUeId();
	s1relcmd.cause = s1apCause.s1apCause_m;
	
	/*Send message to S1AP-APP*/
	cmn::ipc::IpcAddress destAddr;
	destAddr.u32 = TipcServiceInstance::s1apAppInstanceNum_c;

    	mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_S1AP_S1_RELEASE_COMMAND);
	MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));
	mmeIpcIf.dispatchIpcMsg((char *) &s1relcmd, sizeof(s1relcmd), destAddr);
	
	ProcedureStats::num_of_s1_rel_cmd_sent ++;
	
	log_msg(LOG_DEBUG,"Leaving send_s1_rel_cmd_to_ue ");

	return ActStatus::PROCEED;
}

/***************************************
* Action handler : abort_s1_release
***************************************/
ActStatus ActionHandlers::abort_s1_release(ControlBlock& cb)
{
    ERROR_CODES errorCause = SUCCESS;

    MmeProcedureCtxt *procCtxt = dynamic_cast<MmeProcedureCtxt*>(cb.getTempDataBlock());
    if (procCtxt != NULL)
    {
        errorCause = procCtxt->getMmeErrorCause();
        MmeContextManagerUtils::deallocateProcedureCtxt(cb, procCtxt);
    }

    mmeStats::Instance()->increment(mmeStatsCounter::MME_PROCEDURES_S1_RELEASE_PROC_FAILURE);
    if (errorCause == ABORT_DUE_TO_ATTACH_COLLISION)
    {
        MmeContextManagerUtils::deleteUEContext(cb.getCBIndex(), false); // retain control block
    }
    else
    {
        MmeContextManagerUtils::deleteUEContext(cb.getCBIndex());
    }
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : s1_release_complete
***************************************/
ActStatus ActionHandlers::s1_release_complete(ControlBlock& cb)
{
    log_msg(LOG_DEBUG, "Insidei s1_release_complete ");

    UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
    VERIFY_UE(cb, ue_ctxt, "Invalid UE");

    MmContext *mmCtxt = ue_ctxt->getMmContext();
    VERIFY_UE(cb, mmCtxt, "Invalid MM Context");

    mmCtxt->setEcmState(ecmIdle_c);

    mmeStats::Instance()->increment(
            mmeStatsCounter::MME_PROCEDURES_S1_RELEASE_PROC);

    MmeS1RelProcedureCtxt *procedure_p =
            dynamic_cast<MmeS1RelProcedureCtxt*>(cb.getTempDataBlock());
    if (procedure_p != NULL)
    {
        MmeContextManagerUtils::deallocateProcedureCtxt(cb, procedure_p);
    }

    ue_ctxt->setS1apEnbUeId(0);
    ProcedureStats::num_of_s1_rel_comp_received++;

    log_msg(LOG_DEBUG, "Leaving s1_release_complete ");

    return ActStatus::PROCEED;
}

