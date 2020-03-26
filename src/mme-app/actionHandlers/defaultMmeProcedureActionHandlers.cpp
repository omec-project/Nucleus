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

#include <actionHandlers/actionHandlers.h>
#include <contextManager/dataBlocks.h>
#include <contextManager/subsDataGroupManager.h>
#include <controlBlock.h>
#include <event.h>
#include <mmeSmDefs.h>
#include <mmeStates/attachStart.h>
#include <mmeStates/detachStart.h>
#include <mmeStates/niDetachStart.h>
#include <mmeStates/pagingStart.h>
#include <mmeStates/s1ReleaseStart.h>
#include <mmeStates/serviceRequestStart.h>
#include <mmeStates/tauStart.h>
#include <msgBuffer.h>
#include <msgType.h>
#include <log.h>
#include <procedureStats.h>
#include <s1ap_structs.h>
#include <state.h>
#include <string.h>
#include <sstream>
#include <typeinfo>
#include <utils/mmeProcedureTypes.h>
#include <utils/mmeCommonUtils.h>
#include <utils/mmeContextManagerUtils.h>

using namespace mme;
using namespace SM;

/***************************************
* Action handler : default_attach_req_handler
***************************************/
ActStatus ActionHandlers::default_attach_req_handler(ControlBlock& cb)
{
	log_msg(LOG_ERROR, "default_attach_req_handler \n");

	UEContext* ueCtxt_p = NULL;
	MmContext* mmCtxt = NULL;

	ueCtxt_p = static_cast <UEContext *>(cb.getPermDataBlock());
	if (ueCtxt_p != NULL)
		mmCtxt = ueCtxt_p->getMmContext();

	MsgBuffer* msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
	if (msgBuf == NULL)
	{
		log_msg(LOG_ERROR, "Failed to retrieve message buffer \n");
		return ActStatus::HALT;
	}

	const s1_incoming_msg_data_t* msgData_p =
			static_cast<const s1_incoming_msg_data_t*>(msgBuf->getDataPointer());
	if (msgData_p == NULL)
	{
		log_msg(LOG_ERROR, "Failed to retrieve data buffer \n");
		return ActStatus::HALT;
	}

	const struct ue_attach_info &ue_info = (msgData_p->msg_data.ue_attach_info_m);

	AttachType attachType = MmeCommonUtils::getAttachType(ueCtxt_p, ue_info);
	if (attachType == maxAttachType_c)
	{
		log_msg(LOG_ERROR, "Failed to identify attach type \n");
		return ActStatus::HALT;
	}

	if (ueCtxt_p == NULL)
	{
		ueCtxt_p = SubsDataGroupManager::Instance()->getUEContext();
		if (ueCtxt_p == NULL)
		{
			log_msg(LOG_ERROR, "Failed to allocate UE context \n");

			return ActStatus::HALT;
		}

		mmCtxt = SubsDataGroupManager::Instance()->getMmContext();
		if( mmCtxt == NULL )
		{
			log_msg(LOG_ERROR, "Failed to allocate MM Context \n");

			SubsDataGroupManager::Instance()->deleteUEContext( ueCtxt_p );
			return ActStatus::HALT;
		}

		ueCtxt_p->setContextID(cb.getCBIndex());
		ueCtxt_p->setMmContext( mmCtxt );

		mmCtxt->setEcmState(ecmConnected_c);

		cb.setPermDataBlock(ueCtxt_p);
		cb.setFastAccessBlock(ueCtxt_p, 1);
	}

	MmeProcedureCtxt* prcdCtxt_p = SubsDataGroupManager::Instance()->getMmeProcedureCtxt();
	if( prcdCtxt_p == NULL )
	{
		log_msg(LOG_ERROR, "Failed to allocate Procedure Context \n");

		return ActStatus::HALT;
	}

	prcdCtxt_p->setCtxtType( ProcedureType::attach_c );
	prcdCtxt_p->setNextState(AttachStart::Instance());

	cb.setCurrentTempDataBlock(prcdCtxt_p);
	
	MmeCommonUtils::formatS1apPlmnId(const_cast<PLMN*>(&ue_info.tai.plmn_id));
	MmeCommonUtils::formatS1apPlmnId(const_cast<PLMN*>(&ue_info.utran_cgi.plmn_id));

	// Copy attach request message data into UE Context

	ueCtxt_p->setS1apEnbUeId(ue_info.s1ap_enb_ue_id);
	ueCtxt_p->setEnbFd(ue_info.enb_fd);
	ueCtxt_p->setTai(Tai(ue_info.tai));
	ueCtxt_p->setUtranCgi(Cgi(ue_info.utran_cgi));
	ueCtxt_p->setUeNetCapab(Ue_net_capab(ue_info.ue_net_capab));
	ueCtxt_p->setMsNetCapab(Ms_net_capab(ue_info.ms_net_capab));
	ueCtxt_p->setDwnLnkSeqNo(ue_info.seq_no);
	prcdCtxt_p->setPti(ue_info.pti);
	prcdCtxt_p->setPcoOptions(ue_info.pco_options, ue_info.pco_length);
	prcdCtxt_p->setEsmInfoTxRequired(ue_info.esm_info_tx_required);
	prcdCtxt_p->setAttachType(attachType);

	switch(attachType)
	{
		case imsiAttach_c:
		{
			uint8_t imsi[BINARY_IMSI_LEN] = {0};
		    	memcpy( imsi, ue_info.IMSI, BINARY_IMSI_LEN );

			// Only upper nibble of first octect in imsi need to be considered
			// Changing the lower nibble to 0x0f for handling
			uint8_t first = imsi[0] >> 4;
			imsi[0] = (uint8_t)(( first << 4 ) | 0x0f );

			DigitRegister15 IMSIInfo;
			IMSIInfo.convertFromBcdArray(imsi);
			ueCtxt_p->setImsi(IMSIInfo);

			SubsDataGroupManager::Instance()->addimsikey(ueCtxt_p->getImsi(), ueCtxt_p->getContextID());

			SM::Event evt(VALIDATE_IMSI, NULL);
			cb.addEventToProcQ(evt);

			break;
		}
		case knownGutiAttach_c:
		{
			// copy seq num?

			SM::Event evt(VALIDATE_IMSI, NULL);
			cb.addEventToProcQ(evt);

			break;
		}
		case unknownGutiAttach_c:
		{
			SM::Event evt(VALIDATE_IMSI, NULL);
			cb.addEventToProcQ(evt);

			break;
		}
		default:
		{
			log_msg(LOG_ERROR, "Unhandled attach type %s", attachType);
		}
	}

	return ActStatus::PROCEED;
}

