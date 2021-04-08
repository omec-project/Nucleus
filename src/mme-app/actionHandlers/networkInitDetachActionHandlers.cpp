/*
 * Copyright 2019-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <typeinfo>
#include "actionHandlers/actionHandlers.h"
#include "mme_app.h"
#include "controlBlock.h"
#include "msgType.h"
#include "contextManager/subsDataGroupManager.h"
#include "contextManager/dataBlocks.h"
#include "procedureStats.h"
#include "log.h"
#include "secUtils.h"
#include "state.h"
#include <string.h>
#include <sstream>
#include <mmeSmDefs.h>

#include <ipcTypes.h>
#include <tipcTypes.h>
#include <msgBuffer.h>
#include <interfaces/mmeIpcInterface.h>
#include <utils/mmeContextManagerUtils.h>
#include "mmeNasUtils.h"
#include "mmeStatsPromClient.h"

using namespace SM;
using namespace mme;
using namespace cmn;
using namespace cmn::utils;

ActStatus ActionHandlers::ni_detach_req_to_ue(SM::ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside ni_detach_req_to_ue ");
	
	UEContext *ue_ctxt =  dynamic_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ue_ctxt, "Invalid UE");

	MmeDetachProcedureCtxt *procCtxt =  dynamic_cast<MmeDetachProcedureCtxt*>(cb.getTempDataBlock());
	VERIFY(procCtxt, return ActStatus::ABORT, "Procedure Context is NULL ");

	ni_detach_request_Q_msg ni_detach_req;
	
	ni_detach_req.msg_type = ni_detach_request;
	ni_detach_req.enb_fd = ue_ctxt->getEnbFd();
	ni_detach_req.ue_idx = ue_ctxt->getContextID();
	ni_detach_req.enb_s1ap_ue_id =  ue_ctxt->getS1apEnbUeId();
	struct Buffer nasBuffer;
	struct nasPDU nas = {0};
	nas.header.security_header_type = IntegrityProtectedCiphered;
	nas.header.proto_discriminator = EPSMobilityManagementMessages;
	/* placeholder for mac. mac value will be calculated later */
	uint8_t mac[MAC_SIZE] = {0};
	memcpy(nas.header.mac, mac, MAC_SIZE);

	nas.header.seq_no = ue_ctxt->getUeSecInfo().getDownlinkSeqNo(); 
	nas.dl_count = ue_ctxt->getUeSecInfo().getDownlinkCount();	
	ue_ctxt->getUeSecInfo().increment_downlink_count();

	nas.header.message_type = DetachRequest;
 	if(procCtxt->getNasDetachType() > 0)
                nas.header.detach_type = procCtxt->getNasDetachType();
        else
                nas.header.detach_type = reattachRequired;
	if(procCtxt->getDetachCause() > 0)
                nas.header.emm_cause = procCtxt->getDetachCause();	
	else
		nas.header.emm_cause = 0;
	MmeNasUtils::encode_nas_msg(&nasBuffer, &nas, ue_ctxt->getUeSecInfo());
	memcpy(&ni_detach_req.nasMsgBuf[0], &nasBuffer.buf[0], nasBuffer.pos);
	ni_detach_req.nasMsgSize = nasBuffer.pos;
	
	/* Send message to S1app in S1q*/
	cmn::ipc::IpcAddress destAddr;
	destAddr.u32 = TipcServiceInstance::s1apAppInstanceNum_c;

    mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_NAS_NETWORK_INITIATED_DETACH);
	MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));
	mmeIpcIf.dispatchIpcMsg((char *) &ni_detach_req, sizeof(ni_detach_req), destAddr);
;
	ProcedureStats::num_of_detach_req_to_ue_sent ++;

	return ActStatus::PROCEED;
}

ActStatus ActionHandlers::process_detach_accept_from_ue(SM::ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside process_detach_accept_from_ue ");
		
	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ue_ctxt, "Invalid UE");
		
	//ue_ctxt->getUeSecInfo().increment_uplink_count();

	ProcedureStats::num_of_detach_accept_from_ue ++;

	return ActStatus::PROCEED;
}

/***************************************
* Action handler : send_s1_rel_cmd_to_ue_for_detach
***************************************/
ActStatus ActionHandlers::send_s1_rel_cmd_to_ue_for_detach(ControlBlock& cb)
{
    log_msg(LOG_DEBUG, "Inside send_s1_rel_cmd_to_ue_for_detach");

    UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
    VERIFY_UE(cb, ue_ctxt, "Invalid UE");

    struct s1relcmd_info s1relcmd;

    s1relcmd.msg_type = s1_release_command;
    s1relcmd.ue_idx = ue_ctxt->getContextID();
    s1relcmd.enb_fd = ue_ctxt->getEnbFd();
    s1relcmd.enb_s1ap_ue_id = ue_ctxt->getS1apEnbUeId();
    s1relcmd.cause.present = s1apCause_PR_nas;
    s1relcmd.cause.choice.nas = s1apCauseNas_detach;

    /*Send message to S1AP-APP*/
    cmn::ipc::IpcAddress destAddr;
    destAddr.u32 = TipcServiceInstance::s1apAppInstanceNum_c;
    
    mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_S1AP_S1_RELEASE_COMMAND);
    MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));
    mmeIpcIf.dispatchIpcMsg((char *) &s1relcmd, sizeof(s1relcmd), destAddr);

    ProcedureStats::num_of_s1_rel_cmd_sent ++;
    return ActStatus::PROCEED;
}

/************************************************************
* Action handler : process_ue_ctxt_rel_comp_for_detach
**************************************************************/
ActStatus ActionHandlers::process_ue_ctxt_rel_comp_for_detach(ControlBlock& cb)
{
    log_msg(LOG_DEBUG, "Inside process_ue_ctxt_rel_comp_for_detach ");

    UEContext *ueCtxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
    VERIFY_UE(cb, ueCtxt, "Invalid UE");
    MmeDetachProcedureCtxt *procCtxt = dynamic_cast<MmeDetachProcedureCtxt*>(cb.getTempDataBlock());
    VERIFY(procCtxt, return ActStatus::ABORT, "Procedure Context is NULL ");

    MmContext* mmCtxt = ueCtxt->getMmContext();
    VERIFY_UE(cb, mmCtxt, "Invalid UE");

    if(procCtxt->getCancellationType() == SUBSCRIPTION_WITHDRAWAL)
    {
        MmeContextManagerUtils::deleteUEContext(cb.getCBIndex());
    }
    else
    {
    	mmCtxt->setMmState( EpsDetached );
    	mmCtxt->setEcmState( ecmIdle_c );
    	ueCtxt->setS1apEnbUeId(0);
    	if(procCtxt->getDetachType() == pgwInitDetach_c)
    	{
            SM::Event evt(DETACH_COMPLETE, NULL);
            cb.qInternalEvent(evt);
    	}

    	mmeStats::Instance()->increment(mmeStatsCounter::MME_PROCEDURES_DETACH_PROC_SUCCESS);
    	MmeContextManagerUtils::deallocateProcedureCtxt(cb, procCtxt);
    }

    ProcedureStats::num_of_s1_rel_comp_received ++;
    ProcedureStats::num_of_subscribers_detached ++;
    if (ProcedureStats::num_of_subscribers_attached > 0)
    	ProcedureStats::num_of_subscribers_attached --;

    return ActStatus::PROCEED;

}

