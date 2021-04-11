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
#include <contextManager/subsDataGroupManager.h>
#include <mme_app.h>
#include <controlBlock.h>
#include <event.h>
#include <gtpCauseTypes.h>
#include <interfaces/mmeIpcInterface.h>
#include <ipcTypes.h>
#include <mmeSmDefs.h>
#include <mmeStates/attachStates.h>
#include <mmeStates/ueInitDetachStates.h>
#include <mmeStates/networkInitDetachStates.h>
#include <mmeStates/s1ReleaseStates.h>
#include <mmeStates/serviceRequestStates.h>
#include <mmeStates/tauStates.h>
#include <mmeStates/s1HandoverStates.h>
#include <msgBuffer.h>
#include <msgType.h>
#include <log.h>
#include <procedureStats.h>
#include <s1ap_structs.h>
#include <state.h>
#include <string.h>
#include <sstream>
#include <tipcTypes.h>
#include <typeinfo>
#include <utils/mmeCommonUtils.h>
#include <utils/mmeContextManagerUtils.h>
#include "contextManager/dataBlocks.h"
#include "mmeStatsPromClient.h"

using namespace mme;
using namespace SM;
using namespace cmn;

/***************************************
* Action handler : default_attach_req_handler
***************************************/
ActStatus ActionHandlers::default_attach_req_handler(ControlBlock& cb)
{
	log_msg(LOG_ERROR, "default_attach_req_handler ");

	UEContext* ueCtxt_p = NULL;
	MmContext* mmCtxt = NULL;

	ueCtxt_p = static_cast <UEContext *>(cb.getPermDataBlock());
	if (ueCtxt_p != NULL)
		mmCtxt = ueCtxt_p->getMmContext();

	MsgBuffer* msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
	if (msgBuf == NULL)
	{
		log_msg(LOG_ERROR, "Failed to retrieve message buffer ");

		// Invalid message. Cannot proceed further.
		MmeContextManagerUtils::deleteUEContext(cb.getCBIndex());
		return ActStatus::HALT;
	}

	/* Lets see what NAS message has */
	const ue_attach_info_t *ue_info = static_cast<const ue_attach_info_t*>(msgBuf->getDataPointer());
	

	AttachType attachType = MmeCommonUtils::getAttachType(ueCtxt_p, *ue_info);
	if (attachType == maxAttachType_c)
	{
		log_msg(LOG_ERROR, "Failed to identify attach type ");

        MmeContextManagerUtils::deleteUEContext(cb.getCBIndex());
		return ActStatus::HALT;
	}

	if (ueCtxt_p == NULL)
	{
		ueCtxt_p = SubsDataGroupManager::Instance()->getUEContext();
		if (ueCtxt_p == NULL)
		{
			log_msg(LOG_ERROR, "Failed to allocate UE context ");

	        MmeContextManagerUtils::deleteUEContext(cb.getCBIndex());
			return ActStatus::HALT;
		}

		mmCtxt = SubsDataGroupManager::Instance()->getMmContext();
		if( mmCtxt == NULL )
		{
			log_msg(LOG_ERROR, "Failed to allocate MM Context ");

			MmeContextManagerUtils::deleteUEContext(cb.getCBIndex());
			return ActStatus::HALT;
		}

		ueCtxt_p->setContextID(cb.getCBIndex());
		ueCtxt_p->setMmContext( mmCtxt );

		mmCtxt->setEcmState(ecmConnected_c);

		cb.setPermDataBlock(ueCtxt_p);
		cb.setFastAccessBlock(ueCtxt_p, 1);
	}

	MmeAttachProcedureCtxt* prcdCtxt_p = SubsDataGroupManager::Instance()->getMmeAttachProcedureCtxt();
	if( prcdCtxt_p == NULL )
	{
		log_msg(LOG_ERROR, "Failed to allocate Procedure Context ");

		MmeContextManagerUtils::deleteUEContext(cb.getCBIndex());
		return ActStatus::HALT;
	}

    mmeStats::Instance()->increment(mmeStatsCounter::MME_PROCEDURES_ATTACH_PROC_IMSI);
	prcdCtxt_p->setCtxtType( ProcedureType::attach_c );
	prcdCtxt_p->setNextState(AttachStart::Instance());

	cb.addTempDataBlock(prcdCtxt_p);
	
	MmeCommonUtils::formatS1apPlmnId(const_cast<PLMN*>(&ue_info->tai.plmn_id));
	MmeCommonUtils::formatS1apPlmnId(const_cast<PLMN*>(&ue_info->utran_cgi.plmn_id));

	// Copy attach request message data into UE Context

	ueCtxt_p->setS1apEnbUeId(ue_info->header.s1ap_enb_ue_id);
	ueCtxt_p->setEnbFd(ue_info->enb_fd);
	ueCtxt_p->setTai(Tai(ue_info->tai));
	ueCtxt_p->setUtranCgi(Cgi(ue_info->utran_cgi));
	ueCtxt_p->setUeNetCapab(Ue_net_capab(ue_info->ue_net_capab));
	ueCtxt_p->setMsNetCapab(Ms_net_capab(ue_info->ms_net_capab));
	ueCtxt_p->setUeAddSecCapabPres(ue_info->ue_add_sec_cap_present);
    	ueCtxt_p->setUeAddSecCapab(ue_info->ue_add_sec_capab);
	prcdCtxt_p->setPti(ue_info->pti);
	prcdCtxt_p->setPcoOptions(ue_info->pco_options, ue_info->pco_length);
	prcdCtxt_p->setEsmInfoTxRequired(ue_info->esm_info_tx_required);
	prcdCtxt_p->setAttachType(attachType);

	ActStatus status = ActStatus::PROCEED;
	switch(attachType)
	{
		case imsiAttach_c:
		{
			uint8_t imsi[BINARY_IMSI_LEN] = {0};
		    	memcpy( imsi, ue_info->IMSI, BINARY_IMSI_LEN );

			// Only upper nibble of first octect in imsi need to be considered
			// Changing the lower nibble to 0x0f for handling
			uint8_t first = imsi[0] >> 4;
			imsi[0] = (uint8_t)(( first << 4 ) | 0x0f );

			DigitRegister15 IMSIInfo;
			IMSIInfo.convertFromBcdArray(imsi);
			ueCtxt_p->setImsi(IMSIInfo);

			SubsDataGroupManager::Instance()->addimsikey(ueCtxt_p->getImsi(), ueCtxt_p->getContextID());

			SM::Event evt(VALIDATE_IMSI, NULL);
			cb.qInternalEvent(evt);

			break;
		}
		case knownGutiAttach_c:
		{
			// copy seq num?

			SM::Event evt(VALIDATE_IMSI, NULL);
			cb.qInternalEvent(evt);

			break;
		}
		case unknownGutiAttach_c:
		{
			SM::Event evt(VALIDATE_IMSI, NULL);
			cb.qInternalEvent(evt);

			break;
		}
		default:
		{
			log_msg(LOG_ERROR, "Unhandled attach type %s", attachType);

			MmeContextManagerUtils::deleteUEContext(cb.getCBIndex());
			status = ActStatus::HALT;
		}
	}

	ProcedureStats::num_of_attach_req_received ++;
	
	return status;
}

