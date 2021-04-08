/*
* Copyright 2020-present, Infosys Ltd.
*
* SPDX-License-Identifier: Apache-2.0
*/

#include "utils/mmeS1MsgUtils.h"

#include <controlBlock.h>
#include <contextManager/dataBlocks.h>
#include <contextManager/subsDataGroupManager.h>
#include <log.h>
#include <mme_app.h>
#include <msgBuffer.h>
#include <s1ap_structs.h>
#include <utils/defaultMmeProcedureCtxt.h>
#include <utils/mmeContextManagerUtils.h>
#include "mmeNasUtils.h"
#include "mmeStatsPromClient.h"
#include "secUtils.h"
#include "gtpCauseTypes.h"
#include <utils/mmeCommonUtils.h>

using namespace mme;

extern mme_config_t *mme_cfg;

void MmeS1MsgUtils::populateHoRequest(SM::ControlBlock& cb,
        UEContext& ueCtxt,
		S1HandoverProcedureContext& procCtxt,
		struct handover_request_Q_msg& hoReq)
{
    auto& sessionCtxtContainer = ueCtxt.getSessionContextContainer();
    if(sessionCtxtContainer.size() < 1)
    {
        log_msg(LOG_DEBUG,
		" send_ho_request_to_target_enb:Session context list is empty");
	return;
    }

    SessionContext* sessionCtxt = sessionCtxtContainer.front();
    BearerContext *bearerCtxt = sessionCtxt->findBearerContextByBearerId(sessionCtxt->getLinkedBearerId());
    if (bearerCtxt == NULL)
    {
        log_msg(LOG_ERROR, "Failed to retrieve Bearer context for UE IDx %d",
                cb.getCBIndex());
        return;
    }
    hoReq.msg_type = handover_request;
    hoReq.s1ap_mme_ue_id = ueCtxt.getContextID();
    hoReq.target_enb_context_id = procCtxt.getTargetEnbContextId();

    hoReq.handoverType = IntraLTE;
    hoReq.cause.present = s1apCause_PR_radioNetwork;
    hoReq.cause.choice.radioNetwork =
            procCtxt.getS1HoCause().s1apCause_m.choice.radioNetwork;

    memcpy(&(hoReq.src_to_target_transparent_container),
            &(procCtxt.getSrcToTargetTransContainer()),
            sizeof(struct src_target_transparent_container));

    hoReq.ue_aggrt_max_bit_rate.uEaggregateMaxBitRateDL =
            (ueCtxt.getAmbr().ambr_m).max_requested_bw_dl;
    hoReq.ue_aggrt_max_bit_rate.uEaggregateMaxBitRateUL =
            (ueCtxt.getAmbr().ambr_m).max_requested_bw_ul;

    hoReq.erab_su_list.count = 1;
    hoReq.erab_su_list.erab_su_item[0].e_RAB_ID = 5;
    hoReq.erab_su_list.erab_su_item[0].gtp_teid =
            bearerCtxt->getS1uSgwUserFteid().fteid_m.header.teid_gre;
    hoReq.erab_su_list.erab_su_item[0].transportLayerAddress =
            bearerCtxt->getS1uSgwUserFteid().fteid_m.ip.ipv4.s_addr;

    hoReq.gummei.mme_code = mme_cfg->mme_code;
    hoReq.gummei.mme_grp_id = mme_cfg->mme_group_id;
    const Tai &temp = ueCtxt.getTai();
    TAI tai = temp.tai_m;
    MmeCommonUtils::getS1apPlmnIdFroms11(&tai.plmn_id);
    memcpy(hoReq.gummei.plmn_id.idx, tai.plmn_id.idx, 3);
    secinfo& secInfo = const_cast<secinfo&>(ueCtxt.getUeSecInfo().secinfo_m);
    secInfo.next_hop_chaining_count = secInfo.next_hop_chaining_count + 1 ;
    hoReq.security_context.next_hop_chaining_count = secInfo.next_hop_chaining_count;

    unsigned char currentNhKey[SECURITY_KEY_SIZE] = {0};
    memcpy(currentNhKey, secInfo.next_hop_nh, SECURITY_KEY_SIZE);
    const E_UTRAN_sec_vector *secVect = ueCtxt.getAiaSecInfo().AiaSecInfo_mp;

