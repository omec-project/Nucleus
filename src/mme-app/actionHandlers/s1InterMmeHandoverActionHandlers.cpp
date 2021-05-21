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
#include "gtpCauseTypes.h"


using namespace cmn;
using namespace cmn::utils;
using namespace mme;
using namespace SM;

extern mmeConfig *mme_tables;
/***************************************
* Action handler : send_fr_request_to_target_mme
***************************************/
ActStatus ActionHandlers::send_fr_request_to_target_mme(ControlBlock& cb)
{
#ifdef S10_FEATURE
    log_msg(LOG_DEBUG, "Inside send_fr_request_to_target_mme");

    UEContext *ueCtxt = static_cast<UEContext*>(cb.getPermDataBlock());

    if (ueCtxt == NULL)
    {
        log_msg(LOG_DEBUG,
                "send_fr_request_to_target_mme: ue context is NULL");
        return ActStatus::HALT;
    }

    S1HandoverProcedureContext *hoProcCtxt =
            dynamic_cast<S1HandoverProcedureContext*>(cb.getTempDataBlock());
    if (hoProcCtxt == NULL)
    {
        log_msg(LOG_DEBUG,
                "send_fr_request_to_target_mme: MmeS1HandoverProcedureCtxt is NULL");
        return ActStatus::HALT;
    }

    auto& sessionCtxtContainer = ueCtxt->getSessionContextContainer();
    if(sessionCtxtContainer.size() < 1)
    {
        log_msg(LOG_DEBUG, "send_fr_request_to_target_mme: Session context list empty");
        return ActStatus::HALT;
    }

    SessionContext* sessionCtxt = sessionCtxtContainer.front();

    struct forward_relocation_req_Q_msg frReq;
    memset(&frReq, 0, sizeof(struct forward_relocation_req_Q_msg));

    // msg_type
    frReq.msg_type = forward_relocation_request;
    frReq.ue_idx = ueCtxt->getContextID();
    frReq.target_enb_context_id = hoProcCtxt->getTargetEnbContextId();
    frReq.cause.choice = hoProcCtxt->getS1HoCause().s1apCause_m.choice;
    memcpy(&(frReq.srcToTargetTranspContainer),
                &(hoProcCtxt->getSrcToTargetTransContainer()),
                sizeof(struct src_target_transparent_container));
    memcpy(&frReq.tai, &(hoProcCtxt->getTargetTai().tai_m), sizeof(struct TAI));

    // IMSI
    const DigitRegister15& ueImsi = ueCtxt->getImsi();
    ueImsi.convertToBcdArray( frReq.IMSI );

    //PDN connections
    //APN
    const Apn_name &apnName = ueCtxt->getSubscribedApn();
    memcpy(&(frReq.selected_apn), &(apnName.apnname_m), sizeof(struct apn_name));
    const unsigned char *ptr = &apnName.apnname_m.val[1];
    std::string temp_str((char *)ptr);
    apn_config *temp = mme_tables->find_apn(temp_str);
    if(temp != NULL)
    {
        frReq.sgw_ip = temp->get_sgw_addr();
        frReq.pgw_ip = temp->get_pgw_addr();
    }
    else
    {
        log_msg(LOG_DEBUG, "APN not found in static apn configuration ");
    }

    const PAA& pdn_addr = ueCtxt->getPdnAddr().paa_m;
    frReq.paa_v4_addr = pdn_addr.ip_type.ipv4.s_addr;

    // Linked EPS Bearer ID
    BearerContext* bearerCtxt = sessionCtxt->findBearerContextByBearerId(sessionCtxt->getLinkedBearerId());
    if(bearerCtxt == NULL)
    {
        log_msg(LOG_DEBUG, "send_fr_request_to_target_mme: Bearer context list empty");
        return ActStatus::HALT;
    }
    frReq.bearer_id = bearerCtxt->getBearerId();
    frReq.bearer_qos = bearerCtxt->getBearerQos();

    //Bearer Contexts
    //Support dedicated bearers
    auto& bearerCtxtCont = sessionCtxt->getBearerContextContainer();
    if (bearerCtxtCont.size() < 1)
    {
        log_msg(LOG_ERROR, "Bearer context list is empty");
    }
    else
    {
        uint8_t i =0;
        frReq.bearer_ctx_list.bearers_count = bearerCtxtCont.size();
        for (auto &bearerCtxtC : bearerCtxtCont)
        {
            if (bearerCtxtC != NULL)
            {
                frReq.bearer_ctx_list.bearer_ctxt[i].eps_bearer_id = bearerCtxtC->getBearerId();
                frReq.bearer_ctx_list.bearer_ctxt[i].bearer_qos.qci = bearerCtxt->getBearerQos().qci;
                frReq.bearer_ctx_list.bearer_ctxt[i].bearer_qos.arp.prioLevel = bearerCtxt->getBearerQos().arp.prioLevel;
                frReq.bearer_ctx_list.bearer_ctxt[i].bearer_qos.arp.preEmptionCapab = bearerCtxt->getBearerQos().arp.preEmptionCapab;
                frReq.bearer_ctx_list.bearer_ctxt[i].bearer_qos.arp.preEmptionVulnebility = bearerCtxt->getBearerQos().arp.preEmptionVulnebility;
            }
        }
    }

    // neigh_mme_ip : based on context for neighbor mme take that ip in neigh_mme_ip

    // mm context
    frReq.mm_cntxt.security_mode = EPSsecurityContext;

    E_UTRAN_sec_vector *secVect = const_cast<E_UTRAN_sec_vector*>(ueCtxt->getAiaSecInfo().AiaSecInfo_mp);
    memcpy (&(frReq.mm_cntxt.sec_vector), secVect, sizeof (E_UTRAN_security_vector));
    //secinfo& secInfo = const_cast<secinfo&>(ueCtxt->getUeSecInfo().secinfo_m);
    frReq.mm_cntxt.dl_count = ueCtxt->getUeSecInfo().getDownlinkCount();
    frReq.mm_cntxt.ul_count = ueCtxt->getUeSecInfo().getUplinkCount();
    frReq.mm_cntxt.security_encryption_algo = ueCtxt->getUeSecInfo().getSelectSecAlg();
    frReq.mm_cntxt.security_integrity_algo = ueCtxt->getUeSecInfo().getSelectIntAlg();

    memcpy(frReq.mm_cntxt.ue_network.u.octets, ueCtxt->getUeNetCapab().ue_net_capab_m.u.octets,ueCtxt->getUeNetCapab().ue_net_capab_m.len);
    memcpy(&(frReq.mm_cntxt.ue_add_sec_capab), &(ueCtxt->getUeAddSecCapab()), sizeof(ue_add_sec_capabilities));
    // subscribed and used ul/dl ambr
    // ambr
    const AMBR& ambr = ueCtxt->getAmbr().ambr_m;
    frReq.mm_cntxt.ue_aggrt_max_bit_rate.uEaggregateMaxBitRateDL = ambr.max_requested_bw_dl;
    frReq.mm_cntxt.ue_aggrt_max_bit_rate.uEaggregateMaxBitRateUL = ambr.max_requested_bw_ul;

    if (ambr.ext_max_requested_bw_dl > 0)
        frReq.mm_cntxt.ue_aggrt_max_bit_rate.ext_ue_ambr.ext_ue_ambr_DL = ambr.ext_max_requested_bw_dl;
    if (ambr.ext_max_requested_bw_ul > 0)
        frReq.mm_cntxt.ue_aggrt_max_bit_rate.ext_ue_ambr.ext_ue_ambr_UL = ambr.ext_max_requested_bw_ul;

    frReq.mm_cntxt.drx = PAGINX_DRX256;
    uint8_t kenb_key[KENB_SIZE];
    SecUtils::create_kenb_key(secVect->kasme.val, kenb_key, 0);
    unsigned char nh[SECURITY_KEY_SIZE] = { 0 };
    frReq.mm_cntxt.security_context.next_hop_chaining_count = ueCtxt->getUeSecInfo().secinfo_m.next_hop_chaining_count;
    SecUtils::create_nh_key(secVect->kasme.val, nh, kenb_key);
    memcpy(frReq.mm_cntxt.security_context.next_hop_nh , nh, KENB_SIZE);
    frReq.mm_cntxt.isNHIpresent = 1;

    /*Send message to S10-APP*/
    mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_S10_FORWARD_RELOCATION_REQUEST);
    cmn::ipc::IpcAddress destAddr = {TipcServiceInstance::s10AppInstanceNum_c};
    MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));
    mmeIpcIf.dispatchIpcMsg((char *) &frReq, sizeof(frReq), destAddr);

    log_msg(LOG_DEBUG, "Leaving send_fr_request_to_target_mme ");
    ProcedureStats::num_of_fwd_relocation_req_sent++;