/***************************************
* Action handler : default_detach_req_handler
***************************************/
ActStatus ActionHandlers::default_detach_req_handler(ControlBlock& cb)
{
    MsgBuffer* msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
    VERIFY(msgBuf, return ActStatus::PROCEED, "Detach Hdlr: message buffer is NULL ");

    const s1_incoming_msg_header_t* msgData_p =
            static_cast<const s1_incoming_msg_header_t*>(msgBuf->getDataPointer());
    VERIFY(msgBuf, return ActStatus::PROCEED, "Detach Hdlr: message data buffer is NULL ");

    UEContext *ueCtxt = static_cast<UEContext*>(cb.getPermDataBlock());
    VERIFY_UE(cb, ueCtxt, "Detach Hdlr: UE Context is NULL ");

    MmeDetachProcedureCtxt* prcdCtxt_p = MmeContextManagerUtils::allocateDetachProcedureCtxt(cb, ueInitDetach_c);
    if( prcdCtxt_p == NULL )
    {
        log_msg(LOG_ERROR, "Failed to allocate procedure context for detach cbIndex %d", cb.getCBIndex());
        MmeContextManagerUtils::deleteUEContext(cb.getCBIndex());
        return ActStatus::PROCEED;
    }

    ueCtxt->setS1apEnbUeId(msgData_p->s1ap_enb_ue_id);
    
    SM::Event evt(DETACH_REQ_FROM_UE, NULL);
    cb.qInternalEvent(evt);
    
    ProcedureStats::num_of_detach_req_received ++;
    
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : default_ddn_handler
***************************************/
ActStatus ActionHandlers::default_ddn_handler(ControlBlock& cb)
{
    ProcedureStats::num_of_ddn_received++;

    MsgBuffer *msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());

    VERIFY(msgBuf, return ActStatus::PROCEED,
            "default_ddn_handler: Invalid message buffer ");

    VERIFY(msgBuf->getLength() >= sizeof(struct ddn_Q_msg),
            return ActStatus::PROCEED, "default_ddn_handler: Invalid DDN message length ");

    const struct ddn_Q_msg *ddn_info =
    		static_cast<const ddn_Q_msg*>(msgBuf->getDataPointer());

    uint8_t gtpCause = GTPV2C_CAUSE_REQUEST_ACCEPTED;
    int sgw_cp_teid = 0;

    UEContext *ueCtxt = static_cast<UEContext*>(cb.getPermDataBlock());
    if (ueCtxt != NULL)
    {
	auto& sessionCtxtContainer = ueCtxt->getSessionContextContainer();
	if(sessionCtxtContainer.size() > 0)
	{
    	    SessionContext* sess_p = sessionCtxtContainer.front();
	    /*We will fetch the s11_sgw_cp_teid from the session context if it's available*/
            sgw_cp_teid = sess_p->getS11SgwCtrlFteid().fteid_m.header.teid_gre;

            MmeSvcReqProcedureCtxt *svcReqProc_p =
                    MmeContextManagerUtils::allocateServiceRequestProcedureCtxt(
                            cb, ddnInit_c);
            if (svcReqProc_p != NULL)
            {
                svcReqProc_p->setDdnSeqNo(ddn_info->seq_no);
                svcReqProc_p->setArp(Arp(ddn_info->arp));
                svcReqProc_p->setEpsBearerId(ddn_info->eps_bearer_id);

                SM::Event evt(DDN_FROM_SGW, NULL);
                cb.qInternalEvent(evt);
            }
            else
            {
                log_msg(LOG_INFO,
                        "default_ddn_handler: Failed to allocate procedure context ");
                gtpCause = GTPV2C_CAUSE_REQUEST_REJECTED;
            }
        }
        else
        {
            log_msg(LOG_INFO,
                    "default_ddn_handler: Failed to find session context ");
            gtpCause = GTPV2C_CAUSE_REQUEST_REJECTED;
        }
    }
    else
    {
        log_msg(LOG_INFO,
                "default_ddn_handler: Failed to find UE context ");

        gtpCause = GTPV2C_CAUSE_CONTEXT_NOT_FOUND;

        MmeContextManagerUtils::deleteUEContext(cb.getCBIndex());
    }

    if (gtpCause != GTPV2C_CAUSE_REQUEST_ACCEPTED)
    {
        struct DDN_ACK_Q_msg ddnAck;
        ddnAck.msg_type = ddn_acknowledgement;
	/*Incase of unavailability of session/UE Contexts , s11_sgw_cp_teid will be set as 0 */
        ddnAck.s11_sgw_c_fteid.header.teid_gre = sgw_cp_teid;
	ddnAck.s11_sgw_c_fteid.ip.ipv4.s_addr = ddn_info->sgw_ip;
        ddnAck.seq_no= ddn_info->seq_no;
        ddnAck.cause = gtpCause;

        cmn::ipc::IpcAddress destAddr;
        destAddr.u32 = TipcServiceInstance::s11AppInstanceNum_c;
        
        mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_S11_DOWNLINK_DATA_NOTIFICATION_ACK);
        MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));        
        mmeIpcIf.dispatchIpcMsg((char *) &ddnAck, sizeof(ddnAck), destAddr);
    }

    return ActStatus::PROCEED;
}