/***************************************
* Action handler : default_detach_req_handler
***************************************/
ActStatus ActionHandlers::default_detach_req_handler(ControlBlock& cb)
{
    MsgBuffer* msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
    if (msgBuf == NULL)
    {
        log_msg(LOG_ERROR, "Failed to retrieve message buffer \n");
        return ActStatus::HALT;
    }

    const s1_incoming_msg_data_t* msgData_p =
            static_cast<const s1_incoming_msg_data_t*>(msgBuf->getDataPointer());
    if (msgData_p == NULL)
    {
        log_msg(LOG_ERROR, "Failed to retrieve data buffer \n");
        return ActStatus::HALT;
    }

    UEContext *ueCtxt = static_cast<UEContext*>(cb.getPermDataBlock());
    if (ueCtxt == NULL)
    {
        log_msg(LOG_DEBUG, "ue context is NULL \n");
        return ActStatus::HALT;
    }

	MmeDetachProcedureCtxt* prcdCtxt_p = SubsDataGroupManager::Instance()->getMmeDetachProcedureCtxt();
	if( prcdCtxt_p == NULL )
	{
		log_msg(LOG_ERROR, "Failed to allocate procedure context for detach cbIndex %d\n", cb.getCBIndex());

		return ActStatus::HALT;
	}

	prcdCtxt_p->setCtxtType( ProcedureType::detach_c );
	prcdCtxt_p->setDetachType( DetachType::ueInitDetach_c);
	ueCtxt->setS1apEnbUeId(msgData_p->s1ap_enb_ue_id);
	prcdCtxt_p->setNextState(DetachStart::Instance());
	cb.setCurrentTempDataBlock(prcdCtxt_p);

	SM::Event evt(DETACH_REQ_FROM_UE, NULL);
	cb.addEventToProcQ(evt);

	return ActStatus::PROCEED;
}

