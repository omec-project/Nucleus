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
#include "secUtils.h"
using namespace mme;

extern mme_config_t *mme_cfg;

void MmeS1MsgUtils::populateHoRequest(SM::ControlBlock& cb,
        UEContext& ueCtxt,
		S1HandoverProcedureContext& procCtxt,
		struct handover_request_Q_msg& hoReq)
{
    SessionContext *sessionCtxt = ueCtxt.getSessionContext();
    if (sessionCtxt == NULL)
    {
        log_msg(LOG_DEBUG,
                " send_ho_request_to_target_enb: session ctxt is NULL \n");
        return;
    }
    BearerContext *bearerCtxt = sessionCtxt->getBearerContext();
    if (bearerCtxt == NULL)
    {
        log_msg(LOG_ERROR, "Failed to retrieve Bearer context for UE IDx %d\n",
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

    hoReq.eRABSetupList.count = 1;
    hoReq.eRABSetupList.eRABSetup[0].e_RAB_ID = 5;
    hoReq.eRABSetupList.eRABSetup[0].gtp_teid =
            bearerCtxt->getS1uSgwUserFteid().fteid_m.header.teid_gre;
    hoReq.eRABSetupList.eRABSetup[0].transportLayerAddress =
            bearerCtxt->getS1uSgwUserFteid().fteid_m.ip.ipv4.s_addr;

    hoReq.gummei.mme_code = mme_cfg->mme_code;
    hoReq.gummei.mme_grp_id = mme_cfg->mme_group_id;
    memcpy(hoReq.gummei.plmn_id.idx, mme_cfg->plmns[1].idx, 3);

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