/***************************************
* Action handler : default_service_req_handler
***************************************/
ActStatus ActionHandlers::default_service_req_handler(ControlBlock& cb)
{
    log_msg(LOG_DEBUG, "default_service_req_handler ");

    ProcedureStats::num_of_service_request_received++;

    MsgBuffer *msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
    if (msgBuf == NULL)
    {
        log_msg(LOG_ERROR, "Failed to retrieve message buffer ");
        return ActStatus::HALT;
    }

    service_req_Q_msg_t *serviceReq =
            static_cast<service_req_Q_msg_t*>(msgBuf->getDataPointer());

    log_msg(LOG_INFO, "Service request received from enb context %u & UEIndex/TMSI = %u ", serviceReq->enb_fd, serviceReq->header.ue_idx);
    unsigned char emmCause = 0;
    int ue_idx = serviceReq->header.ue_idx;
    int s1ap_enb_ue_id = serviceReq->header.s1ap_enb_ue_id;
    int enb_fd = serviceReq->enb_fd;

    UEContext *ueCtxt = static_cast<UEContext*>(cb.getPermDataBlock());
    if (ueCtxt != NULL)
    {
        MmContext *mmCtxt = ueCtxt->getMmContext();
        if (mmCtxt != NULL)
        {
            MmeSvcReqProcedureCtxt *srvReqProc_p =
                    MmeContextManagerUtils::allocateServiceRequestProcedureCtxt(cb, none_c);
            if (srvReqProc_p != NULL)
            {
                log_msg(LOG_ERROR, "Allocated service request procedure context ");
                mmCtxt->setEcmState(ecmConnected_c);
                ueCtxt->setS1apEnbUeId(serviceReq->header.s1ap_enb_ue_id);

                SM::Event evt(SERVICE_REQUEST_FROM_UE, NULL);
                cb.qInternalEvent(evt);
            }
            else
            {
                log_msg(LOG_ERROR, "Failed to allocate procedure context ");

                emmCause = emmCause_network_failure;
            }
        }
        else
        {
            log_msg(LOG_ERROR, "Invalid UE Context in service request handling");

            emmCause = emmCause_ue_id_not_derived_by_network;
            MmeContextManagerUtils::deleteUEContext(cb.getCBIndex());
        }
    }
    else
    {
        log_msg(LOG_ERROR, "UE Context is NULL in service request handling %u ",serviceReq->enb_fd);
        emmCause = emmCause_ue_id_not_derived_by_network;
        MmeContextManagerUtils::deleteUEContext(cb.getCBIndex());
    }

    if (emmCause != 0)
    {
        log_msg(LOG_INFO, "Send service request reject for enb context %u ", serviceReq->enb_fd);
        struct commonRej_info serviceRej =
        {
                service_reject,
                ue_idx,
                s1ap_enb_ue_id,
                enb_fd,
                emmCause
        };

        cmn::ipc::IpcAddress destAddr;
        destAddr.u32 = TipcServiceInstance::s1apAppInstanceNum_c;
        
    mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_NAS_SERVICE_REJECT);
	MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));
	mmeIpcIf.dispatchIpcMsg((char *) &serviceRej, sizeof(struct commonRej_info), destAddr);
    }

    return ActStatus::PROCEED;
}

