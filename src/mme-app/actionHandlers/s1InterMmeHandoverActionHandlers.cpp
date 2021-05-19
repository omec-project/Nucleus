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
#include "controlBlock.h" 
#include "msgType.h"
using namespace mme;
using namespace SM;

/***************************************
* Action handler : send_fr_request_to_target_mme
***************************************/
ActStatus ActionHandlers::send_fr_request_to_target_mme(ControlBlock& cb)
{
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : process_fr_res
***************************************/
ActStatus ActionHandlers::process_fr_res(ControlBlock& cb)
{
    log_msg(LOG_DEBUG, "Inside forward_relocation_reponse ");

	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ue_ctxt, "Invalid UE");
    ProcedureStats::num_of_processed_attach_cmp_from_ue ++;
	log_msg(LOG_DEBUG, "Leaving handle_attach_cmp_from_ue ");

    
    if (ue_ctxt == NULL)
    {
        log_msg(LOG_ERROR, "forward_relocation_reponse: ue context is NULL",
                cb.getCBIndex());
        return ActStatus::HALT;
    }
    MsgBuffer *msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
    if (msgBuf == NULL)
    {
        log_msg(LOG_ERROR, "Failed to retrieve message buffer ");
        return ActStatus::HALT;
    }

    S1HandoverProcedureContext* s1HoPrCtxt = dynamic_cast<S1HandoverProcedureContext*>(cb.getTempDataBlock());
    if(s1HoPrCtxt==NULL)
    {  
        log_msg(LOG_DEBUG,"forward_relocation_reponse: S1HandoverProcedureContext is NULL");
        return ActStatus::HALT;

    }
    const forward_relocation_resp_BQ_msg *forward_rel_resp = static_cast<const forward_relocation_resp_BQ_msg*>(msgBuf->getDataPointer());
    s1HoPrCtxt->setTargetToSrcTransContainer(forward_rel_resp->eutran_container);
    ProcedureStats::num_of_fwd_relocation_resp_received++;

    return ActStatus::PROCEED;
    
}

/***************************************
* Action handler : process_enb_status_transfer
***************************************/
ActStatus ActionHandlers::process_enb_status_transfer(ControlBlock& cb)
{
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : send_fwd_acc_ctxt_noti_to_target_mme
***************************************/
ActStatus ActionHandlers::send_fwd_acc_ctxt_noti_to_target_mme(ControlBlock& cb)
{
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : process_fwd_acc_ctxt_ack
***************************************/
ActStatus ActionHandlers::process_fwd_acc_ctxt_ack(ControlBlock& cb)
{
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : process_fwd_rel_comp_notify
***************************************/
ActStatus ActionHandlers::process_fwd_rel_comp_notify(ControlBlock& cb)
{
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : send_fwd_rel_comp_ack_to_target_mme
***************************************/
ActStatus ActionHandlers::send_fwd_rel_comp_ack_to_target_mme(ControlBlock& cb)
{
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : send_s1_rel_cmd_to_ue_for_ho
***************************************/
ActStatus ActionHandlers::send_s1_rel_cmd_to_ue_for_ho(ControlBlock& cb)
{
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : send_ho_req_to_target_enb
***************************************/
ActStatus ActionHandlers::send_ho_req_to_target_enb(ControlBlock& cb)
{
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : process_ho_req_ack
***************************************/
ActStatus ActionHandlers::process_ho_req_ack(ControlBlock& cb)
{
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : send_fwd_rel_resp_to_src_mme
***************************************/
ActStatus ActionHandlers::send_fwd_rel_resp_to_src_mme(ControlBlock& cb)
{
    log_msg(LOG_DEBUG, "Inside send_fwd_rel_resp_to_src_mme");

	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ue_ctxt, "Invalid UE");
    struct forward_relocation_resp_Q_msg forward_relocation_resp;
     
    forward_relocation_resp.msg_type = identification_request;
	forward_relocation_resp.ue_idx = ue_ctxt->getContextID();

    forward_relocation_resp.f_cause = prcdCtxt_p->getS1apCause();
    memcpy(&(forward_relocation_resp.eutran_container),&(procCtxt.getSrcToTargetTransContainer()),sizeof(struct src_target_transparent_container));
    
    cmn::ipc::IpcAddress destAddr;
	destAddr.u32 = TipcServiceInstance::s10AppInstanceNum_c;

    mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_S10_FORWARD_RELOCATION_RESPONSE);
	MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));   
	mmeIpcIf.dispatchIpcMsg((char *) &id_msg, sizeof(id_msg), destAddr);
		
	ProcedureStats::num_of_fwd_relocation_resp_sent++;
	log_msg(LOG_DEBUG, "Leaving send_identification_request_to_old_mme ");

    
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : process_ho_fwd_acc_cntx_noti
***************************************/
ActStatus ActionHandlers::process_ho_fwd_acc_cntx_noti(ControlBlock& cb)
{
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : send_ho_fwd_acc_cntx_ack_to_src_mme
***************************************/
ActStatus ActionHandlers::send_ho_fwd_acc_cntx_ack_to_src_mme(ControlBlock& cb)
{
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : send_from_target_mme_status_tranfer_to_target_enb
***************************************/
ActStatus ActionHandlers::send_from_target_mme_status_tranfer_to_target_enb(ControlBlock& cb)
{
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : process_s1_ho_notify_from_target_enb
***************************************/
ActStatus ActionHandlers::process_s1_ho_notify_from_target_enb(ControlBlock& cb)
{
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : send_ho_fwd_rel_comp_notification_to_src_mme
***************************************/
ActStatus ActionHandlers::send_ho_fwd_rel_comp_notification_to_src_mme(ControlBlock& cb)
{
    log_msg(LOG_DEBUG, "Inside send_ho_fwd_rel_comp_notification_to_src_mme");

	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ue_ctxt, "Invalid UE");
    
    cmn::ipc::IpcAddress destAddr;
	destAddr.u32 = TipcServiceInstance::s10AppInstanceNum_c;

    mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_S10_FORWARD_RELOCATION_COMPLETE);
	MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));   
	mmeIpcIf.dispatchIpcMsg((char *) &id_msg, sizeof(id_msg), destAddr);
		
	ProcedureStats::num_of_fwd_relocation_complete_sent++;
	log_msg(LOG_DEBUG, "Leaving send_ho_fwd_rel_comp_notification_to_src_mme ");

    return ActStatus::PROCEED;
}

