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
#include "mmeNasUtils.h"
#include "mme_app.h"
#include <utils/mmeCommonUtils.h>
#include "mmeStatsPromClient.h"

using namespace mme;
using namespace SM;
using namespace cmn;
using namespace cmn::utils;

extern mme_config_t *mme_cfg;
extern mmeConfig *mme_tables;

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
	tau_resp.ue_idx = ue_ctxt->getContextID();
	tau_resp.status = 0;

	struct Buffer nasBuffer;
	struct nasPDU nas = {0};
	const uint8_t num_nas_elements = 5;
	nas.elements = (nas_pdu_elements *) calloc(num_nas_elements, sizeof(nas_pdu_elements)); // TODO : should i use new ?
	nas.elements_len = num_nas_elements;

	nas.header.security_header_type = IntegrityProtectedCiphered;
	nas.header.proto_discriminator = EPSMobilityManagementMessages;
	/* placeholder for mac. mac value will be calculated later */
	uint8_t mac[MAC_SIZE] = {0};
	memcpy(nas.header.mac, mac, MAC_SIZE);
	nas.header.seq_no = ue_ctxt->getUeSecInfo().getDownlinkSeqNo(); 
	nas.dl_count = ue_ctxt->getUeSecInfo().getDownlinkCount();	
	ue_ctxt->getUeSecInfo().increment_downlink_count();

	nas.header.message_type = TauAccept;
	nas.elements[0].pduElement.eps_update_result = 0;
	nas.elements[1].pduElement.t3412 = 0x21;
	//nas.elements[2].pduElement.mi_guti = 0x21;
	//nas.elements[3].pduElement.mi_guti = 0x21; TAI LIST 
   /* Send the allocated GUTI to UE  */
	nas.elements[3].pduElement.mi_guti.odd_even_indication = 0;
	nas.elements[3].pduElement.mi_guti.id_type = 6;

	memcpy(&(nas.elements[3].pduElement.mi_guti.plmn_id),
			&(ue_ctxt->getTai().tai_m.plmn_id), 3); // ajaymerge - dont use sizeof(struct PLMN));
	nas.elements[3].pduElement.mi_guti.mme_grp_id = htons(mme_cfg->mme_group_id);
	nas.elements[3].pduElement.mi_guti.mme_code = mme_cfg->mme_code;
	nas.elements[3].pduElement.mi_guti.m_TMSI = htonl(ue_ctxt->getMTmsi());


	//nas.elements[4].pduElement. MS identity  tmsi 
	MmeNasUtils::encode_nas_msg(&nasBuffer, &nas, ue_ctxt->getUeSecInfo());
	memcpy(&tau_resp.nasMsgBuf[0], &nasBuffer.buf[0], nasBuffer.pos);
	tau_resp.nasMsgSize = nasBuffer.pos;
	free(nas.elements);
	
	ue_ctxt->setTai(Tai(tau_resp.tai)); /* ajaymerge --need careful reading here... Did i merge correctly ?? */
	cmn::ipc::IpcAddress destAddr;
    destAddr.u32 = TipcServiceInstance::s1apAppInstanceNum_c;

    mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_NAS_TAU_RESPONSE);
    MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));
    mmeIpcIf.dispatchIpcMsg((char *) &tau_resp, sizeof(tau_resp), destAddr);

	if( prcdCtxt_p->getCtxtType() != s1Handover_c)
	{
    	mmeStats::Instance()->increment(mmeStatsCounter::MME_PROCEDURES_TAU_PROC_SUCCESS);
		MmeContextManagerUtils::deallocateProcedureCtxt(cb, tau_c );
	}
	ProcedureStats::num_of_tau_response_to_ue_sent++;

	log_msg(LOG_INFO,"Leaving send_tau_response_to_ue\n");
	return ActStatus::PROCEED;
}