#endif
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : process_fr_res
***************************************/
ActStatus ActionHandlers::process_fr_res(ControlBlock& cb)
{
#ifdef S10_FEATURE
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
#endif
    return ActStatus::PROCEED;
    
}

/***************************************
* Action handler : send_fwd_acc_ctxt_noti_to_target_mme
***************************************/
ActStatus ActionHandlers::send_fwd_acc_ctxt_noti_to_target_mme(ControlBlock& cb)
{
#ifdef S10_FEATURE
    log_msg(LOG_DEBUG, "Inside send_fwd_acc_ctxt_noti_to_target_mme");

	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ue_ctxt, "Invalid UE");

    MsgBuffer *msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
    if (msgBuf == NULL)
    {
        log_msg(LOG_ERROR, "Failed to retrieve message buffer ");
        return ActStatus::HALT;
    }

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
#endif
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : process_fwd_acc_ctxt_ack
***************************************/
ActStatus ActionHandlers::process_fwd_acc_ctxt_ack(ControlBlock& cb)
{
#ifdef S10_FEATURE
	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ue_ctxt, "Invalid UE");
#endif
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : send_fwd_rel_comp_ack_to_target_mme
***************************************/
ActStatus ActionHandlers::send_fwd_rel_comp_ack_to_target_mme(ControlBlock& cb)
{
#ifdef S10_FEATURE
	log_msg(LOG_DEBUG, "Inside send_fwd_rel_comp_ack_to_target_mme");

	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ue_ctxt, "Invalid UE");
	
	struct FWD_REL_CMP_ACK_Q_msg fwd_rel_cmp_ack_msg;
	fwd_rel_cmp_ack_msg.msg_type = forward_relocation_complete_acknowledgement;
	fwd_rel_cmp_ack_msg.ue_idx = ue_ctxt->getContextID();
	uint8_t cause = GTPV2C_CAUSE_REQUEST_ACCEPTED;
	fwd_rel_cmp_ack_msg.cause = cause;
	
	cmn::ipc::IpcAddress destAddr;
	destAddr.u32 = TipcServiceInstance::s10AppInstanceNum_c;
	
	//mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_S10_FORWARD_RELOCATION_COMPLETE_ACKNOWLEDGEMENT);
	MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId)); 
	mmeIpcIf.dispatchIpcMsg((char *) &fwd_rel_cmp_ack_msg,sizeof(fwd_rel_cmp_ack_msg), destAddr); 

	//ProcedureStats::num_of_fwd_access_context_notify_sent ++;
	log_msg(LOG_DEBUG, "Leaving send_fwd_rel_comp_ack_to_target_mme ");
