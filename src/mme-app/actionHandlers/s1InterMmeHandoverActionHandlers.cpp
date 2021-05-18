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
#include "mme_app.h"
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
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : send_fwd_acc_ctxt_noti_to_target_mme
***************************************/
ActStatus ActionHandlers::send_fwd_acc_ctxt_noti_to_target_mme(ControlBlock& cb)
{
    log_msg(LOG_DEBUG, "Inside send_fwd_acc_ctxt_noti_to_target_mme");

	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ue_ctxt, "Invalid UE");

	struct FWD_ACC_CTXT_NOTIF_Q_msg fwd_acc_ctxt_msg;
	fwd_acc_ctxt_msg.msg_type = forward_access_context_notification;
	fwd_acc_ctxt_msg.ue_idx = ue_ctxt->getContextID();

    const enb_status_transfer_Q_msg_t  *enb_status_trans = static_cast<const enb_status_transfer_Q_msg_t*>(msgBuf->getDataPointer());
    memcpy(&(fwd_acc_ctxt_msg.f_container), &(enb_status_trans->enB_status_transfer_transparent_containerlist.enB_status_transfer_transparent_container),
        sizeof(struct enB_status_transfer_transparent_container));

	cmn::ipc::IpcAddress destAddr;
	destAddr.u32 = TipcServiceInstance::s10AppInstanceNum_c;

    mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_S10_FORWARD_ACCESS_CONTEXT_NOTIFY);
	MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));   
	mmeIpcIf.dispatchIpcMsg((char *) &fwd_acc_ctxt_msg, sizeof(fwd_acc_ctxt_msg), destAddr);
		
	ProcedureStats::num_of_fwd_access_context_notify_sent ++;
	log_msg(LOG_DEBUG, "Leaving send_fwd_acc_ctxt_noti_to_target_mme ");

    return ActStatus::PROCEED;
}

/***************************************
* Action handler : process_fwd_acc_ctxt_ack
***************************************/
ActStatus ActionHandlers::process_fwd_acc_ctxt_ack(ControlBlock& cb)
{
	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ue_ctxt, "Invalid UE");

    return ActStatus::PROCEED;
}

/***************************************
* Action handler : send_fwd_rel_comp_ack_to_target_mme
***************************************/
ActStatus ActionHandlers::send_fwd_rel_comp_ack_to_target_mme(ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside send_fwd_rel_comp_ack_to_target_mme");

	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ue_ctxt, "Invalid UE");
	
	struct FWD_REL_CMP_ACK_Q_msg fwd_rel_cmp_ack_msg;
	fwd_rel_cmp_ack_msg.msg_type = forward_relocation_complete_acknowledgement;
	fwd_rel_cmp_ack_msg.ue_idx = ue_ctxt->getContextID();
	uint8_t cause = GTPV2C_CAUSE_REQUEST_ACCEPTED;
	fwd_rel_cmp_ack_msg.cause = cause;
	
	cmn::ipc:IpcAddress destAddr;
	destAddr.u32 = TipcServiceInstance::s10AppInstanceNum_c;
	
	mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_S10_FORWARD_RELOCATION_COMPLETE_ACKNOWLEDGEMENT);
	MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId)); 
	mmeIpcIf.dispatchIpcMsg((char *) &fwd_rel_cmp_ack_msg,sizeof(fwd_rel_cmp_ack_msg), destAddr); 

	//ProcedureStats::num_of_fwd_access_context_notify_sent ++;
	log_msg(LOG_DEBUG, "Leaving send_fwd_rel_comp_ack_to_target_mme ");
	
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
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : send_ho_fwd_acc_cntx_ack_to_src_mme
***************************************/
ActStatus ActionHandlers::send_ho_fwd_acc_cntx_ack_to_src_mme(ControlBlock& cb)
{
    log_msg(LOG_DEBUG, "Inside send_ho_fwd_acc_cntx_ack_to_src_mme");

	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ue_ctxt, "Invalid UE");

	struct FWD_ACC_CTXT_ACK_Q_msg fwd_acc_ctxt_ack_msg;
	fwd_acc_ctxt_ack_msg.msg_type = forward_access_context_ack;
	fwd_acc_ctxt_ack_msg.ue_idx = ue_ctxt->getContextID();

    uint8_t gtpCause = GTPV2C_CAUSE_REQUEST_ACCEPTED;
    fwd_acc_ctxt_ack_msg.cause = gtpCause;

	cmn::ipc::IpcAddress destAddr;
	destAddr.u32 = TipcServiceInstance::s10AppInstanceNum_c;

    mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_S10_FORWARD_ACCESS_CONTEXT_ACK);
	MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));   
	mmeIpcIf.dispatchIpcMsg((char *) &fwd_acc_ctxt_ack_msg, sizeof(fwd_acc_ctxt_ack_msg), destAddr);
		
	ProcedureStats::num_of_fwd_access_context_ack_sent ++;
	log_msg(LOG_DEBUG, "Leaving send_ho_fwd_acc_cntx_ack_to_src_mme ");

    return ActStatus::PROCEED;
}

