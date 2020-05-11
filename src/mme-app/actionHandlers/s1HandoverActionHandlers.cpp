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

using namespace cmn;
using namespace cmn::utils;
using namespace mme;
using namespace SM;

/***************************************
 * Action handler : send_ho_request_to_target_enb
 ***************************************/
ActStatus ActionHandlers::send_ho_request_to_target_enb(ControlBlock &cb)
{
    log_msg(LOG_INFO, "Inside send_ho_request_to_target_enb\n");

    UEContext *ueCtxt = static_cast<UEContext*>(cb.getPermDataBlock());

    if (ueCtxt == NULL)
    {
        log_msg(LOG_DEBUG,
                "send_ho_request_to_target_enb: ue context is NULL\n");
        return ActStatus::HALT;
    }

    S1HandoverProcedureContext *hoProcCtxt =
            dynamic_cast<S1HandoverProcedureContext*>(cb.getTempDataBlock());
    if (hoProcCtxt == NULL)
    {
        log_msg(LOG_DEBUG,
                "send_ho_request_to_target_enb: MmeS1HandoverProcedureCtxt is NULL\n");
        return ActStatus::HALT;
    }

    struct handover_request_Q_msg hoReq;
    memset(&hoReq, 0, sizeof(struct handover_request_Q_msg));

    MmeS1MsgUtils::populateHoRequest(cb, *ueCtxt, *hoProcCtxt, hoReq);

    /*Send message to S1AP-APP*/
    cmn::ipc::IpcAddress destAddr = {TipcServiceInstance::s1apAppInstanceNum_c};
    MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));
    mmeIpcIf.dispatchIpcMsg((char *) &hoReq, sizeof(hoReq), destAddr);

    log_msg(LOG_DEBUG, "Leaving send_ho_request_to_target_enb \n");

    ProcedureStats::num_of_ho_request_to_target_enb_sent++;
    return ActStatus::PROCEED;
}

/***************************************
 * Action handler : process_ho_request_ack
 ***************************************/
ActStatus ActionHandlers::process_ho_request_ack(ControlBlock &cb)
{
    log_msg(LOG_DEBUG, "Inside process_ho_request_ack\n");

    UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
    if (ue_ctxt == NULL)
    {
        log_msg(LOG_DEBUG, "process_ho_request_ack: ue ctxt is NULL \n");
        return ActStatus::HALT;
    }

    S1HandoverProcedureContext *ho_ctxt =
            dynamic_cast<S1HandoverProcedureContext*>(cb.getTempDataBlock());
    if (ho_ctxt == NULL)
    {
        log_msg(LOG_DEBUG, "process_ho_request_ack: procedure ctxt is NULL \n");
        return ActStatus::HALT;
    }

    MsgBuffer *msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
    if (msgBuf == NULL)
    {
        log_msg(LOG_ERROR, "Failed to retrieve message buffer \n");
        return ActStatus::HALT;
    }

    const s1_incoming_msg_data_t *msgData_p =
            static_cast<const s1_incoming_msg_data_t*>(msgBuf->getDataPointer());
    if (msgData_p == NULL)
    {
        log_msg(LOG_ERROR, "Failed to retrieve data buffer \n");
        return ActStatus::HALT;
    }

    const struct handover_req_acknowledge_Q_msg &ho_request_ack =
            (msgData_p->msg_data.handover_req_acknowledge_Q_msg_m);

    ho_ctxt->setTargetS1apEnbUeId(ho_request_ack.s1ap_enb_ue_id);
    if (ho_request_ack.erab_admitted_list.count == 0)
    {
        log_msg(LOG_INFO, "HO Request ACK does not contain any eRAB Admitted Items.");

        ho_ctxt->setMmeErrorCause(hoRequestAckFailure_c);
       	S1apCause s1apCause = MmeCauseUtils::convertToS1apCause(
                ho_ctxt->getMmeErrorCause());
        ho_ctxt->setS1HoCause(s1apCause);

        return ActStatus::ABORT;
    }
    ho_ctxt->setTargetToSrcTransContainer(
            ho_request_ack.targetToSrcTranspContainer);
    ho_ctxt->setErabAdmittedItem(
            ho_request_ack.erab_admitted_list.erab_admitted[0]);

    ProcedureStats::num_of_ho_request_ack_received++;

    log_msg(LOG_DEBUG, "Leaving process_ho_request_ack\n");
    return ActStatus::PROCEED;
}

