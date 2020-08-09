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

#include "actionHandlers/actionHandlers.h"
#include "contextManager/subsDataGroupManager.h"
#include "contextManager/dataBlocks.h"
#include "mme_app.h"
#include "msgType.h"
#include "controlBlock.h"
#include "procedureStats.h"
#include "log.h"
#include <string.h>
#include <mmeSmDefs.h>

#include <ipcTypes.h>
#include <tipcTypes.h>
#include <msgBuffer.h>
#include <interfaces/mmeIpcInterface.h>
#include <utils/mmeContextManagerUtils.h>
#include "mmeNasUtils.h"
#include "promClient.h"

using namespace SM;
using namespace mme;
using namespace cmn;
using namespace cmn::utils;

ActStatus ActionHandlers::del_session_req(SM::ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside delete_session_req \n");
		
	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	
	if (ue_ctxt == NULL)
	{
		log_msg(LOG_DEBUG, "delete_session_req: ue context is NULL\n");
		return ActStatus::HALT;
	}
		
	//ue_ctxt->getUeSecInfo().increment_uplink_count();
	
	struct DS_Q_msg g_ds_msg;
	g_ds_msg.msg_type = delete_session_request;
	g_ds_msg.ue_idx = ue_ctxt->getContextID();
	
	memset(g_ds_msg.indication, 0, S11_DS_INDICATION_FLAG_SIZE);
	g_ds_msg.indication[0] = 8; /* TODO : define macro or enum */
	
	SessionContext* sessionCtxt = ue_ctxt->getSessionContext();
	BearerContext* bearerCtxt = sessionCtxt->getBearerContext();
	g_ds_msg.bearer_id = bearerCtxt->getBearerId();

	memcpy(&(g_ds_msg.s11_sgw_c_fteid), &(sessionCtxt->getS11SgwCtrlFteid().fteid_m), sizeof(struct fteid));
		
	/* Send message to S11app in S11q*/
	cmn::ipc::IpcAddress destAddr;
	destAddr.u32 = TipcServiceInstance::s11AppInstanceNum_c;

    gtp_outgoing_msgs_t top_msg;
    top_msg.msg_type = g_ds_msg.msg_type;
    memcpy(&top_msg.dsr_req_msg, &g_ds_msg, sizeof(g_ds_msg)); 

    statistics::Instance()->Increment_s11_msg_tx_stats(msg_type_t::delete_session_request);
	MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));
	mmeIpcIf.dispatchIpcMsg((char *) &top_msg, sizeof(top_msg), destAddr);
	
	log_msg(LOG_DEBUG, "Leaving delete_session_req \n");
	ProcedureStats::num_of_del_session_req_sent ++;	
	return ActStatus::PROCEED;

}

ActStatus ActionHandlers::purge_req(SM::ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside purge_req \n");
	UEContext *ue_ctxt =  dynamic_cast<UEContext*>(cb.getPermDataBlock());
	
	if (ue_ctxt == NULL)
	{
		log_msg(LOG_DEBUG, "purge_req: ue context is NULL\n");
		return ActStatus::HALT;
	}
	
	s6a_Q_msg purge_msg;
	purge_msg.msg_type = purge_request;	
	purge_msg.ue_idx = ue_ctxt->getContextID();
	memset(purge_msg.imsi, '\0', sizeof(purge_msg.imsi));
	ue_ctxt->getImsi().getImsiDigits(purge_msg.imsi);
		
	/* Send message to S6app in S6q*/
	cmn::ipc::IpcAddress destAddr;
	destAddr.u32 = TipcServiceInstance::s6AppInstanceNum_c;

    statistics::Instance()->Increment_s6_msg_tx_stats(msg_type_t::purge_request);
    MmeIpcInterface &mmeIpcIf =static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));	
	mmeIpcIf.dispatchIpcMsg((char *) &purge_msg, sizeof(purge_msg), destAddr);
	
	log_msg(LOG_DEBUG, "Leaving purge_req \n");
	ProcedureStats::num_of_purge_req_sent ++;
	return ActStatus::PROCEED;
	
}


