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
    // S10_FEATURE
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
    frReq.ue_idx = frReq->getContextID();

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
        log_msg(LOG_DEBUG, "Found APN mapping in static table %x ",cs_msg.sgw_ip);
        frReq.sgw_ip = temp->get_sgw_addr();
        frReq.pgw_ip = temp->get_pgw_addr();
    }
    else
    {
        log_msg(LOG_DEBUG, "APN not found in static apn configuration ");
    }

    // Linked EPS Bearer ID
    BearerContext* bearerCtxt = sessionCtxt->findBearerContextByBearerId(sessionCtxt->getLinkedBearerId());
    if(bearerCtxt == NULL)
    {
        log_msg(LOG_DEBUG, "send_fr_request_to_target_mme: Bearer context list empty");
        return ActStatus::HALT;
    }
    frReq.bearer_id = bearerCtxt->getBearerId();

    //Bearer Contexts
    //Support dedicated bearers
    auto& bearerCtxtCont = sessionCtxt->getBearerContextContainer();
    if (bearerCtxtCont != NULL)
    {
        uint8_t i =0;
        frReq.bearer_ctx_list.bearers_count = bearerCtxtCont.size();
        for (auto &bearerCtxtC : bearerCtxtCont)
        {
            if (bearerCtxtC != NULL)
            {
                frReq.bearer_ctx_list.bearer_ctxt[i].eps_bearer_id = bearerCtxtC->getBearerId();
                frReq.bearer_ctx_list.bearer_ctxt[i].bearer_qos.qci = bearerCtxt->getBearerQos().qci;
                frReq.bearer_ctx_list.bearer_ctxt[i].bearer_qos.ARP.pl = bearerCtxt->getBearerQos().arp.prioLevel;
                frReq.bearer_ctx_list.bearer_ctxt[i].bearer_qos.ARP.pci = bearerCtxt->getBearerQos().arp.preEmptionCapab;
                frReq.bearer_ctx_list.bearer_ctxt[i].bearer_qos.ARP.pvi = bearerCtxt->getBearerQos().arp.preEmptionVulnebility;
            }
        }
    }

    // ambr
    frReq.exg_max_dl_bitrate = (ueCtxt->getAmbr().ambr_m).max_requested_bw_dl;
    frReq.exg_max_ul_bitrate = (ueCtxt->getAmbr().ambr_m).max_requested_bw_ul;

    // neigh_mme_ip : based on context for neighbor mme take that ip in neigh_mme_ip

    // mm context



    /*Send message to S10-APP*/
    mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_S10_FORWARD_RELOCATION_REQUEST);
    cmn::ipc::IpcAddress destAddr = {TipcServiceInstance::s10AppInstanceNum_c};
    MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));
    mmeIpcIf.dispatchIpcMsg((char *) &hoReq, sizeof(frReq), destAddr);

    log_msg(LOG_DEBUG, "Leaving send_fr_request_to_target_mme ");

    ProcedureStats::num_of_fr_request_to_target_mme_sent++;

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
    return ActStatus::PROCEED;
}