/***************************************
 * Action handler : send_ho_command_to_src_enb
 ***************************************/
ActStatus ActionHandlers::send_ho_command_to_src_enb(ControlBlock &cb)
{
    log_msg(LOG_DEBUG, "Inside send_ho_command_to_src_enb\n");

    UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
    if (ue_ctxt == NULL)
    {
        log_msg(LOG_DEBUG, "send_ho_command_to_src_enb: ue ctxt is NULL \n");
        return ActStatus::HALT;
    }

    S1HandoverProcedureContext *ho_ctxt =
            dynamic_cast<S1HandoverProcedureContext*>(cb.getTempDataBlock());
    if (ho_ctxt == NULL)
    {
        log_msg(LOG_DEBUG,
                "send_ho_command_to_src_enb: procedure ctxt is NULL \n");
        return ActStatus::HALT;
    }

    struct handover_command_Q_msg ho_command;
    memset(&ho_command, 0, sizeof(struct handover_command_Q_msg));

    MmeS1MsgUtils::populateHoCommand(cb, *ue_ctxt, *ho_ctxt, ho_command);

    cmn::ipc::IpcAddress destAddr = {TipcServiceInstance::s1apAppInstanceNum_c};
    MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));
    mmeIpcIf.dispatchIpcMsg((char *) &ho_command, sizeof(ho_command), destAddr);

    ProcedureStats::num_of_ho_command_to_src_enb_sent++;
    log_msg(LOG_DEBUG, "Leaving send_ho_command_to_src_enb\n");

    return ActStatus::PROCEED;
}

/***************************************
* Action handler : send_mme_status_tranfer_to_target_enb
***************************************/
ActStatus ActionHandlers::send_mme_status_tranfer_to_target_enb(ControlBlock& cb)
{
    log_msg(LOG_DEBUG, "Inside send_mme_status_tranfer_to_target_enb\n");
    UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
    if (ue_ctxt == NULL)
    {
        log_msg(LOG_DEBUG, "send_mme_status_tranfer_to_target_enb: ue ctxt is NULL \n");
        return ActStatus::HALT;
    }

    S1HandoverProcedureContext *ho_ctxt =
            dynamic_cast<S1HandoverProcedureContext*>(cb.getTempDataBlock());
    if (ho_ctxt == NULL)
    {
        log_msg(LOG_DEBUG, "send_mme_status_tranfer_to_target_enb: procedure ctxt is NULL \n");
        return ActStatus::HALT;
    }

    MsgBuffer *msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
    if (msgBuf == NULL)
    {
        log_msg(LOG_ERROR, "Failed to retrieve message buffer \n");
        return ActStatus::HALT;
    }

    const s1_incoming_msg_data_t *msgData_p =
            static_cast<const s1_incoming_msg_data_t*>(msgBuf->getDataPointer());
    if (msgData_p == NULL)
    {
        log_msg(LOG_ERROR, "Failed to retrieve data buffer \n");
        return ActStatus::HALT;
    }

    const struct enb_status_transfer_Q_msg & enb_status_trans =
            (msgData_p->msg_data.enb_status_transfer_Q_msg_m);

    struct mme_status_transfer_Q_msg mme_status_trans;
    memset(&mme_status_trans, 0, sizeof(struct mme_status_transfer_Q_msg));

    mme_status_trans.msg_type = mme_status_transfer;
    mme_status_trans.target_enb_context_id = ho_ctxt->getTargetEnbContextId();
    mme_status_trans.s1ap_enb_ue_id = ho_ctxt->getTargetS1apEnbUeId();
    mme_status_trans.s1ap_mme_ue_id = ue_ctxt->getContextID();
    mme_status_trans.enB_status_transfer_transparent_containerlist.count =
    	enb_status_trans.enB_status_transfer_transparent_containerlist.count;
    memcpy(&(mme_status_trans.enB_status_transfer_transparent_containerlist.enB_status_transfer_transparent_container) ,
    	&(enb_status_trans.enB_status_transfer_transparent_containerlist.enB_status_transfer_transparent_container),
	sizeof(struct enB_status_transfer_transparent_container));

    cmn::ipc::IpcAddress destAddr = {TipcServiceInstance::s1apAppInstanceNum_c};
    MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));
    mmeIpcIf.dispatchIpcMsg((char *) &mme_status_trans, sizeof(mme_status_trans), destAddr);
    return ActStatus::PROCEED;
}

