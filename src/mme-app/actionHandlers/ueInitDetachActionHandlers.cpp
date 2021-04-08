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
#include "mmeStatsPromClient.h"

using namespace SM;
using namespace mme;
using namespace cmn;
using namespace cmn::utils;

ActStatus ActionHandlers::del_session_req(SM::ControlBlock& cb)
{

    UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
    VERIFY_UE(cb, ue_ctxt, "Invalid UE");
    log_msg(LOG_DEBUG, "Inside delete_session_req %u ", ue_ctxt->getContextID());

    bool status = false;

    auto &sessionCtxtContainer = ue_ctxt->getSessionContextContainer();
    if(sessionCtxtContainer.size() > 0)
    {
        SessionContext *sessionCtxt = sessionCtxtContainer.front();
        if(sessionCtxt)
        {
            struct DS_Q_msg g_ds_msg;
            g_ds_msg.msg_type = delete_session_request;
            g_ds_msg.ue_idx = ue_ctxt->getContextID();

            memset(g_ds_msg.indication, 0, S11_DS_INDICATION_FLAG_SIZE);
            g_ds_msg.indication[0] = 8; /* TODO : define macro or enum */

            g_ds_msg.bearer_id = sessionCtxt->getLinkedBearerId();

            memcpy(&(g_ds_msg.s11_sgw_c_fteid),
                   &(sessionCtxt->getS11SgwCtrlFteid().fteid_m),
                   sizeof(struct fteid));

            /* Send message to S11app in S11q*/
            cmn::ipc::IpcAddress destAddr;
            destAddr.u32 = TipcServiceInstance::s11AppInstanceNum_c;

            mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_S11_DELETE_SESSION_REQUEST);

            MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));
            mmeIpcIf.dispatchIpcMsg((char*) &g_ds_msg, sizeof(g_ds_msg), destAddr);

            ProcedureStats::num_of_del_session_req_sent++;

            status = true; // delete session sent success
        }
        else
        {
            log_msg(LOG_DEBUG, "session context does not exist ");
        }
    }

    ActStatus rc = ActStatus::PROCEED;

    // This action may be invoked during an abort of procedure other than detach as well.
    // In such cases, even if we are not able to successfully send delete, return proceed
    // so that abort can continue.
    if(!status)
    {
        log_msg(LOG_DEBUG, "failure in handling dsreq event ");
        MmeProcedureCtxt *procCtxt = dynamic_cast<MmeProcedureCtxt*>(cb.getTempDataBlock());

        if(procCtxt != NULL)
        {
            switch (procCtxt->getCtxtType())
            {
                case detach_c:
                {
                    // Action invoked as part of detach success path, return abort as
                    // we failed to send out delete session request to gw
                    log_msg(LOG_DEBUG, "failure in handling dsreq event for detach_c procedure ");
                    if(procCtxt->getMmeErrorCause() == SUCCESS)
                    {
                        log_msg(LOG_DEBUG, "failure in handling dsreq event. return abort ");
                        rc = ActStatus::ABORT;
                    }

                }break;
                default:
                {
                    // Do Nothing
                }
            }

        }
    }

    return rc;
}

ActStatus ActionHandlers::purge_req(SM::ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside purge_req ");
	UEContext *ue_ctxt =  static_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ue_ctxt, "Invalid UE");
	
	s6a_Q_msg purge_msg;
	purge_msg.msg_type = purge_request;	
	purge_msg.ue_idx = ue_ctxt->getContextID();
	memset(purge_msg.imsi, '\0', sizeof(purge_msg.imsi));
	ue_ctxt->getImsi().getImsiDigits(purge_msg.imsi);
		
	/* Send message to S6app in S6q*/
	cmn::ipc::IpcAddress destAddr;
	destAddr.u32 = TipcServiceInstance::s6AppInstanceNum_c;

    mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_S6A_PURGE_REQUEST);
    MmeIpcInterface &mmeIpcIf =static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));	
	mmeIpcIf.dispatchIpcMsg((char *) &purge_msg, sizeof(purge_msg), destAddr);
	
	ProcedureStats::num_of_purge_req_sent ++;
	return ActStatus::PROCEED;
	
}


