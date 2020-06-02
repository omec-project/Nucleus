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
#include "mme_app.h"
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
#include <utils/mmeCommonUtils.h>

using namespace mme;
using namespace SM;
using namespace cmn;
using namespace cmn::utils;

/***************************************
* Action handler : process_tau_request
***************************************/
ActStatus ActionHandlers::process_tau_request(ControlBlock& cb)
{
	log_msg(LOG_INFO,"Inside process_tau_request\n");

	UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
	if (ue_ctxt == NULL)
	{
		log_msg(LOG_ERROR, "process_tau_request: ue context is NULL\n",cb.getCBIndex());
		return ActStatus::HALT;
	}

	MmeProcedureCtxt* prcdCtxt_p = dynamic_cast<MmeProcedureCtxt*>(cb.getTempDataBlock());
	if (prcdCtxt_p == NULL)
	{
		log_msg(LOG_DEBUG, "process_tau_request: MmeProcedureCtxt is NULL\n");
		return ActStatus::HALT;
	}

	MsgBuffer* msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
	if (msgBuf == NULL)
	{
            log_msg(LOG_DEBUG,"process_tau_req: msgBuf is NULL \n");
            return ActStatus::HALT;
	}

	const s1_incoming_msg_data_t* msgData_p =
			static_cast<const s1_incoming_msg_data_t*>(msgBuf->getDataPointer());
	if (msgData_p == NULL)
	{
		log_msg(LOG_ERROR, "Failed to retrieve data buffer \n");
		return ActStatus::HALT;
	}

	const struct tauReq_Q_msg &tauReq = (msgData_p->msg_data.tauReq_Q_msg_m);
	ue_ctxt->setUpLnkSeqNo(ue_ctxt->getUpLnkSeqNo()+1);

	if( prcdCtxt_p->getCtxtType() == s1Handover_c)
	{
		S1HandoverProcedureContext *s1HoPrCtxt = dynamic_cast<S1HandoverProcedureContext*>(prcdCtxt_p);

		//TAI and CGI obtained from s1ap ies.
		//Convert the plmn in s1ap format to nas format
		//before storing in ue context/sending in tai list of tau response.
		MmeCommonUtils::formatS1apPlmnId(const_cast<PLMN*>(&tauReq.tai.plmn_id));
		MmeCommonUtils::formatS1apPlmnId(const_cast<PLMN*>(&tauReq.eUtran_cgi.plmn_id));
		s1HoPrCtxt->setTargetTai(Tai(tauReq.tai));
		s1HoPrCtxt->setTargetCgi(Cgi(tauReq.eUtran_cgi));
	}
    return ActStatus::PROCEED;
}

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

	MmeProcedureCtxt* prcdCtxt_p = dynamic_cast<MmeProcedureCtxt*>(cb.getTempDataBlock());
	if (prcdCtxt_p == NULL)
	{
		log_msg(LOG_DEBUG, "process_tau_request: MmeProcedureCtxt is NULL\n");
		return ActStatus::HALT;
	}

	struct tauResp_Q_msg tau_resp;
	if( prcdCtxt_p->getCtxtType() == s1Handover_c)
	{
		S1HandoverProcedureContext *s1HoPrCtxt = static_cast<S1HandoverProcedureContext*>(prcdCtxt_p);
		tau_resp.enb_fd = s1HoPrCtxt->getTargetEnbContextId();
		tau_resp.s1ap_enb_ue_id = s1HoPrCtxt->getTargetS1apEnbUeId();
		memcpy(&tau_resp.tai, &(s1HoPrCtxt->getTargetTai().tai_m), sizeof(struct TAI));
		ue_ctxt->setUtranCgi(s1HoPrCtxt->getTargetCgi());
	}
	else
	{
		MmeTauProcedureCtxt *tauPrCtxt = static_cast<MmeTauProcedureCtxt*>(prcdCtxt_p);
		tau_resp.enb_fd = tauPrCtxt->getEnbFd();
		tau_resp.s1ap_enb_ue_id = tauPrCtxt->getS1apEnbUeId();
		memcpy(&tau_resp.tai, &(tauPrCtxt->getTai().tai_m), sizeof(struct TAI));
		ue_ctxt->setUtranCgi(tauPrCtxt->getEUtranCgi());
	}

	tau_resp.msg_type = tau_response;
	tau_resp.status = 0;
	tau_resp.ue_idx = ue_ctxt->getContextID();
	tau_resp.dl_seq_no = ue_ctxt->getDwnLnkSeqNo();
	ue_ctxt->setDwnLnkSeqNo(tau_resp.dl_seq_no+1);
	memcpy(&(tau_resp.int_key), &(ue_ctxt->getUeSecInfo().secinfo_m.int_key),
			NAS_INT_KEY_SIZE);

	tau_resp.m_tmsi = ue_ctxt->getMTmsi();
	ue_ctxt->setTai(Tai(tau_resp.tai));
	cmn::ipc::IpcAddress destAddr;
    	destAddr.u32 = TipcServiceInstance::s1apAppInstanceNum_c;

        MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));
        mmeIpcIf.dispatchIpcMsg((char *) &tau_resp, sizeof(tau_resp), destAddr);

	if( prcdCtxt_p->getCtxtType() != s1Handover_c)
	{
		MmeContextManagerUtils::deallocateProcedureCtxt(cb, tau_c );
	}
	ProcedureStats::num_of_tau_response_to_ue_sent++;

	log_msg(LOG_INFO,"Leaving send_tau_response_to_ue\n");
	return ActStatus::PROCEED;
}

