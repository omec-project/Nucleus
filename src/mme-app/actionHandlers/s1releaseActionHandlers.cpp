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

using namespace SM;
using namespace mme;
using namespace cmn;
using namespace cmn::utils;

ActStatus ActionHandlers:: send_rel_ab_req_to_sgw(SM::ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside send_rel_ab_req_to_sgw \n");

	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	if (ue_ctxt == NULL)
	{
		log_msg(LOG_DEBUG, "send_rel_ab_req_to_sgw: ue ctxt is NULL \n");
		return ActStatus::HALT;
	}
	
	SessionContext* sessionCtxt = ue_ctxt->getSessionContext();
	if (sessionCtxt == NULL)
	{
		log_msg(LOG_DEBUG, " send_rel_ab_req_to_sgw: session ctxt is NULL \n");
		return ActStatus::HALT;
	}

	BearerContext* bearerCtxt = sessionCtxt->getBearerContext();
	if (bearerCtxt == NULL)
	{
		log_msg(LOG_DEBUG, " send_rel_ab_req_to_sgw: bearer ctxt is NULL \n");
		return ActStatus::HALT;
	}

	struct RB_Q_msg rb_msg;
	rb_msg.msg_type = release_bearer_request;
	rb_msg.ue_idx = ue_ctxt->getContextID();
	memset(rb_msg.indication, 0 , S11_RB_INDICATION_FLAG_SIZE);
	rb_msg.bearer_id = bearerCtxt->getBearerId();
	memcpy(&(rb_msg.s11_sgw_c_fteid), &(sessionCtxt->getS11SgwCtrlFteid()),
			sizeof(struct fteid));
	memcpy(&(rb_msg.s1u_enb_fteid), &(bearerCtxt->getS1uEnbUserFteid()),
			sizeof(struct fteid));
			
	cmn::ipc::IpcAddress destAddr;
	destAddr.u32 = TipcServiceInstance::s11AppInstanceNum_c;
	
	MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));
	mmeIpcIf.dispatchIpcMsg((char *) &rb_msg, sizeof(rb_msg), destAddr);

	ProcedureStats::num_of_rel_access_bearer_req_sent ++;
	
	log_msg(LOG_DEBUG, "Inside send_rel_ab_req_to_sgw \n");

	return ActStatus::PROCEED;
}

ActStatus ActionHandlers:: process_rel_ab_resp_from_sgw(SM::ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "process_rel_ab_resp_from_sgw \n");

	ProcedureStats::num_of_rel_access_bearer_resp_received ++;

	return ActStatus::PROCEED;
}

ActStatus ActionHandlers:: send_s1_rel_cmd_to_ue(SM::ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside send_s1_rel_cmd_to_ue\n");

	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	if(ue_ctxt == NULL)
	{
		log_msg(LOG_DEBUG, "send_s1_rel_cmd_to_ue: ue context is NULL \n");

		return ActStatus::HALT;
	}
	
	MmeProcedureCtxt* prcdCtxt_p = 
		dynamic_cast<MmeProcedureCtxt*>(cb.getTempDataBlock());

	if (prcdCtxt_p == NULL)
	{
		log_msg(LOG_ERROR, "send_s1_rel_cmd_to_ue: Proc Ctxt is NULL\n");
		return ActStatus::HALT;
	}

	S1apCause s1apCause = prcdCtxt_p->getS1apCause();
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

	MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));
	mmeIpcIf.dispatchIpcMsg((char *) &s1relcmd, sizeof(s1relcmd), destAddr);
	
	ProcedureStats::num_of_s1_rel_cmd_sent ++;
	
	log_msg(LOG_DEBUG,"Leaving send_s1_rel_cmd_to_ue \n");

	return ActStatus::PROCEED;
}

ActStatus ActionHandlers:: process_ue_ctxt_rel_comp(SM::ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside handle_ctxt_rel_comp \n");

	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	if (ue_ctxt == NULL)
	{
		log_msg(LOG_DEBUG, "process_ue_ctxt_rel_comp: ue context is NULL\n");

		return ActStatus::HALT;
	}

	MmContext* mmCtxt = ue_ctxt->getMmContext();
        if (mmCtxt == NULL)
        {
                log_msg(LOG_ERROR, "process_ue_ctxt_rel_comp: MMcontext is NULL \n");
                return ActStatus::HALT;
        }

        mmCtxt->setEcmState(ecmIdle_c);


	MmeContextManagerUtils::deallocateProcedureCtxt(cb, s1Release_c);
	ue_ctxt->setS1apEnbUeId(0);
	ProcedureStats::num_of_s1_rel_comp_received++;

    	log_msg(LOG_DEBUG, "Leaving process_ue_ctxt_rel_comp \n");

    	return ActStatus::PROCEED;
}

	
	