    unsigned char nh[SECURITY_KEY_SIZE] =
    { 0 };
    SecUtils::create_nh_key(secVect->kasme.val, nh, currentNhKey);
    memcpy(hoReq.security_context.next_hop_nh, nh, SECURITY_KEY_SIZE);
    memcpy(secInfo.next_hop_nh, nh, SECURITY_KEY_SIZE);
}

void MmeS1MsgUtils::populateHoCommand(SM::ControlBlock& cb,
        UEContext& ueCtxt,
		S1HandoverProcedureContext& procCtxt,
		struct handover_command_Q_msg& hoCommand)
{
	hoCommand.msg_type = handover_command;

	hoCommand.s1ap_mme_ue_id = ueCtxt.getContextID();
	hoCommand.s1ap_enb_ue_id = ueCtxt.getS1apEnbUeId();
	hoCommand.src_enb_context_id = procCtxt.getSrcEnbContextId();
	hoCommand.handoverType = IntraLTE;
	hoCommand.target_to_src_transparent_container =
	        procCtxt.getTargetToSrcTransContainer();

    if(0) {
	    hoCommand.erabs_Subject_to_Forwarding_List.count = 1;

	    ERABs_Subject_to_Forwarding *erabsSubjectToFwding =
	            &(hoCommand.erabs_Subject_to_Forwarding_List.eRABs_Subject_to_Forwarding[0]);

	    erabsSubjectToFwding->e_RAB_ID =
	            procCtxt.getErabAdmittedItem().e_RAB_ID;
	    erabsSubjectToFwding->dL_gtp_teid =
	            procCtxt.getErabAdmittedItem().dL_gtp_teid;
	    erabsSubjectToFwding->dL_transportLayerAddress =
	            procCtxt.getErabAdmittedItem().dL_transportLayerAddress;
    }

}

bool MmeS1MsgUtils::populateErabSetupAndActDedBrReq(SM::ControlBlock &cb,
        UEContext &ueCtxt, MmeSmCreateBearerProcCtxt &procCtxt,
        struct erabsu_ctx_req_Q_msg &erab_su_req)
{
    bool status = true;
    erab_su_req.msg_type = erab_setup_request;

    erab_su_req.mme_ue_s1ap_id = ueCtxt.getContextID();
    erab_su_req.enb_s1ap_ue_id = ueCtxt.getS1apEnbUeId();
    erab_su_req.ue_aggrt_max_bit_rate.uEaggregateMaxBitRateDL =
            (ueCtxt.getAmbr().ambr_m).max_requested_bw_dl;
    erab_su_req.ue_aggrt_max_bit_rate.uEaggregateMaxBitRateUL =
            (ueCtxt.getAmbr().ambr_m).max_requested_bw_ul;
    erab_su_req.enb_context_id = ueCtxt.getEnbFd();

    SessionContext *sess_p = ueCtxt.findSessionContextByLinkedBearerId(procCtxt.getBearerId());
    VERIFY(sess_p, procCtxt.setMmeErrorCause(SESSION_CONTEXT_NOT_FOUND);
        return false, "Session Context is NULL");

    auto& cbBearerStatusCont = procCtxt.getBearerStatusContainer();

    uint8_t i = 0;