/***************************************
 * Action handler : process_ho_notify
 ***************************************/
ActStatus ActionHandlers::process_ho_notify(ControlBlock &cb)
{
    log_msg(LOG_DEBUG, "Inside process_ho_notify\n");
    UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
    if (ue_ctxt == NULL)
    {
        log_msg(LOG_DEBUG, "process_ho_notify: ue ctxt is NULL \n");
        return ActStatus::HALT;
    }

    S1HandoverProcedureContext *ho_ctxt =
            dynamic_cast<S1HandoverProcedureContext*>(cb.getTempDataBlock());
    if (ho_ctxt == NULL)
    {
        log_msg(LOG_DEBUG, "process_ho_notify: procedure ctxt is NULL \n");
        return ActStatus::HALT;
    }

    MsgBuffer *msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
    if (msgBuf == NULL)
    {
        log_msg(LOG_ERROR, "Failed to retrieve message buffer \n");
        return ActStatus::HALT;
    }

    const s1_incoming_msg_data_t *msgData_p =
            static_cast<const s1_incoming_msg_data_t*>(msgBuf->getDataPointer());
    if (msgData_p == NULL)
    {
        log_msg(LOG_ERROR, "Failed to retrieve data buffer \n");
        return ActStatus::HALT;
    }

    const struct handover_notify_Q_msg &ho_notify =
            (msgData_p->msg_data.handover_notify_Q_msg_m);

    // The UE has synced to target cell. Set the current enb
    // to target enb.
    ue_ctxt->setEnbFd(ho_ctxt->getTargetEnbContextId());
    ue_ctxt->setS1apEnbUeId(ho_ctxt->getTargetS1apEnbUeId());

    // Wait till TAU complete to overwrite TAI?
    //TAI and CGI obtained from s1ap ies.
    //Convert the PLMN in s1ap format to nas format before storing in procedure context.
    MmeCommonUtils::formatS1apPlmnId(const_cast<PLMN*>(&ho_notify.tai.plmn_id));
    MmeCommonUtils::formatS1apPlmnId(const_cast<PLMN*>(&ho_notify.utran_cgi.plmn_id));
    ho_ctxt->setTargetTai(Tai(ho_notify.tai));
    ho_ctxt->setTargetCgi(Cgi(ho_notify.utran_cgi));

    ProcedureStats::num_of_ho_notify_received++;
    log_msg(LOG_DEBUG, "Leaving process_ho_notify\n");

    return ActStatus::PROCEED;
}

/********************************************
 * Action handler : send_mb_req_to_sgw_for_ho
 ********************************************/