/***************************************
* Action handler : send_from_target_mme_status_tranfer_to_target_enb
***************************************/
ActStatus ActionHandlers::send_from_target_mme_status_tranfer_to_target_enb(ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside send_from_target_mme_status_tranfer_to_target_enb");
    UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
    if (ue_ctxt == NULL)
    {
        log_msg(LOG_DEBUG, "send_from_target_mme_status_tranfer_to_target_enb: ue ctxt is NULL ");
        return ActStatus::HALT;
    }

    S1HandoverProcedureContext *ho_ctxt =
            dynamic_cast<S1HandoverProcedureContext*>(cb.getTempDataBlock());
    if (ho_ctxt == NULL)
    {
        log_msg(LOG_DEBUG, "send_from_target_mme_status_tranfer_to_target_enb: procedure ctxt is NULL ");
        return ActStatus::HALT;
    }

    MsgBuffer *msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
    if (msgBuf == NULL)
    {
        log_msg(LOG_ERROR, "Failed to retrieve message buffer ");
        return ActStatus::HALT;
    }

    const enb_status_transfer_Q_msg_t *enb_status_trans = static_cast<const enb_status_transfer_Q_msg_t*>(msgBuf->getDataPointer());

    mme_status_transfer_Q_msg_t mme_status_trans;
    memset(&mme_status_trans, 0, sizeof(struct mme_status_transfer_Q_msg));

    mme_status_trans.msg_type = mme_status_transfer;
    mme_status_trans.target_enb_context_id = ho_ctxt->getTargetEnbContextId();
    mme_status_trans.s1ap_enb_ue_id = ho_ctxt->getTargetS1apEnbUeId();
    mme_status_trans.s1ap_mme_ue_id = ue_ctxt->getContextID();
    mme_status_trans.enB_status_transfer_transparent_containerlist.count =
    	enb_status_trans->enB_status_transfer_transparent_containerlist.count;
    memcpy(&(mme_status_trans.enB_status_transfer_transparent_containerlist.enB_status_transfer_transparent_container) ,
    	&(enb_status_trans->enB_status_transfer_transparent_containerlist.enB_status_transfer_transparent_container),
	sizeof(struct enB_status_transfer_transparent_container));

    mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_S1AP_MME_STATUS_TRANSFER);
    cmn::ipc::IpcAddress destAddr = {TipcServiceInstance::s1apAppInstanceNum_c};
    MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));
    mmeIpcIf.dispatchIpcMsg((char *) &mme_status_trans, sizeof(mme_status_trans), destAddr);

	log_msg(LOG_DEBUG, "Leaving send_from_target_mme_status_tranfer_to_target_enb");
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : process_s1_ho_notify_from_target_enb
***************************************/
ActStatus ActionHandlers::process_s1_ho_notify_from_target_enb(ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside process_s1_ho_notify_from_target_enb");
    UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
    if (ue_ctxt == NULL)
    {
        log_msg(LOG_DEBUG, "process_s1_ho_notify_from_target_enb: ue ctxt is NULL ");
        return ActStatus::HALT;
    }

    S1HandoverProcedureContext *ho_ctxt =
            dynamic_cast<S1HandoverProcedureContext*>(cb.getTempDataBlock());
    if (ho_ctxt == NULL)
    {
        log_msg(LOG_DEBUG, "process_s1_ho_notify_from_target_enb: procedure ctxt is NULL ");
        return ActStatus::HALT;
    }

    MsgBuffer *msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
    if (msgBuf == NULL)
    {
        log_msg(LOG_ERROR, "Failed to retrieve message buffer ");
        return ActStatus::HALT;
    }

    const handover_notify_Q_msg_t *ho_notify = static_cast<const handover_notify_Q_msg_t*>(msgBuf->getDataPointer());

    // The UE has synced to target cell. Set the current enb
    // to target enb.
    ue_ctxt->setEnbFd(ho_ctxt->getTargetEnbContextId());
    ue_ctxt->setS1apEnbUeId(ho_ctxt->getTargetS1apEnbUeId());

    // Wait till TAU complete to overwrite TAI?
    //TAI and CGI obtained from s1ap ies.
    //Convert the PLMN in s1ap format to nas format before storing in procedure context.
    MmeCommonUtils::formatS1apPlmnId(const_cast<PLMN*>(&ho_notify->tai.plmn_id));
    MmeCommonUtils::formatS1apPlmnId(const_cast<PLMN*>(&ho_notify->utran_cgi.plmn_id));
    ho_ctxt->setTargetTai(Tai(ho_notify->tai));
    ho_ctxt->setTargetCgi(Cgi(ho_notify->utran_cgi));

    ProcedureStats::num_of_ho_notify_received++;
    log_msg(LOG_DEBUG, "Leaving process_ho_notify");

    return ActStatus::PROCEED;
}

/***************************************
* Action handler : send_ho_fwd_rel_comp_notification_to_src_mme
***************************************/
ActStatus ActionHandlers::send_ho_fwd_rel_comp_notification_to_src_mme(ControlBlock& cb)
{
    return ActStatus::PROCEED;
}