#endif	
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : send_s1_rel_cmd_to_ue_for_ho
***************************************/
ActStatus ActionHandlers::send_s1_rel_cmd_to_ue_for_ho(ControlBlock& cb)
{
#ifdef S10_FEATURE
    log_msg(LOG_DEBUG, "Inside send_s1_rel_cmd_to_ue_for_ho");
    UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
    if(ue_ctxt == NULL)
    {
        log_msg(LOG_DEBUG, "send_s1_rel_cmd_to_ue_for_ho: ue context is NULL ");
        return ActStatus::HALT;
    }

    S1HandoverProcedureContext *ho_ctxt =
            dynamic_cast<S1HandoverProcedureContext*>(cb.getTempDataBlock());
    if (ho_ctxt == NULL)
    {
        log_msg(LOG_DEBUG, "send_s1_rel_cmd_to_ue_for_ho: procedure ctxt is NULL ");
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
    mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_S1AP_S1_RELEASE_COMMAND);
    cmn::ipc::IpcAddress destAddr = {TipcServiceInstance::s1apAppInstanceNum_c};
    MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));
    mmeIpcIf.dispatchIpcMsg((char *) &s1relcmd, sizeof(s1relcmd), destAddr);

    ProcedureStats::num_of_s1_rel_cmd_sent ++;
    log_msg(LOG_DEBUG,"Leaving send_s1_rel_cmd_to_ue_for_ho ");
#endif
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : send_ho_req_to_target_enb
***************************************/
ActStatus ActionHandlers::send_ho_req_to_target_enb(ControlBlock& cb)
{
#ifdef S10_FEATURE
    log_msg(LOG_INFO, "Inside send_ho_req_to_target_enb");

    UEContext *ueCtxt = static_cast<UEContext*>(cb.getPermDataBlock());

    if (ueCtxt == NULL)
    {
        log_msg(LOG_DEBUG,
                "send_ho_req_to_target_enb: ue context is NULL");
        return ActStatus::HALT;
    }

    S1HandoverProcedureContext *hoProcCtxt =
            dynamic_cast<S1HandoverProcedureContext*>(cb.getTempDataBlock());
    if (hoProcCtxt == NULL)
    {
        log_msg(LOG_DEBUG,
                "send_ho_req_to_target_enb: MmeS1HandoverProcedureCtxt is NULL");
        return ActStatus::HALT;
    }

    struct handover_request_Q_msg hoReq;
    memset(&hoReq, 0, sizeof(struct handover_request_Q_msg));

    MmeS1MsgUtils::populateHoRequest(cb, *ueCtxt, *hoProcCtxt, hoReq);

    /*Send message to S1AP-APP*/
    mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_S1AP_HANDOVER_REQUEST);
    cmn::ipc::IpcAddress destAddr = {TipcServiceInstance::s1apAppInstanceNum_c};
    MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));
    mmeIpcIf.dispatchIpcMsg((char *) &hoReq, sizeof(hoReq), destAddr);

    log_msg(LOG_DEBUG, "Leaving send_ho_request_to_target_enb ");
    ProcedureStats::num_of_ho_request_to_target_enb_sent++;