/***************************************
* Action handler : default_cancel_loc_req_handler
***************************************/
ActStatus ActionHandlers::default_cancel_loc_req_handler(ControlBlock& cb)
{
	UEContext *ueCtxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ueCtxt, "CLR Hdlr: UE Context is NULL ");

	MmContext* mmCtxt = ueCtxt->getMmContext();
    	VERIFY_UE(cb, mmCtxt, "CLR Hdlr: MmContext is NULL ");
	
	ProcedureStats::num_of_clr_received ++;
	ProcedureStats::num_of_cla_sent ++;
	
	if (mmCtxt->getMmState() == EpsDetached)
	{
		log_msg(LOG_INFO, "Subscriber is already detached. "
				"Cleaning up the contexts. UE IDx %d", cb.getCBIndex());
		
		MmeContextManagerUtils::deleteUEContext(cb.getCBIndex());		
		return ActStatus::PROCEED;
	}

	MmeDetachProcedureCtxt* prcdCtxt_p =
	        MmeContextManagerUtils::allocateDetachProcedureCtxt(cb, hssInitDetach_c);
	if(prcdCtxt_p == NULL)
	{
        	log_msg(LOG_ERROR,
                	"Failed to allocate procedure context for detach cbIndex %d",
                	cb.getCBIndex());
        	MmeContextManagerUtils::deleteUEContext(cb.getCBIndex());
        	return ActStatus::PROCEED;
	}
	prcdCtxt_p->setCancellationType(SUBSCRIPTION_WITHDRAWAL);
	prcdCtxt_p->setNasDetachType(reattachNotRequired);

	SM::Event evt(CLR_FROM_HSS, NULL);
	cb.qInternalEvent(evt);
	
	return ActStatus::PROCEED;
}

/***************************************
* Action handler : default_s1_release_req_handler
***************************************/
ActStatus ActionHandlers::default_s1_release_req_handler(ControlBlock& cb)
{	
    MsgBuffer *msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
    if (msgBuf == NULL)
    {
        log_msg(LOG_ERROR, "Failed to retrieve message buffer ");
	// Wait for UE to retry.
        return ActStatus::PROCEED;
    }

    s1_incoming_msg_header_t *msgData_p =
            static_cast<s1_incoming_msg_header_t*>(msgBuf->getDataPointer());
    if (msgData_p == NULL)
    {
        log_msg(LOG_ERROR, "Failed to retrieve data buffer ");
	// Wait for UE to retry.
        return ActStatus::PROCEED;
    }

    MmeS1RelProcedureCtxt* prcdCtxt_p = SubsDataGroupManager::Instance()->getMmeS1RelProcedureCtxt();
    if( prcdCtxt_p == NULL )
    {
	log_msg(LOG_ERROR, "Failed to allocate procedure Ctxt ");
	// Wait for UE to retry.
	return ActStatus::PROCEED;
    }
	
    prcdCtxt_p->setS1apEnbUeId(msgData_p->s1ap_enb_ue_id);
	prcdCtxt_p->setCtxtType( ProcedureType::s1Release_c );
	prcdCtxt_p->setNextState(S1ReleaseStart::Instance());	
	cb.addTempDataBlock(prcdCtxt_p);

    ProcedureStats::num_of_s1_rel_req_received ++;

	SM::Event evt(S1_REL_REQ_FROM_UE, NULL);
	cb.qInternalEvent(evt);

    return ActStatus::PROCEED;
}