    for (auto &bearerStatus: cbBearerStatusCont)
    {
        if (bearerStatus.bearer_ctxt_cb_resp_m.cause.cause == GTPV2C_NO_CAUSE)
        {
            BearerContext *bearerCtxt_p =
                    MmeContextManagerUtils::findBearerContext(
                            bearerStatus.bearer_ctxt_cb_resp_m.eps_bearer_id,
                            &ueCtxt);
            if (bearerCtxt_p != NULL)
            {
                erab_su_req.erab_su_list.erab_su_item[i].e_RAB_ID =
                        bearerCtxt_p->getBearerId();
                erab_su_req.erab_su_list.erab_su_item[i].e_RAB_QoS_Params.qci =
                        bearerCtxt_p->getBearerQos().qci;
                erab_su_req.erab_su_list.erab_su_item[i].e_RAB_QoS_Params.arPrio.prioLevel =
                        bearerCtxt_p->getBearerQos().arp.prioLevel;
                erab_su_req.erab_su_list.erab_su_item[i].e_RAB_QoS_Params.arPrio.preEmptionCapab =
                        bearerCtxt_p->getBearerQos().arp.preEmptionCapab;
                erab_su_req.erab_su_list.erab_su_item[i].e_RAB_QoS_Params.arPrio.preEmptionVulnebility =
                        bearerCtxt_p->getBearerQos().arp.preEmptionVulnebility;
                erab_su_req.erab_su_list.erab_su_item[i].transportLayerAddress =
                        bearerCtxt_p->getS1uSgwUserFteid().fteid_m.ip.ipv4.s_addr;
                erab_su_req.erab_su_list.erab_su_item[i].gtp_teid =
                        bearerCtxt_p->getS1uSgwUserFteid().fteid_m.header.teid_gre;

                erab_su_req.nas_buf[i] = { 0 };
                struct nasPDU nas = erab_su_req.erab_su_list.erab_su_item[i].nas;

                MmeNasUtils::encode_act_ded_br_req_nas_pdu(sess_p,
                        bearerCtxt_p, ueCtxt.getUeSecInfo(), &nas);

                MmeNasUtils::encode_nas_msg(&erab_su_req.nas_buf[i], &nas,
                        ueCtxt.getUeSecInfo());

                mmeStats::Instance()->increment(
                        mmeStatsCounter::MME_MSG_TX_NAS_ACT_DED_BR_CTXT_REQUEST);

                i++;
            }
            else
            {
                bearerStatus.bearer_ctxt_cb_resp_m.cause.cause = GTPV2C_CAUSE_REQUEST_REJECTED;
            }
        }
    }

    if (i > 0)
    {
        erab_su_req.erab_su_list.count = i;
    }
    else
    {
        status = false;
    }

    return status;
}

bool MmeS1MsgUtils::populateErabRelAndDeActDedBrReq(SM::ControlBlock &cb,
        UEContext &ueCtxt, BearerContext &bearerCtxt,
        struct erab_release_command_Q_msg &erab_rel_cmd)
{
    bool status = true;
    erab_rel_cmd.msg_type = erab_release_command;

    erab_rel_cmd.mme_ue_s1ap_id = ueCtxt.getContextID();
    erab_rel_cmd.enb_s1ap_ue_id = ueCtxt.getS1apEnbUeId();
    erab_rel_cmd.ue_aggrt_max_bit_rate.uEaggregateMaxBitRateDL =
            (ueCtxt.getAmbr().ambr_m).max_requested_bw_dl;
    erab_rel_cmd.ue_aggrt_max_bit_rate.uEaggregateMaxBitRateUL =
            (ueCtxt.getAmbr().ambr_m).max_requested_bw_ul;
    erab_rel_cmd.enb_context_id = ueCtxt.getEnbFd();

    erab_rel_cmd.erab_to_be_released_list.erab_item[0].e_RAB_ID = bearerCtxt.getBearerId();
    erab_rel_cmd.erab_to_be_released_list.erab_item[0].cause.present = s1apCause_PR_nas;
    erab_rel_cmd.erab_to_be_released_list.erab_item[0].cause.choice.nas = s1apCauseNas_normal_release;
    struct Buffer nasBuffer;
    struct nasPDU nas = {0};
    MmeNasUtils::encode_deact_ded_br_req_nas_pdu(bearerCtxt.getBearerId(),
		    ueCtxt.getUeSecInfo(), &nas);

    MmeNasUtils::encode_nas_msg(&nasBuffer, &nas,
		    ueCtxt.getUeSecInfo());

    memcpy(&erab_rel_cmd.nasMsgBuf[0], &nasBuffer.buf[0], nasBuffer.pos);
    erab_rel_cmd.nasMsgSize = nasBuffer.pos;
    free(nas.elements);

    if (erab_rel_cmd.nasMsgSize > 0)
    {
        erab_rel_cmd.erab_to_be_released_list.count = 1;
    }
    else
    {
        status = false;
    }

    return status;
}