#endif
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : process_ho_req_ack
***************************************/
ActStatus ActionHandlers::process_ho_req_ack(ControlBlock& cb)
{
#ifdef S10_FEATURE
    log_msg(LOG_DEBUG, "Inside process_ho_request_ack");

    UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
    if (ue_ctxt == NULL)
    {
        log_msg(LOG_DEBUG, "process_ho_request_ack: ue ctxt is NULL ");
        return ActStatus::HALT;
    }

    S1HandoverProcedureContext *ho_ctxt =
            dynamic_cast<S1HandoverProcedureContext*>(cb.getTempDataBlock());
    if (ho_ctxt == NULL)
    {
        log_msg(LOG_DEBUG, "process_ho_request_ack: procedure ctxt is NULL ");
        return ActStatus::HALT;
    }

    MsgBuffer *msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
    if (msgBuf == NULL)
    {
        log_msg(LOG_ERROR, "Failed to retrieve message buffer ");
        return ActStatus::HALT;
    }

    const handover_req_acknowledge_Q_msg_t *ho_request_ack = static_cast<const handover_req_acknowledge_Q_msg_t*>(msgBuf->getDataPointer());

    ho_ctxt->setTargetS1apEnbUeId(ho_request_ack->header.s1ap_enb_ue_id);
    if (ho_request_ack->erab_admitted_list.count == 0)
    {
        log_msg(LOG_INFO, "HO Request ACK does not contain any eRAB Admitted Items.");

        ho_ctxt->setMmeErrorCause(S1AP_HOREQACK_FAILED);
        S1apCause s1apCause = MmeCauseUtils::convertToS1apCause(
                ho_ctxt->getMmeErrorCause());
        ho_ctxt->setS1HoCause(s1apCause);

        return ActStatus::ABORT;
    }
    ho_ctxt->setTargetToSrcTransContainer(
            ho_request_ack->targetToSrcTranspContainer);
    ho_ctxt->setErabAdmittedItem(
            ho_request_ack->erab_admitted_list.erab_admitted[0]);

    ProcedureStats::num_of_ho_request_ack_received++;
    log_msg(LOG_DEBUG, "Leaving process_ho_request_ack");
#endif
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : send_fwd_rel_resp_to_src_mme
***************************************/
ActStatus ActionHandlers::send_fwd_rel_resp_to_src_mme(ControlBlock& cb)
{
#ifdef S10_FEATURE
    log_msg(LOG_DEBUG, "Inside send_fwd_rel_resp_to_src_mme");

	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ue_ctxt, "Invalid UE");

    S1HandoverProcedureContext *prcdCtxt_p =
            dynamic_cast<S1HandoverProcedureContext*>(cb.getTempDataBlock());
    if (prcdCtxt_p == NULL)
    {
        log_msg(LOG_DEBUG,
                "send_fwd_rel_resp_to_src_mme: MmeS1HandoverProcedureCtxt is NULL");
        return ActStatus::HALT;
    }

    struct forward_relocation_resp_Q_msg forward_relocation_resp; 
    forward_relocation_resp.msg_type = forward_relocation_response;
	forward_relocation_resp.ue_idx = ue_ctxt->getContextID();

    forward_relocation_resp.f_cause.choice = prcdCtxt_p->getS1HoCause().s1apCause_m.choice;
    memcpy(&(forward_relocation_resp.eutran_container),&(prcdCtxt_p->getSrcToTargetTransContainer()),sizeof(struct src_target_transparent_container));
    
    cmn::ipc::IpcAddress destAddr;
	destAddr.u32 = TipcServiceInstance::s10AppInstanceNum_c;

    mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_S10_FORWARD_RELOCATION_RESPONSE);
	MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));   
	mmeIpcIf.dispatchIpcMsg((char *) &forward_relocation_resp, sizeof(forward_relocation_resp), destAddr);
		
	ProcedureStats::num_of_fwd_relocation_resp_sent++;
	log_msg(LOG_DEBUG, "Leaving send_fwd_rel_resp_to_src_mme ");