/***************************************
* Action handler : default_tau_req_handler
***************************************/
ActStatus ActionHandlers::default_tau_req_handler(ControlBlock& cb)
{
    log_msg(LOG_DEBUG, "default_tau_req_handler: Entry ");

    ProcedureStats::num_of_tau_req_received++;

    MsgBuffer *msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
    if (msgBuf == NULL)
    {
        log_msg(LOG_DEBUG, "process_tau_req: msgBuf is NULL ");
        return ActStatus::HALT;
    }

    const tauReq_Q_msg_t *tauReq =
            static_cast<const tauReq_Q_msg_t*>(msgBuf->getDataPointer());

    unsigned char emmCause = 0;
    int ue_m_tmsi = tauReq->ue_m_tmsi;
    int s1ap_enb_ue_id = tauReq->header.s1ap_enb_ue_id;
    int enb_fd = tauReq->enb_fd;

    UEContext *ueCtxt = static_cast<UEContext*>(cb.getPermDataBlock());
    if (ueCtxt != NULL)
    {
        MmContext *mmCtxt = ueCtxt->getMmContext();
        if (mmCtxt != NULL)
        {
            MmeTauProcedureCtxt *tauReqProc_p =
                    MmeContextManagerUtils::allocateTauProcedureCtxt(cb);
            if (tauReqProc_p != NULL)
            {
                mmCtxt->setEcmState(ecmConnected_c);

                if(tauReq->ue_net_capab.len > 0)
                {
                    ueCtxt->setUeNetCapab(Ue_net_capab(tauReq->ue_net_capab));
                }
                if(tauReq->ue_add_sec_cap_present)
                {
                    ueCtxt->setUeAddSecCapabPres(tauReq->ue_add_sec_cap_present);
                    ueCtxt->setUeAddSecCapab(tauReq->ue_add_sec_capab);
                }

                tauReqProc_p->setS1apEnbUeId(tauReq->header.s1ap_enb_ue_id);
                tauReqProc_p->setEnbFd(tauReq->enb_fd);

                //TAI and CGI obtained from s1ap ies.
                //Convert the PLMN in s1ap format to nas format before storing in procedure context.
                MmeCommonUtils::formatS1apPlmnId(const_cast<PLMN*>(&tauReq->tai.plmn_id));
                MmeCommonUtils::formatS1apPlmnId(const_cast<PLMN*>(&tauReq->eUtran_cgi.plmn_id));
                tauReqProc_p->setTai(Tai(tauReq->tai));
                tauReqProc_p->setEUtranCgi(Cgi(tauReq->eUtran_cgi));
                SM::Event evt(TAU_REQUEST_FROM_UE, NULL);
                cb.qInternalEvent(evt);
            }
            else
            {
                log_msg(LOG_ERROR, "Failed to allocate procedure context ");

                emmCause = emmCause_network_failure;
            }
        }
        else
        {
            log_msg(LOG_ERROR, "Invalid UE Context ");

            emmCause = emmCause_ue_id_not_derived_by_network;
            MmeContextManagerUtils::deleteUEContext(cb.getCBIndex());
        }
    }
    else
    {
        log_msg(LOG_ERROR, "UE Context is NULL ");

        emmCause = emmCause_ue_id_not_derived_by_network;
        MmeContextManagerUtils::deleteUEContext(cb.getCBIndex());
    }

    if (emmCause != 0)
    {
        struct commonRej_info tauRej =
        {
                tau_reject,
		        ue_m_tmsi,
                s1ap_enb_ue_id,
                enb_fd,
                emmCause
        };

        cmn::ipc::IpcAddress destAddr;
        destAddr.u32 = TipcServiceInstance::s1apAppInstanceNum_c;
        
        mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_NAS_TAU_RESPONSE);
        MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));
        mmeIpcIf.dispatchIpcMsg((char *) &tauRej, sizeof(tauRej), destAddr);
    }

    return ActStatus::PROCEED;
}

/***************************************
* Action handler : default_s1_ho_handler
***************************************/
ActStatus ActionHandlers::default_s1_ho_handler(ControlBlock& cb)
{
    MsgBuffer* msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
    if (msgBuf == NULL)
    {
        log_msg(LOG_DEBUG,"process_handover_required: msgBuf is NULL ");
        return ActStatus::HALT;
    }

    const handover_required_Q_msg_t *hoReq =
            static_cast<const handover_required_Q_msg_t*>(msgBuf->getDataPointer());
    if (hoReq == NULL)
    {
        log_msg(LOG_ERROR, "Failed to retrieve data buffer ");
        return ActStatus::HALT;
    }

	S1HandoverProcedureContext* hoReqProc_p = MmeContextManagerUtils::allocateHoContext(cb);
	if (hoReqProc_p == NULL)
	{
		log_msg(LOG_ERROR, "Failed to allocate procedure context"
				" for ho required cbIndex %d", cb.getCBIndex());
		return ActStatus::HALT;
	}

	UEContext *ueCtxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	if (ueCtxt == NULL)
	{
		log_msg(LOG_DEBUG, "ue context is NULL ");
		return ActStatus::HALT;
	}

	hoReqProc_p->setS1HoCause(hoReq->cause);
	hoReqProc_p->setTargetEnbContextId(hoReq->target_enb_context_id);
	hoReqProc_p->setSrcToTargetTransContainer(hoReq->srcToTargetTranspContainer);
	hoReqProc_p->setTargetTai(hoReq->target_id.selected_tai);
	hoReqProc_p->setSrcS1apEnbUeId(hoReq->header.s1ap_enb_ue_id);
	hoReqProc_p->setSrcEnbContextId(hoReq->src_enb_context_id);

	ProcedureStats::num_of_ho_required_received++;

	SM::Event evt(INTRA_S1HO_START, NULL);
	cb.qInternalEvent(evt);
    return ActStatus::PROCEED;
}

/******************************************************
* Action handler : default_erab_mod_indication_handler
*******************************************************/
ActStatus ActionHandlers::default_erab_mod_indication_handler(ControlBlock& cb)
{
    log_msg(LOG_DEBUG, "default_erab_mod_indication_handler: Entry ");

    ProcedureStats::num_of_erab_mod_ind_received++;

    UEContext *ueCtxt = static_cast<UEContext*>(cb.getPermDataBlock());
    if (ueCtxt == NULL)
    {
        log_msg(LOG_ERROR, "UE Context is NULL ");

        MmeContextManagerUtils::deleteUEContext(cb.getCBIndex());
        return ActStatus::HALT;
    }

    MsgBuffer *msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
    if (msgBuf == NULL)
    {
        log_msg(LOG_DEBUG, "process_erab_mod_indication: msgBuf is NULL ");
        return ActStatus::PROCEED;
    }

    const erab_mod_ind_Q_msg_t *erabModInd = static_cast<const erab_mod_ind_Q_msg_t*>(msgBuf->getDataPointer());
    if (erabModInd == NULL)
    {
        log_msg(LOG_ERROR, "Failed to retrieve data buffer ");
        return ActStatus::PROCEED;
    }

    MmeErabModIndProcedureCtxt *erabModIndProc_p =
            MmeContextManagerUtils::allocateErabModIndProcedureCtxt(cb);
    if (erabModIndProc_p != NULL)
    {
        erabModIndProc_p->setErabToBeModifiedList(
                erabModInd->erab_to_be_mod_list.erab_to_be_mod_item,
                erabModInd->erab_to_be_mod_list.count);

        SM::Event evt(eRAB_MOD_IND_START, NULL);
        cb.qInternalEvent(evt);
    }
    else
    {
        log_msg(LOG_INFO, "Failed to allocate procedure context ");
    }

    return ActStatus::PROCEED;
}