/***************************************
* Action handler : default_ddn_handler
***************************************/
ActStatus ActionHandlers::default_ddn_handler(ControlBlock& cb)
{
	MmeSvcReqProcedureCtxt* svcReqProc_p = SubsDataGroupManager::Instance()->getMmeSvcReqProcedureCtxt();
	if (svcReqProc_p == NULL)
	{
		log_msg(LOG_ERROR, "Failed to allocate procedure context"
				" for DDN handling cbIndex %d\n", cb.getCBIndex());

		return ActStatus::HALT;
	}
	
	MsgBuffer* msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());

	if (msgBuf == NULL)
	{
	    log_msg(LOG_DEBUG,"process_ddn: msgBuf is NULL \n");
	    return ActStatus::HALT;
   	}

   	const gtp_incoming_msg_data_t* gtp_msg_data= static_cast<const gtp_incoming_msg_data_t*>(msgBuf->getDataPointer());
   	const struct ddn_Q_msg& ddn_info = gtp_msg_data->msg_data.ddn_Q_msg_m;

	svcReqProc_p->setCtxtType(ProcedureType::serviceRequest_c);
	svcReqProc_p->setNextState(PagingStart::Instance());
	svcReqProc_p->setPagingTrigger(ddnInit_c);
	svcReqProc_p->setDdnSeqNo(ddn_info.seq_no);
	svcReqProc_p->setArp(Arp(ddn_info.arp));
	svcReqProc_p->setEpsBearerId(ddn_info.eps_bearer_id);

	cb.setCurrentTempDataBlock(svcReqProc_p);
    
	SM::Event evt(DDN_FROM_SGW, NULL);
	cb.addEventToProcQ(evt);
	return ActStatus::PROCEED;
}

/***************************************
* Action handler : default_service_req_handler
***************************************/
ActStatus ActionHandlers::default_service_req_handler(ControlBlock& cb)
{	
	MsgBuffer* msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
	if (msgBuf == NULL)
	{
        	log_msg(LOG_ERROR, "Failed to retrieve message buffer \n");
        	return ActStatus::HALT;
    	}
	const s1_incoming_msg_data_t* msgData_p =
            static_cast<const s1_incoming_msg_data_t*>(msgBuf->getDataPointer());
    	if (msgData_p == NULL)
    	{
        	log_msg(LOG_ERROR, "Failed to retrieve data buffer \n");
        	return ActStatus::HALT;
    	}
    	MmeSvcReqProcedureCtxt* svcReqProc_p = SubsDataGroupManager::Instance()->getMmeSvcReqProcedureCtxt();
	if (svcReqProc_p == NULL)
	{
		log_msg(LOG_ERROR, "Failed to allocate procedure context"
				" for service request cbIndex %d\n", cb.getCBIndex());

		return ActStatus::HALT;
	}

	UEContext *ueCtxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	if (ueCtxt == NULL)
	{
		log_msg(LOG_DEBUG, "ue context is NULL \n");
		return ActStatus::HALT;
	}

	MmContext* mmCtxt = ueCtxt->getMmContext();
	if (mmCtxt == NULL)
	{
		log_msg(LOG_DEBUG, "mm context is NULL \n");
		return ActStatus::HALT;
	}
	
	mmCtxt->setEcmState(ecmConnected_c);
	
	svcReqProc_p->setCtxtType(ProcedureType::serviceRequest_c);
	svcReqProc_p->setNextState(ServiceRequestStart::Instance());
	ueCtxt->setS1apEnbUeId(msgData_p->s1ap_enb_ue_id);
	cb.setCurrentTempDataBlock(svcReqProc_p);

	SM::Event evt(SERVICE_REQUEST_FROM_UE, NULL);
	cb.addEventToProcQ(evt);

    return ActStatus::PROCEED;
}