ActStatus ActionHandlers::process_del_session_resp(SM::ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside handle_delete_session_resp \n");
	
	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	if (ue_ctxt == NULL)
	{
	    log_msg(LOG_DEBUG, "delete_session_req: ue context is NULL\n");
	    return ActStatus::HALT;
	}

	SessionContext* sessionCtxt = ue_ctxt->getSessionContext();
	if (sessionCtxt != NULL)
	{
	    BearerContext* bearerCtxt = sessionCtxt->getBearerContext();
	    if (bearerCtxt)
	    {
	        SubsDataGroupManager::Instance()->deleteBearerContext( bearerCtxt );
	    }
	    SubsDataGroupManager::Instance()->deleteSessionContext( sessionCtxt );
	}

	ue_ctxt->setSessionContext(NULL);
	
	log_msg(LOG_DEBUG, "Leaving handle_delete_session_resp \n");
	ProcedureStats::num_of_processed_del_session_resp ++;
	return ActStatus::PROCEED;
	
}


ActStatus ActionHandlers::process_pur_resp(SM::ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside handle_purge_resp \n");
	
	UEContext *ue_ctxt =  dynamic_cast<UEContext*>(cb.getPermDataBlock());
	
	if (ue_ctxt == NULL)
	{
		log_msg(LOG_DEBUG, "handle_purge_resp: ue context is NULL \n");
		return ActStatus::HALT;
	}
	//struct purge_resp_Q_msg *purge_msg = nullptr;
	
	/*Nothing is been done. Only takes the UE Index
	 * increment the stats counter and changes the state*/
	
	
	log_msg(LOG_DEBUG, "Leaving handle_purge_resp for UE-%d.\n", ue_ctxt->getContextID());
	ProcedureStats::num_of_processed_pur_resp ++;
	return ActStatus::PROCEED;
	
}

ActStatus ActionHandlers::detach_accept_to_ue(SM::ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside send_detach_accept \n");
	
	UEContext *ue_ctxt =  dynamic_cast<UEContext*>(cb.getPermDataBlock());
	
	if (ue_ctxt == NULL)
	{
		log_msg(LOG_DEBUG, "send_detach_accept: ue context is NULL\n");
		return ActStatus::HALT;
	}
    statistics::Instance()->ue_detached(ue_ctxt);
	detach_accept_Q_msg detach_accpt;
	detach_accpt.msg_type = detach_accept;
	detach_accpt.enb_fd = ue_ctxt->getEnbFd();
	detach_accpt.ue_idx = ue_ctxt->getContextID();
	detach_accpt.enb_s1ap_ue_id =  ue_ctxt->getS1apEnbUeId();
	
	struct Buffer nasBuffer;
	struct nasPDU nas = {0};
	nas.header.security_header_type = IntegrityProtectedCiphered;
	nas.header.proto_discriminator = EPSMobilityManagementMessages;
	uint8_t mac[MAC_SIZE] = {0};
	memcpy(nas.header.mac, mac, MAC_SIZE);
	nas.header.seq_no = ue_ctxt->getUeSecInfo().getDownlinkSeqNo(); 
	nas.dl_count = ue_ctxt->getUeSecInfo().getDownlinkCount();	
	ue_ctxt->getUeSecInfo().increment_downlink_count();

	nas.header.message_type = DetachAccept;
	nas.header.nas_security_param = AUTHREQ_NAS_SECURITY_PARAM;

	MmeNasUtils::encode_nas_msg(&nasBuffer, &nas, ue_ctxt->getUeSecInfo());
	memcpy(&detach_accpt.nasMsgBuf[0], &nasBuffer.buf[0], nasBuffer.pos);
	detach_accpt.nasMsgSize = nasBuffer.pos;
	
	/* Send message to S11app in S11q*/
	cmn::ipc::IpcAddress destAddr;
	destAddr.u32 = TipcServiceInstance::s1apAppInstanceNum_c;

    statistics::Instance()->Increment_s1ap_msg_tx_stats(msg_type_t::detach_accept);
	MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));
	mmeIpcIf.dispatchIpcMsg((char *) &detach_accpt, sizeof(detach_accpt), destAddr);
	
	MmeContextManagerUtils::deallocateProcedureCtxt(cb, detach_c );

	MmContext* mmCtxt = ue_ctxt->getMmContext();
	mmCtxt->setMmState( EpsDetached );
	mmCtxt->setEcmState( ecmIdle_c );

	log_msg(LOG_DEBUG, "Leaving send_detach_accept for UE \n");
	ue_ctxt->setS1apEnbUeId(0);
	ProcedureStats::num_of_detach_accept_to_ue_sent ++;
	ProcedureStats::num_of_subscribers_detached ++;
	if (ProcedureStats::num_of_subscribers_attached > 0)
		ProcedureStats::num_of_subscribers_attached --;

	return ActStatus::PROCEED;
	
}