/***************************************
 * Action handler : default_create_bearer_req_handler
 ***************************************/
ActStatus ActionHandlers::default_create_bearer_req_handler(ControlBlock &cb)
{
    log_msg(LOG_DEBUG, "default_create_bearer_req_handler: Entry ");
    ProcedureStats::num_of_create_bearer_req_received++;

    MsgBuffer *msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
    // Invalid buffer. Nothing to do, wait for gw to retry.
    VERIFY(msgBuf, return ActStatus::PROCEED,
            "Invalid create bearer request msg buffer");

    const cb_req_Q_msg *cb_req =
            static_cast<const cb_req_Q_msg*>(msgBuf->getDataPointer());
    VERIFY(cb_req, return ActStatus::PROCEED,
            "Invalid create bearer request data");

    uint8_t gtpCause = 0;
    int sgw_cp_teid = 0;

    UEContext *ueCtxt = static_cast<UEContext*>(cb.getPermDataBlock());
    if (ueCtxt != NULL)
    {
        MmContext *mmCtxt = ueCtxt->getMmContext();
        if (mmCtxt != NULL)
        {
            MmeSmCreateBearerProcCtxt *cbReqProc_p =
                    MmeContextManagerUtils::allocateCreateBearerRequestProcedureCtxt(
                            cb, cb_req->linked_eps_bearer_id);

            if (cbReqProc_p != NULL)
            {
                cbReqProc_p->setCreateBearerReqEMsg(cb.getEventMessage());
                MmeSvcReqProcedureCtxt *srvReqProc_p = NULL;
                srvReqProc_p =
                        dynamic_cast<MmeSvcReqProcedureCtxt*>(MmeContextManagerUtils::findProcedureCtxt(cb, serviceRequest_c));
                if(srvReqProc_p != NULL)
                {
                    // If a service request is already in progress, just set the paging flag,
                    // so that at the end of service request procedure, CBReq procedure will
                    // be informed and can proceed with the ded activation.
                    srvReqProc_p->setPagingTriggerBit(pgwInit_c);

                    SM::Event evt(GW_CP_REQ_INIT_PAGING, NULL);
                    cb.qInternalEvent(evt);
                }
                else if(mmCtxt->getEcmState() == ecmIdle_c)
                {
                    // If ECM state is ECM idle, allocate service request and set paging
                    // trigger as pgwInit_c, so that at the end of service request procedure,
                    // CBReq procedure will be informed and can proceed with the ded activation.
                    srvReqProc_p =
                            MmeContextManagerUtils::allocateServiceRequestProcedureCtxt(
                                    cb, pgwInit_c);
                    if(srvReqProc_p != NULL)
                    {
                        SM::Event evt(GW_CP_REQ_INIT_PAGING, NULL);
                        cb.qInternalEvent(evt);
                    }
                    else
                    {
                        log_msg(LOG_ERROR,
                                "Failed to allocate context for paging procedure.");
                        gtpCause = GTPV2C_CAUSE_UNABLE_TO_PAGE_UE;
                    }

                }
                else
                {
                    SM::Event evt(CREATE_BEARER_START, NULL);
                    cb.qInternalEvent(evt);
                }
            }
            else
            {
                log_msg(LOG_INFO,
                        "Failed to allocate context for create bearer procedure.");
                gtpCause = GTPV2C_CAUSE_REQUEST_REJECTED;
            }
        }
        else
        {
            log_msg(LOG_ERROR, "Invalid UE Context. MmContext is NULL ");
            gtpCause = GTPV2C_CAUSE_CONTEXT_NOT_FOUND;
        }
    }
    else
    {
        log_msg(LOG_ERROR, "UE Context is NULL ");

        gtpCause = GTPV2C_CAUSE_CONTEXT_NOT_FOUND;
        MmeContextManagerUtils::deleteUEContext(cb.getCBIndex());
    }

    if (gtpCause != 0)
    {
        struct CB_RESP_Q_msg cbRsp;
        memset(&cbRsp, 0, sizeof(cbRsp));
        cbRsp.msg_type = create_bearer_response;
        cbRsp.ue_idx = cb_req->s11_mme_cp_teid;
        cbRsp.seq_no = cb_req->seq_no;
        cbRsp.cause = gtpCause;
        /*Incase of unavailability of session/UE Contexts , s11_sgw_cp_teid will be set as 0 */
        cbRsp.s11_sgw_c_fteid.header.teid_gre = sgw_cp_teid;
        cbRsp.s11_sgw_c_fteid.ip.ipv4.s_addr = cb_req->sgw_ip;
        cbRsp.destination_port = cb_req->source_port;

        cmn::ipc::IpcAddress destAddr;
        destAddr.u32 = TipcServiceInstance::s11AppInstanceNum_c;

        mmeStats::Instance()->increment(
                mmeStatsCounter::MME_MSG_TX_S11_CREATE_BEARER_RESPONSE);
        FIND_COMPONENT(MmeIpcInterface, MmeIpcInterfaceCompId).
        		dispatchIpcMsg((char*) &cbRsp, sizeof(cbRsp), destAddr);
    }

    return ActStatus::PROCEED;
}

