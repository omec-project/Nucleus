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
    //////////////Complete after msg structure creation is done//////////////////////
    log_msg(LOG_DEBUG, "Entering process_fr_res ");

	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ue_ctxt, "Invalid UE");
	
	S1HandoverProcedureContext *interMmeHoProcCtxt =
            dynamic_cast<S1HandoverProcedureContext*>(cb.getTempDataBlock());
	if (interMmeHoProcCtxt == NULL)
    {
        log_msg(LOG_DEBUG,
                "process_fr_res: MmeS1HandoverProcedureCtxt is NULL");
        return ActStatus::HALT;
    }

	MsgBuffer* msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
	VERIFY(msgBuf, return ActStatus::ABORT, "Invalid message buffer ");
	VERIFY(msgBuf->getLength() >= sizeof(struct id_resp_Q_msg), return ActStatus::ABORT, "Invalid CSRsp message length ");

	const struct id_resp_Q_msg* id_resp_info = static_cast<const struct id_resp_Q_msg*>(msgBuf->getDataPointer());

    /*if(id_resp_info->status != GTPV2C_CAUSE_REQUEST_ACCEPTED)
    {
		log_msg(LOG_DEBUG, "IDRsp rejected by MME with cause %d ",id_resp_info->status);
        std::ostringstream reason;
        reason<<"IDRsp_reject_cause_"<<id_resp_info->status;
        mmeStats::Instance()->increment(mmeStatsCounter::MME_PROCEDURES_ATTACH_PROC_FAILURE, {{"failure_reason", reason.str()}});
       	return ActStatus::ABORT;
    }*/

	uint8_t imsi[BINARY_IMSI_LEN] = {0};
	memcpy( imsi, id_resp_info->IMSI, BINARY_IMSI_LEN );

	// Only upper nibble of first octect in imsi need to be considered
	// Changing the lower nibble to 0x0f for handling
	uint8_t first = imsi[0] >> 4;
	imsi[0] = (uint8_t)(( first << 4 ) | 0x0f );

	DigitRegister15 IMSIInfo;
	IMSIInfo.convertFromBcdArray(imsi);
	ue_ctxt->setImsi(IMSIInfo);

	SubsDataGroupManager::Instance()->addimsikey(ue_ctxt->getImsi(), ue_ctxt->getContextID());	
	ProcedureStats::num_of_processed_identification_resp++;
	log_msg(LOG_DEBUG, "Leaving process_identification_response");

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
    return ActStatus::PROCEED;
}