ActStatus ActionHandlers::send_mb_req_to_sgw_for_ho(ControlBlock &cb)
{
    log_msg(LOG_DEBUG, "Inside send_mb_req_to_sgw_for_ho \n");

    UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
    if (ue_ctxt == NULL)
    {
        log_msg(LOG_DEBUG,
                "send_mb_req_to_sgw_for_ho: ue context or procedure ctxt is NULL \n");
        return ActStatus::HALT;
    }

    S1HandoverProcedureContext *ho_ctxt =
            dynamic_cast<S1HandoverProcedureContext*>(cb.getTempDataBlock());
    if (ho_ctxt == NULL)
    {
        log_msg(LOG_DEBUG, "process_ho_notify: procedure ctxt is NULL \n");
        return ActStatus::HALT;
    }
    SessionContext *sessionCtxt = ue_ctxt->getSessionContext();
    if (sessionCtxt == NULL)
    {
        log_msg(LOG_DEBUG,
                "send_mb_req_to_sgw_for_ho: session ctxt is NULL \n");
        return ActStatus::HALT;
    }

    struct MB_Q_msg mb_msg;
    MmeGtpMsgUtils::populateModifyBearerRequestHo(
            cb, *ue_ctxt, *sessionCtxt, *ho_ctxt, mb_msg);

    cmn::ipc::IpcAddress destAddr = {TipcServiceInstance::s11AppInstanceNum_c};
    MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));
    mmeIpcIf.dispatchIpcMsg((char *) &mb_msg, sizeof(mb_msg), destAddr);

    ProcedureStats::num_of_mb_req_to_sgw_sent++;
    log_msg(LOG_DEBUG, "Leaving send_mb_req_to_sgw_for_ho \n");
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : process_mb_resp_for_ho
***************************************/
ActStatus ActionHandlers::process_mb_resp_for_ho(ControlBlock& cb)
{
    log_msg(LOG_DEBUG, "Inside process_mb_resp_for_ho \n");

    ProcedureStats::num_of_processed_mb_resp ++;
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : send_s1_rel_cmd_to_ue_for_ho
***************************************/
ActStatus ActionHandlers::send_s1_rel_cmd_to_src_enb_for_ho(ControlBlock& cb)
{
    log_msg(LOG_DEBUG, "Inside send_s1_rel_cmd_to_ue_for_ho\n");
    UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
    if(ue_ctxt == NULL)
    {
    	log_msg(LOG_DEBUG, "send_s1_rel_cmd_to_ue_for_ho: ue context is NULL \n");
    	return ActStatus::HALT;
    }

    S1HandoverProcedureContext *ho_ctxt =
            dynamic_cast<S1HandoverProcedureContext*>(cb.getTempDataBlock());
    if (ho_ctxt == NULL)
    {
        log_msg(LOG_DEBUG, "send_s1_rel_cmd_to_ue_for_ho: procedure ctxt is NULL \n");
        return ActStatus::HALT;
    }
    struct s1relcmd_info s1relcmd;
    s1relcmd.msg_type = s1_release_command;
    s1relcmd.ue_idx = ue_ctxt->getContextID();
    s1relcmd.enb_s1ap_ue_id = ho_ctxt->getSrcS1apEnbUeId();
    s1relcmd.cause.present = s1apCause_PR_radioNetwork;
    s1relcmd.cause.choice.radioNetwork = s1apCauseRadioNetwork_successful_handover;
    s1relcmd.enb_fd = ho_ctxt->getSrcEnbContextId();

    // Fire and forget s1 release to src enb

    /*Send message to S1AP-APP*/
    cmn::ipc::IpcAddress destAddr = {TipcServiceInstance::s1apAppInstanceNum_c};
    MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));
    mmeIpcIf.dispatchIpcMsg((char *) &s1relcmd, sizeof(s1relcmd), destAddr);

    ProcedureStats::num_of_s1_rel_cmd_sent ++;
    log_msg(LOG_DEBUG,"Leaving send_s1ap_ue_ctxt_rel_command_to_src_enb \n");
    return ActStatus::PROCEED;
}

/***************************************
 * Action handler : ho_complete
 ***************************************/
ActStatus ActionHandlers::ho_complete(ControlBlock &cb)
{
    log_msg(LOG_INFO, "Inside ho_complete\n");

    ProcedureStats::num_of_ho_complete++;

    MmeContextManagerUtils::deallocateProcedureCtxt(cb, s1Handover_c);

    return ActStatus::PROCEED;
}

/***************************************
* Action handler : is_tau_required
***************************************/
ActStatus ActionHandlers::is_tau_required(ControlBlock& cb)
{
    log_msg(LOG_INFO, "Inside is_tau_required\n");
    UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
    if (ue_ctxt == NULL)
    {
        log_msg(LOG_ERROR, "is_tau_required: ue context is NULL\n",
                cb.getCBIndex());
        return ActStatus::HALT;
    }

    S1HandoverProcedureContext *s1HoPrcdCtxt_p =
            dynamic_cast<S1HandoverProcedureContext*>(cb.getTempDataBlock());
    if (s1HoPrcdCtxt_p == NULL)
    {
        log_msg(LOG_DEBUG,
                "is_tau_required: S1HandoverProcedureContext is NULL\n");
        return ActStatus::HALT;
    }

    if (ue_ctxt->getTai() == s1HoPrcdCtxt_p->getTargetTai())
    {
        log_msg(LOG_DEBUG, "TAI is same, TAU not Required\n");
        SM::Event evt(TAU_NOT_REQUIRED, NULL);
        cb.addEventToProcQ(evt);
    }
    else
    {
        log_msg(LOG_DEBUG, "TAI is not same, TAU Required\n");
        SM::Event evt(TAU_REQUIRED, NULL);
        cb.addEventToProcQ(evt);
    }

    return ActStatus::PROCEED;
}

/***************************************
 * Action handler : process_ho_failure
 ***************************************/