/***************************************
* Action handler : handle_paging_failure
***************************************/
ActStatus ActionHandlers::handle_paging_failure(ControlBlock& cb)
{
    log_msg(LOG_DEBUG, "handle_paging_failure: Entry ");

    ActStatus rc = ActStatus::PROCEED;

    MmeProcedureCtxt *procCtxt_p =
            dynamic_cast<MmeProcedureCtxt*>(cb.getTempDataBlock());
    if (procCtxt_p != NULL)
    {
        if (procCtxt_p->getCtxtType() == cbReq_c)
        {
            MmeSmCreateBearerProcCtxt *cbReqProc_p =
                    dynamic_cast<MmeSmCreateBearerProcCtxt*>(procCtxt_p);

            cbReqProc_p->setMmeErrorCause(PAGING_FAILED);

            rc = ActStatus::ABORT;
        }
        else if (procCtxt_p->getCtxtType() == dbReq_c)
        {
            MmeSmDeleteBearerProcCtxt *dbReqProc_p =
                    dynamic_cast<MmeSmDeleteBearerProcCtxt*>(procCtxt_p);

            dbReqProc_p->setMmeErrorCause(PAGING_FAILED);

            rc = ActStatus::ABORT;
        }
    }

    log_msg(LOG_DEBUG, "handle_paging_failure: Exit ");

    return rc;
}