ActStatus ActionHandlers::process_del_session_resp(SM::ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside handle_delete_session_resp ");
	
	UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ue_ctxt, "Invalid UE");

	auto& sessionCtxtContainer = ue_ctxt->getSessionContextContainer();
	if(sessionCtxtContainer.size() > 0)
	{
	    SessionContext* sessionCtxt = sessionCtxtContainer.front();
	    VERIFY(sessionCtxt, return ActStatus::ABORT, "Session Context is NULL ");
	    MmeContextManagerUtils::deallocateSessionContext(cb, sessionCtxt, ue_ctxt);
	}
	ProcedureStats::num_of_processed_del_session_resp ++;
	return ActStatus::PROCEED;
}


ActStatus ActionHandlers::process_pur_resp(SM::ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside handle_purge_resp ");
	
	UEContext *ue_ctxt =  static_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ue_ctxt, "Invalid UE");
	//purge_resp_Q_msg_t *purge_msg = nullptr;
	
	/*Nothing is been done. Only takes the UE Index
	 * increment the stats counter and changes the state*/
	
	ProcedureStats::num_of_processed_pur_resp ++;
	return ActStatus::PROCEED;
	
}

ActStatus ActionHandlers::detach_accept_to_ue(SM::ControlBlock& cb)
{
	
	UEContext *ue_ctxt =  static_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ue_ctxt, "Invalid UE");
  
	log_msg(LOG_DEBUG, "Inside send_detach_accept %u ",ue_ctxt->getContextID());
  
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

    mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_S1AP_DETACH_ACCEPT);
	MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));
	mmeIpcIf.dispatchIpcMsg((char *) &detach_accpt, sizeof(detach_accpt), destAddr);
	
	mmeStats::Instance()->increment(mmeStatsCounter::MME_PROCEDURES_DETACH_PROC_SUCCESS);

	MmeProcedureCtxt* procedure_p = static_cast<MmeProcedureCtxt*>(cb.getTempDataBlock());
	MmeContextManagerUtils::deallocateProcedureCtxt(cb, procedure_p );

	MmContext* mmCtxt = ue_ctxt->getMmContext();
	VERIFY_UE(cb, mmCtxt, "Invalid UE");

	mmCtxt->setMmState(EpsDetached);
	mmCtxt->setEcmState(ecmIdle_c);
	mmeStats::Instance()->decrement(mmeStatsCounter::MME_NUM_ACTIVE_SUBSCRIBERS);

	ue_ctxt->setS1apEnbUeId(0);
	ProcedureStats::num_of_detach_accept_to_ue_sent ++;
	ProcedureStats::num_of_subscribers_detached ++;
	if (ProcedureStats::num_of_subscribers_attached > 0)
		ProcedureStats::num_of_subscribers_attached --;

	return ActStatus::PROCEED;
	
}
/***************************************
* Action handler : abort_detach
***************************************/
ActStatus ActionHandlers::abort_detach(ControlBlock& cb)
{
    log_msg(LOG_DEBUG, "abort_detach : Entry ");
    MmeDetachProcedureCtxt *procedure_p =
            static_cast<MmeDetachProcedureCtxt*>(cb.getTempDataBlock());
    if(procedure_p != NULL)
    {
        MmeContextManagerUtils::deallocateProcedureCtxt(cb, procedure_p);
    }

    // Fire a Detach failure event, so that interested procedures
    // can take appropriate actions.
    SM::Event evt(DETACH_FAILURE, NULL);
    cb.qInternalEvent(evt);
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : handle_s1_rel_req_during_detach
***************************************/
ActStatus ActionHandlers::handle_s1_rel_req_during_detach(ControlBlock& cb)
{
    log_msg(LOG_DEBUG, "Recevied S1 Release Request during detach for cb %d", cb.getCBIndex());

    MmeDetachProcedureCtxt *procedure_p =
            static_cast<MmeDetachProcedureCtxt*>(cb.getTempDataBlock());
    if(procedure_p != NULL)
    { 
       procedure_p->setMmeErrorCause(DETACH_FAILED);
    }

    return ActStatus::ABORT;
}