/***************************************
* Action handler : default_cancel_loc_req_handler
***************************************/
ActStatus ActionHandlers::default_cancel_loc_req_handler(ControlBlock& cb)
{
	UEContext *ueCtxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	if (ueCtxt == NULL)
	{
		log_msg(LOG_DEBUG, "ue context is NULL \n");
		return ActStatus::HALT;
	}

	MmContext* mmCtxt = ueCtxt->getMmContext();
	if (mmCtxt == NULL)
	{
		log_msg(LOG_DEBUG, "mm context is NULL \n");
		return ActStatus::HALT;
	}
	
	ProcedureStats::num_of_clr_received ++;
	ProcedureStats::num_of_cla_sent ++;
	
	if (mmCtxt->getMmState() == EpsDetached)
	{
		log_msg(LOG_INFO, "Subscriber is already detached. "
				"Cleaning up the contexts. UE IDx %d\n", cb.getCBIndex());
		
		MmeContextManagerUtils::deleteUEContext(cb.getCBIndex());		
		return ActStatus::PROCEED;
	}

	MmeDetachProcedureCtxt* prcdCtxt_p = SubsDataGroupManager::Instance()->getMmeDetachProcedureCtxt();
	if(prcdCtxt_p == NULL)
	{
		log_msg(LOG_ERROR, "Failed to allocate Procedure Ctxt\n");
		return ActStatus::HALT;
	}
	prcdCtxt_p->setCtxtType( ProcedureType::detach_c );
	prcdCtxt_p->setDetachType(DetachType::hssInitDetach_c);
	prcdCtxt_p->setNextState(NiDetachStart::Instance());
	prcdCtxt_p->setCancellationType(SUBSCRIPTION_WITHDRAWAL);
	cb.setCurrentTempDataBlock(prcdCtxt_p);

	SM::Event evt(CLR_FROM_HSS, NULL);
	cb.addEventToProcQ(evt);
	
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : default_s1_release_req_handler
***************************************/
ActStatus ActionHandlers::default_s1_release_req_handler(ControlBlock& cb)
{	
	MmeProcedureCtxt* prcdCtxt_p = SubsDataGroupManager::Instance()->getMmeProcedureCtxt();
	if( prcdCtxt_p == NULL )
	{
		log_msg(LOG_ERROR, "Failed to allocate procedure Ctxt \n");
		return ActStatus::HALT;
	}
	
	prcdCtxt_p->setCtxtType( ProcedureType::s1Release_c );
	prcdCtxt_p->setNextState(S1ReleaseStart::Instance());	
	cb.setCurrentTempDataBlock(prcdCtxt_p);

	ProcedureStats::num_of_s1_rel_req_received ++;

	SM::Event evt(S1_REL_REQ_FROM_UE, NULL);
	cb.addEventToProcQ(evt);

	return ActStatus::PROCEED;
}

/***************************************
* Action handler : default_tau_req_handler
***************************************/
ActStatus ActionHandlers::default_tau_req_handler(ControlBlock& cb)
{
	MmeTauProcedureCtxt* tauReqProc_p = SubsDataGroupManager::Instance()->getMmeTauProcedureCtxt();
	if (tauReqProc_p == NULL)
	{
		log_msg(LOG_ERROR, "Failed to allocate procedure context"
				" for tau request cbIndex %d\n", cb.getCBIndex());

		return ActStatus::HALT;
	}

	UEContext *ueCtxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	if (ueCtxt == NULL)
	{
		log_msg(LOG_DEBUG, "ue context is NULL \n");
		return ActStatus::HALT;
	}

	MmContext* mmCtxt = ueCtxt->getMmContext();
	if (mmCtxt == NULL)
	{
		log_msg(LOG_DEBUG, "mm context is NULL \n");
		return ActStatus::HALT;
	}

	mmCtxt->setEcmState(ecmConnected_c);
	
	MsgBuffer* msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
	if (msgBuf == NULL)
	{	
            log_msg(LOG_DEBUG,"process_tau_req: msgBuf is NULL \n");
            return ActStatus::HALT;
	}
	
	const s1_incoming_msg_data_t* msgData_p =
			static_cast<const s1_incoming_msg_data_t*>(msgBuf->getDataPointer());
	if (msgData_p == NULL)
	{
		log_msg(LOG_ERROR, "Failed to retrieve data buffer \n");
		return ActStatus::HALT;
	}

	const struct tauReq_Q_msg &tauReq = (msgData_p->msg_data.tauReq_Q_msg_m);	
	
	tauReqProc_p->setCtxtType(ProcedureType::tau_c);
	tauReqProc_p->setNextState(TauStart::Instance());	
	tauReqProc_p->setS1apEnbUeId(msgData_p->s1ap_enb_ue_id);
	tauReqProc_p->setEnbFd(tauReq.enb_fd);
	cb.setCurrentTempDataBlock(tauReqProc_p);	

	SM::Event evt(TAU_REQUEST_FROM_UE, NULL);
	cb.addEventToProcQ(evt);
	return ActStatus::PROCEED;
}

