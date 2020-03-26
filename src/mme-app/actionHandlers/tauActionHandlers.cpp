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
#include "msgType.h"
#include "contextManager/dataBlocks.h"
#include "procedureStats.h"
#include "log.h"
#include "secUtils.h"
#include "state.h"
#include <string.h>
#include <sstream>
#include <mmeSmDefs.h>
#include "common_proc_info.h"
#include <ipcTypes.h>
#include <tipcTypes.h>
#include <msgBuffer.h>
#include <interfaces/mmeIpcInterface.h>
#include <event.h>
#include <stateMachineEngine.h>
#include <utils/mmeContextManagerUtils.h>

using namespace mme;
using namespace SM;
using namespace cmn::utils;

extern MmeIpcInterface* mmeIpcIf_g;

/***************************************
* Action handler : send_tau_response_to_ue
***************************************/
ActStatus ActionHandlers::send_tau_response_to_ue(ControlBlock& cb)
{	
	log_msg(LOG_INFO,"Inside send_tau_response_to_ue\n");
	
	UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());	
	if (ue_ctxt == NULL)
	{
		log_msg(LOG_ERROR, "send_tau_response_to_ue: ue context is NULL\n",cb.getCBIndex());
		return ActStatus::HALT;
	}
	
	MmeTauProcedureCtxt* tauPrcdCtxt_p = dynamic_cast<MmeTauProcedureCtxt*>(cb.getTempDataBlock());	
	if (tauPrcdCtxt_p == NULL)
	{
		log_msg(LOG_DEBUG, "send_tau_response_to_ue: MmeTauProcedureCtxt is NULL\n");
		return ActStatus::HALT;
	}
	
	struct tauResp_Q_msg tau_resp;

	tau_resp.msg_type = tau_response;
	tau_resp.status = 0;
	tau_resp.ue_idx = ue_ctxt->getContextID();
	tau_resp.enb_fd = tauPrcdCtxt_p->getEnbFd();
	tau_resp.s1ap_enb_ue_id = tauPrcdCtxt_p->getS1apEnbUeId();	
	tau_resp.dl_seq_no = ue_ctxt->getDwnLnkSeqNo();
	memcpy(&(tau_resp.int_key), &(ue_ctxt->getUeSecInfo().secinfo_m.int_key),
			NAS_INT_KEY_SIZE);
	memcpy(&tau_resp.tai, &(ue_ctxt->getTai().tai_m), sizeof(struct TAI));	
	tau_resp.m_tmsi = ue_ctxt->getMTmsi();
	
	cmn::ipc::IpcAddress destAddr;
        destAddr.u32 = TipcServiceInstance::s1apAppInstanceNum_c;	
	mmeIpcIf_g->dispatchIpcMsg((char *) &tau_resp, sizeof(tau_resp), destAddr);
	
	MmeContextManagerUtils::deallocateProcedureCtxt(cb, tau_c );
	ProcedureStats::num_of_tau_response_to_ue_sent++;

	log_msg(LOG_INFO,"Leaving send_tau_response_to_ue\n");
	return ActStatus::PROCEED;
}