ActStatus ActionHandlers::process_ho_failure(ControlBlock &cb)
{
    S1HandoverProcedureContext *ho_ctxt =
            dynamic_cast<S1HandoverProcedureContext*>(cb.getTempDataBlock());
    if (ho_ctxt == NULL)
    {
        log_msg(LOG_DEBUG, "process_ho_failure: procedure ctxt is NULL \n");
        return ActStatus::HALT;
    }

    MsgBuffer *msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
    if (msgBuf == NULL)
    {
        log_msg(LOG_ERROR, "Failed to retrieve message buffer \n");
        return ActStatus::HALT;
    }
    const s1_incoming_msg_data_t *msgData_p =
            static_cast<const s1_incoming_msg_data_t*>(msgBuf->getDataPointer());
    if (msgData_p == NULL)
    {
        log_msg(LOG_ERROR, "Failed to retrieve data buffer \n");
        return ActStatus::HALT;
    }

    const struct handover_failure_Q_msg &ho_failure =
            (msgData_p->msg_data.handover_failure_Q_msg_m);
    ho_ctxt->setS1HoCause(S1apCause(ho_failure.cause));

    ProcedureStats::num_of_ho_failure_received++;

    log_msg(LOG_INFO, "Leaving process_ho_failure\n");
    return ActStatus::PROCEED;
}

/***************************************
 * Action handler : send_ho_prep_failure_to_src_enb
 ***************************************/
ActStatus ActionHandlers::send_ho_prep_failure_to_src_enb(ControlBlock &cb)
{
    log_msg(LOG_DEBUG, "Inside send_ho_prep_failure_to_src_enb\n");

    UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
    if (ue_ctxt == NULL)
    {
        log_msg(LOG_DEBUG,
                "send_ho_prep_failure_to_src_enb: ue ctxt is NULL \n");
        return ActStatus::HALT;
    }

    S1HandoverProcedureContext *ho_ctxt =
            dynamic_cast<S1HandoverProcedureContext*>(cb.getTempDataBlock());
    if (ho_ctxt == NULL)
    {
        log_msg(LOG_DEBUG,
                "send_ho_prep_failure_to_src_enb: procedure ctxt is NULL \n");
        return ActStatus::HALT;
    }

    struct handover_preparation_failure_Q_msg ho_prep_failure;
    memset(&ho_prep_failure, 0,
            sizeof(struct handover_preparation_failure_Q_msg));
    ho_prep_failure.src_enb_context_id = ho_ctxt->getSrcEnbContextId();
    ho_prep_failure.msg_type = handover_preparation_failure;
    ho_prep_failure.s1ap_enb_ue_id = ho_ctxt->getSrcS1apEnbUeId();
    ho_prep_failure.s1ap_mme_ue_id = ue_ctxt->getContextID();
    ho_prep_failure.cause = ho_ctxt->getS1HoCause().s1apCause_m;

    cmn::ipc::IpcAddress destAddr =
    { TipcServiceInstance::s1apAppInstanceNum_c };
    MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));   
    mmeIpcIf.dispatchIpcMsg((char *) &ho_prep_failure, sizeof(ho_prep_failure), destAddr);
    ProcedureStats::num_of_ho_prep_failure_sent++;
    log_msg(LOG_DEBUG, "Leaving send_ho_prep_failure_to_src_enb\n");

    return ActStatus::PROCEED;
}

/***************************************
 * Action handler : abort_handover
 ***************************************/
ActStatus ActionHandlers::abort_handover(ControlBlock &cb)
{
    MmeContextManagerUtils::deallocateProcedureCtxt(cb, s1Handover_c);
    return ActStatus::PROCEED;
}

/***************************************
 * Action handler : send_s1_rel_cmd_to_target_enb
 ***************************************/