/***************************************
* Action handler : default_delete_bearer_req_handler
***************************************/
ActStatus ActionHandlers::default_delete_bearer_req_handler(ControlBlock& cb)
{
    log_msg(LOG_DEBUG, "default_delete_bearer_req_handler: Entry ");
	
    MsgBuffer *msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
    // Invalid buffer. Nothing to do, wait for gw to retry.
    VERIFY(msgBuf, return ActStatus::PROCEED,
            "Invalid delete bearer request msg buffer");

    const db_req_Q_msg *db_req =
            static_cast<const db_req_Q_msg*>(msgBuf->getDataPointer());
    VERIFY(db_req, return ActStatus::PROCEED,
            "Invalid delete bearer request data");

    uint8_t gtpCause = 0;
    int sgw_cp_teid = 0;

    UEContext *ueCtxt = static_cast<UEContext*>(cb.getPermDataBlock());
    if (ueCtxt != NULL)
    {
        SessionContext* sessCtxt = NULL;
        if (db_req->linked_bearer_id != 0)
        {
            sessCtxt = ueCtxt->findSessionContextByLinkedBearerId(db_req->linked_bearer_id);
        }
        else
        {
            sessCtxt = MmeContextManagerUtils::findSessionCtxtForEpsBrId(
                    db_req->eps_bearer_ids[0], ueCtxt);
        }
        if (sessCtxt)
        {
            MmContext *mmCtxt = ueCtxt->getMmContext();
            if (mmCtxt != NULL)
            {
                MmeSmDeleteBearerProcCtxt *dbReqProc_p =
                        MmeContextManagerUtils::allocateDeleteBearerRequestProcedureCtxt(
                                cb, sessCtxt->getLinkedBearerId());

                if (dbReqProc_p != NULL)
                {
                    dbReqProc_p->setDeleteBearerReqEMsg(cb.getEventMessage());
                    if (db_req->linked_bearer_id)
                        dbReqProc_p->setLbiPresent(true);
                    MmeSvcReqProcedureCtxt *srvReqProc_p = NULL;
                    srvReqProc_p =
                            dynamic_cast<MmeSvcReqProcedureCtxt*>(MmeContextManagerUtils::findProcedureCtxt(
                                    cb, serviceRequest_c));
                    if (srvReqProc_p != NULL)
                    {
                        // If a service request is already in progress, just set the paging flag,
                        // so that at the end of service request procedure, DBReq procedure will
                        // be informed and can proceed with the ded deactivation.
                        srvReqProc_p->setPagingTriggerBit(pgwInit_c);

                        SM::Event evt(GW_CP_REQ_INIT_PAGING, NULL);
                        cb.qInternalEvent(evt);
                    }
                    else if (mmCtxt->getEcmState() == ecmIdle_c)
                    {
                        log_msg(LOG_DEBUG,"UE is IDLE");
                        if (db_req->linked_bearer_id == 0 && db_req->cause != GTPV2C_CAUSE_REACTIVATION_REQUESTED)
                        {
                            for (int i = 0; i < db_req->eps_bearer_ids_count; i++)
                            {
                                BearerContext *bearerCtxt_p =
                                    MmeContextManagerUtils::findBearerContext(
                                        db_req->eps_bearer_ids[i], ueCtxt, sessCtxt);

                                if (bearerCtxt_p)
                                {
                                    MmeContextManagerUtils::deallocateBearerContext(cb,
                                        bearerCtxt_p, sessCtxt, ueCtxt);

                                    gtpCause = GTPV2C_CAUSE_REQUEST_ACCEPTED;
                                }
                            }
                        }
                        else
                        {
                            // If ECM state is ECM idle, allocate service request and set paging
                            // trigger as pgwInit_c, so that at the end of service request procedure,
                            // DBReq procedure will be informed and can proceed with the ded deactivation.
                            log_msg(LOG_DEBUG,"In Idle");
                            srvReqProc_p =
                                MmeContextManagerUtils::allocateServiceRequestProcedureCtxt(
                                        cb, pgwInit_c);
                            if (srvReqProc_p != NULL)
                            {
                                SM::Event evt(GW_CP_REQ_INIT_PAGING, NULL);
                                cb.qInternalEvent(evt);
                            }
                            else
                            {
                                log_msg(LOG_ERROR,
                                    "Failed to allocate context for paging procedure.");
                                gtpCause = GTPV2C_CAUSE_UNABLE_TO_PAGE_UE;
                            }
                        }
                    }
                    else
                    {
                        if ((db_req->linked_bearer_id)
                                && (ueCtxt->getSessionContextContainer().size()
                                        == 1))
                        {
                            SM::Event evt(START_UE_DETACH, NULL);
                            cb.qInternalEvent(evt);
                        }
                        else
                        {
                            SM::Event evt(START_DED_DEACTIVATION, NULL);
                            cb.qInternalEvent(evt);
                        }
                    }
                }
                else
                {
                    log_msg(LOG_INFO,
                            "Failed to allocate context for delete bearer procedure.");
                    gtpCause = GTPV2C_CAUSE_REQUEST_REJECTED;
                }
            }
            else
            {
                log_msg(LOG_ERROR, "Invalid UE Context. MmContext is NULL ");
                gtpCause = GTPV2C_CAUSE_CONTEXT_NOT_FOUND;
            }
        }
        else
        {
            log_msg(LOG_ERROR, "Invalid UE Context. Session Context is NULL ");
            gtpCause = GTPV2C_CAUSE_CONTEXT_NOT_FOUND;
        }
    }
    else
    {
        log_msg(LOG_ERROR, "UE Context is NULL ");

        gtpCause = GTPV2C_CAUSE_CONTEXT_NOT_FOUND;
        MmeContextManagerUtils::deleteUEContext(cb.getCBIndex());
    }

    if (gtpCause != 0)
    {
        struct DB_RESP_Q_msg dbRsp;
        memset(&dbRsp, 0, sizeof(dbRsp));
        dbRsp.msg_type = delete_bearer_response;
        dbRsp.ue_idx = db_req->s11_mme_cp_teid;
        dbRsp.seq_no = db_req->seq_no;
        dbRsp.cause = gtpCause;
        /*Incase of unavailability of session/UE Contexts , s11_sgw_cp_teid will be set as 0 */
        dbRsp.s11_sgw_c_fteid.header.teid_gre = sgw_cp_teid;
        dbRsp.s11_sgw_c_fteid.ip.ipv4.s_addr = db_req->sgw_ip;
        dbRsp.destination_port = db_req->source_port;

        cmn::ipc::IpcAddress destAddr;
        destAddr.u32 = TipcServiceInstance::s11AppInstanceNum_c;

        mmeStats::Instance()->increment(
                mmeStatsCounter::MME_MSG_TX_S11_DELETE_BEARER_RESPONSE);
        FIND_COMPONENT(MmeIpcInterface, MmeIpcInterfaceCompId).
        		dispatchIpcMsg((char*) &dbRsp, sizeof(dbRsp), destAddr);
    }
	
    log_msg(LOG_DEBUG, "default_delete_bearer_req_handler: Exit ");
	
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : handle_detach_failure
***************************************/
ActStatus ActionHandlers::handle_detach_failure(ControlBlock& cb)
{
    log_msg(LOG_DEBUG, "handle_detach_failure: Entry ");

    ActStatus rc = ActStatus::PROCEED;

    MmeProcedureCtxt *procCtxt_p =
            dynamic_cast<MmeProcedureCtxt*>(cb.getTempDataBlock());
    if(procCtxt_p != NULL)
    {
        if(procCtxt_p->getCtxtType() == dbReq_c)
        {
            MmeSmDeleteBearerProcCtxt *dbReqProc_p =
                    dynamic_cast<MmeSmDeleteBearerProcCtxt*>(procCtxt_p);

            dbReqProc_p->setMmeErrorCause(DETACH_FAILED);

            rc = ActStatus::ABORT;
        }
        else if(procCtxt_p->getCtxtType() == defaultMmeProcedure_c)
        {
            MmeContextManagerUtils::deleteUEContext(cb.getCBIndex());
        }
    }
    log_msg(LOG_DEBUG, "handle_detach_failure: Exit ");

    return rc;
}