#endif  
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : send_ho_fwd_acc_cntx_ack_to_src_mme
***************************************/
ActStatus ActionHandlers::send_ho_fwd_acc_cntx_ack_to_src_mme(ControlBlock& cb)
{
#ifdef S10_FEATURE
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
#endif
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : send_from_target_mme_status_tranfer_to_target_enb
***************************************/
ActStatus ActionHandlers::send_from_target_mme_status_tranfer_to_target_enb(ControlBlock& cb)
{
#ifdef S10_FEATURE
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
#endif
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : process_s1_ho_notify_from_target_enb
***************************************/
ActStatus ActionHandlers::process_s1_ho_notify_from_target_enb(ControlBlock& cb)
{
#ifdef S10_FEATURE
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
#endif
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : send_ho_fwd_rel_comp_notification_to_src_mme
***************************************/
ActStatus ActionHandlers::send_ho_fwd_rel_comp_notification_to_src_mme(ControlBlock& cb)
{
#ifdef S10_FEATURE
    log_msg(LOG_DEBUG, "Inside send_ho_fwd_rel_comp_notification_to_src_mme");

	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ue_ctxt, "Invalid UE");
    
    struct FWD_REL_CMP_NOT_Q_msg fwd_rel_cmp_not_msg;
    fwd_rel_cmp_not_msg.msg_type = forward_relocation_complete_notification;
    fwd_rel_cmp_not_msg.ue_idx = ue_ctxt->getContextID();


    cmn::ipc::IpcAddress destAddr;
	destAddr.u32 = TipcServiceInstance::s10AppInstanceNum_c;

    mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_S10_FORWARD_RELOCATION_COMPLETE);
	MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));   
	mmeIpcIf.dispatchIpcMsg((char *) &fwd_rel_cmp_not_msg, sizeof(fwd_rel_cmp_not_msg), destAddr);
		
	ProcedureStats::num_of_fwd_relocation_complete_sent++;
	log_msg(LOG_DEBUG, "Leaving send_ho_fwd_rel_comp_notification_to_src_mme ");
#endif
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : send_ho_mb_req_to_sgw
***************************************/
ActStatus ActionHandlers::send_ho_mb_req_to_sgw(ControlBlock& cb)
{
#ifdef S10_FEATURE
    log_msg(LOG_DEBUG, "Inside send_ho_mb_req_to_sgw");

    UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
    if (ue_ctxt == NULL)
    {
        log_msg(LOG_DEBUG,
                "send_ho_mb_req_to_sgw: ue context or procedure ctxt is NULL ");
        return ActStatus::HALT;
    }

    S1HandoverProcedureContext *ho_ctxt =
            dynamic_cast<S1HandoverProcedureContext*>(cb.getTempDataBlock());
    if (ho_ctxt == NULL)
    {
        log_msg(LOG_DEBUG, "process_ho_notify: procedure ctxt is NULL ");
        return ActStatus::HALT;
    }
    auto& sessionCtxtContainer = ue_ctxt->getSessionContextContainer();
    if(sessionCtxtContainer.size() < 1)
    {
        log_msg(LOG_DEBUG, "send_ho_mb_req_to_sgw:Session context list empty");
        return ActStatus::HALT;
    }

    SessionContext* sessionCtxt = sessionCtxtContainer.front();

    struct MB_Q_msg mb_msg;
    memset(&mb_msg, 0, sizeof(struct MB_Q_msg));
    MmeGtpMsgUtils::populateModifyBearerRequestHo(
            cb, *ue_ctxt, *sessionCtxt, *ho_ctxt, mb_msg);

    mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_S11_MODIFY_BEARER_REQUEST);
    cmn::ipc::IpcAddress destAddr = {TipcServiceInstance::s11AppInstanceNum_c};
    MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));
    mmeIpcIf.dispatchIpcMsg((char *) &mb_msg, sizeof(mb_msg), destAddr);

    ProcedureStats::num_of_mb_req_to_sgw_sent++;
    log_msg(LOG_DEBUG, "Leaving send_ho_mb_req_to_sgw");
#endif
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : process_mb_resp_from_sgw
***************************************/
ActStatus ActionHandlers::process_mb_resp_from_sgw(ControlBlock& cb)
{
#ifdef S10_FEATURE
    log_msg(LOG_DEBUG, "Inside process_mb_resp_from_sgw ");

    ProcedureStats::num_of_processed_mb_resp ++;
#endif
    return ActStatus::PROCEED;
}