ActStatus ActionHandlers::send_s1_rel_cmd_to_target_enb(ControlBlock &cb)
{
    log_msg(LOG_DEBUG, "Inside send_s1_rel_cmd_to_target_enb\n");

    UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
    if (ue_ctxt == NULL)
    {
        log_msg(LOG_DEBUG,
                "send_s1_rel_cmd_to_target_enb: ue context is NULL \n");
        return ActStatus::HALT;
    }

    S1HandoverProcedureContext *ho_ctxt =
            dynamic_cast<S1HandoverProcedureContext*>(cb.getTempDataBlock());
    if (ho_ctxt == NULL)
    {
        log_msg(LOG_DEBUG,
                "send_s1_rel_cmd_to_target_enb: procedure ctxt is NULL \n");
        return ActStatus::HALT;
    }
   
    struct s1relcmd_info s1relcmd;
    s1relcmd.msg_type = s1_release_command;
    s1relcmd.ue_idx = ue_ctxt->getContextID();
    s1relcmd.enb_s1ap_ue_id = ho_ctxt->getTargetS1apEnbUeId();
    s1relcmd.cause = ho_ctxt->getS1HoCause().s1apCause_m;
    s1relcmd.enb_fd = ho_ctxt->getTargetEnbContextId();

    // Fire and forget s1 release to target enb

    /*Send message to S1AP-APP*/
    cmn::ipc::IpcAddress destAddr =
    { TipcServiceInstance::s1apAppInstanceNum_c };
    MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));   
    mmeIpcIf.dispatchIpcMsg((char *) &s1relcmd, sizeof(s1relcmd), destAddr);

    ProcedureStats::num_of_s1_rel_cmd_sent++;
    log_msg(LOG_DEBUG, "Leaving send_s1_rel_cmd_to_target_enb \n");
    return ActStatus::PROCEED;
}

/***************************************
 * Action handler : process_ho_cancel_req
 ***************************************/
ActStatus ActionHandlers::process_ho_cancel_req(ControlBlock &cb)
{
    S1HandoverProcedureContext *ho_ctxt =
            dynamic_cast<S1HandoverProcedureContext*>(cb.getTempDataBlock());
    if (ho_ctxt == NULL)
    {
        log_msg(LOG_DEBUG, "process_ho_cancel_req: procedure ctxt is NULL \n");
        return ActStatus::HALT;
    }

    MsgBuffer *msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
    if (msgBuf == NULL)
    {
        log_msg(LOG_ERROR, "Failed to retrieve message buffer \n");
        return ActStatus::HALT;
    }
    const s1_incoming_msg_data_t *msgData_p =
            static_cast<const s1_incoming_msg_data_t*>(msgBuf->getDataPointer());
    if (msgData_p == NULL)
    {
        log_msg(LOG_ERROR, "Failed to retrieve data buffer \n");
        return ActStatus::HALT;
    }

    const struct handover_cancel_Q_msg &ho_cancel =
            (msgData_p->msg_data.handover_cancel_Q_msg_m);
    ho_ctxt->setS1HoCause(S1apCause(ho_cancel.cause));

    ProcedureStats::num_of_ho_cancel_received++;

    log_msg(LOG_INFO, "Leaving process_ho_cancel_req\n");
    return ActStatus::PROCEED;
}

/***************************************
 * Action handler : send_ho_cancel_ack_to_src_enb
 ***************************************/
ActStatus ActionHandlers::send_ho_cancel_ack_to_src_enb(ControlBlock &cb)
{
    log_msg(LOG_DEBUG, "Inside send_ho_cancel_ack_to_src_enb\n");

    UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
    if (ue_ctxt == NULL)
    {
        log_msg(LOG_DEBUG, "send_ho_cancel_ack_to_src_enb: ue ctxt is NULL \n");
        return ActStatus::HALT;
    }

    S1HandoverProcedureContext *ho_ctxt =
            dynamic_cast<S1HandoverProcedureContext*>(cb.getTempDataBlock());
    if (ho_ctxt == NULL)
    {
        log_msg(LOG_DEBUG,
                "send_ho_cancel_ack_to_src_enb: procedure ctxt is NULL \n");
        return ActStatus::HALT;
    }

    struct handover_cancel_ack_Q_msg ho_cancel_ack;
    memset(&ho_cancel_ack, 0, sizeof(struct handover_cancel_ack_Q_msg));

    ho_cancel_ack.src_enb_context_id = ho_ctxt->getSrcEnbContextId();
    ho_cancel_ack.msg_type = handover_cancel_ack;
    ho_cancel_ack.s1ap_enb_ue_id = ho_ctxt->getSrcS1apEnbUeId();
    ho_cancel_ack.s1ap_mme_ue_id = ue_ctxt->getContextID();

    cmn::ipc::IpcAddress destAddr =
    { TipcServiceInstance::s1apAppInstanceNum_c };
    MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));   
    mmeIpcIf.dispatchIpcMsg((char *) &ho_cancel_ack, sizeof(ho_cancel_ack), destAddr);
    ProcedureStats::num_of_ho_cancel_ack_sent++;
    log_msg(LOG_DEBUG, "Leaving send_ho_cancel_ack_to_src_enb\n");

    return ActStatus::PROCEED;
}
