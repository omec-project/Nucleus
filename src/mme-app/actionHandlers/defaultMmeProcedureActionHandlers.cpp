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
#include <mmeStates/attachStart.h>
#include <mmeStates/detachStart.h>
#include <mmeStates/niDetachStart.h>
#include <mmeStates/pagingStart.h>
#include <mmeStates/s1ReleaseStart.h>
#include <mmeStates/serviceRequestStart.h>
#include <mmeStates/tauStart.h>
#include <mmeStates/intraS1HoStart.h>
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

	/* Lets see what NAS message has */
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

	MmeAttachProcedureCtxt* prcdCtxt_p = SubsDataGroupManager::Instance()->getMmeAttachProcedureCtxt();
	if( prcdCtxt_p == NULL )
	{
		log_msg(LOG_ERROR, "Failed to allocate Procedure Context \n");

		return ActStatus::HALT;
	}

    mmeStats::Instance()->increment(mmeStatsCounter::MME_PROCEDURES_ATTACH_PROC_IMSI);
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

	ProcedureStats::num_of_attach_req_received ++;
	
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

    MmeDetachProcedureCtxt* prcdCtxt_p = MmeContextManagerUtils::allocateDetachProcedureCtxt(cb, ueInitDetach_c);
    if( prcdCtxt_p == NULL )
    {
	log_msg(LOG_ERROR, "Failed to allocate procedure context for detach cbIndex %d\n", cb.getCBIndex());
	return ActStatus::HALT;
    }

    ueCtxt->setS1apEnbUeId(msgData_p->s1ap_enb_ue_id);
    
    SM::Event evt(DETACH_REQ_FROM_UE, NULL);
    cb.addEventToProcQ(evt);
    
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
    if (msgBuf == NULL)
    {
        log_msg(LOG_INFO, "default_ddn_handler: msgBuf is NULL \n");
        return ActStatus::PROCEED;
    }

    const gtp_incoming_msg_data_t *gtp_msg_data =
            static_cast<const gtp_incoming_msg_data_t*>(msgBuf->getDataPointer());
    const struct ddn_Q_msg &ddn_info =
            gtp_msg_data->msg_data.ddn_Q_msg_m;

    uint8_t gtpCause = GTPV2C_CAUSE_REQUEST_ACCEPTED;
    int sgw_cp_teid = 0;

    UEContext *ueCtxt = static_cast<UEContext*>(cb.getPermDataBlock());
    if (ueCtxt != NULL)
    {
        SessionContext *sess_p = ueCtxt->getSessionContext();
        if (sess_p != NULL)
        {
	    /*We will fetch the s11_sgw_cp_teid from the session context if it's available*/
            sgw_cp_teid = sess_p->getS11SgwCtrlFteid().fteid_m.header.teid_gre;

            MmeSvcReqProcedureCtxt *svcReqProc_p =
                    MmeContextManagerUtils::allocateServiceRequestProcedureCtxt(
                            cb, PagingTrigger::ddnInit_c);
            if (svcReqProc_p != NULL)
            {
                svcReqProc_p->setDdnSeqNo(ddn_info.seq_no);
                svcReqProc_p->setArp(Arp(ddn_info.arp));
                svcReqProc_p->setEpsBearerId(ddn_info.eps_bearer_id);

                SM::Event evt(DDN_FROM_SGW, NULL);
                cb.addEventToProcQ(evt);
            }
            else
            {
                log_msg(LOG_INFO,
                        "default_ddn_handler: Failed to allocate procedure context \n");
                gtpCause = GTPV2C_CAUSE_REQUEST_REJECTED;
            }
        }
        else
        {
            log_msg(LOG_INFO,
                    "default_ddn_handler: Failed to find session context \n");
            gtpCause = GTPV2C_CAUSE_REQUEST_REJECTED;
        }
    }
    else
    {
        log_msg(LOG_INFO,
                "default_ddn_handler: Failed to find UE context \n");

        gtpCause = GTPV2C_CAUSE_CONTEXT_NOT_FOUND;

        MmeContextManagerUtils::deleteUEContext(cb.getCBIndex());
    }

    if (gtpCause != GTPV2C_CAUSE_REQUEST_ACCEPTED)
    {
        struct DDN_ACK_Q_msg ddnAck;
        ddnAck.msg_type = ddn_acknowledgement;
	/*Incase of unavailability of session/UE Contexts , s11_sgw_cp_teid will be set as 0 */
        ddnAck.s11_sgw_c_fteid.header.teid_gre = sgw_cp_teid;
	ddnAck.s11_sgw_c_fteid.ip.ipv4.s_addr = ddn_info.sgw_ip;
        ddnAck.seq_no= ddn_info.seq_no;
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
    log_msg(LOG_DEBUG, "default_service_req_handler \n");

    ProcedureStats::num_of_service_request_received++;

    MsgBuffer *msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
    if (msgBuf == NULL)
    {
        log_msg(LOG_ERROR, "Failed to retrieve message buffer \n");
        return ActStatus::HALT;
    }

    s1_incoming_msg_data_t *msgData_p =
            static_cast<s1_incoming_msg_data_t*>(msgBuf->getDataPointer());
    if (msgData_p == NULL)
    {
        log_msg(LOG_ERROR, "Failed to retrieve data buffer \n");
        return ActStatus::HALT;
    }
    struct service_req_Q_msg &serviceReq = msgData_p->msg_data.service_req_Q_msg_m;

    unsigned char emmCause = 0;

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
                mmCtxt->setEcmState(ecmConnected_c);
                ueCtxt->setS1apEnbUeId(msgData_p->s1ap_enb_ue_id);

                SM::Event evt(SERVICE_REQUEST_FROM_UE, NULL);
                cb.addEventToProcQ(evt);
            }
            else
            {
                log_msg(LOG_ERROR, "Failed to allocate procedure context \n");

                emmCause = emmCause_network_failure;
            }
        }
        else
        {
            log_msg(LOG_ERROR, "Invalid UE Context \n");

            emmCause = emmCause_ue_id_not_derived_by_network;
            MmeContextManagerUtils::deleteUEContext(cb.getCBIndex());
        }
    }
    else
    {
        log_msg(LOG_ERROR, "UE Context is NULL \n");

        emmCause = emmCause_ue_id_not_derived_by_network;
        MmeContextManagerUtils::deleteUEContext(cb.getCBIndex());
    }

    if (emmCause != 0)
    {
        struct commonRej_info serviceRej =
        {
                service_reject,
                msgData_p->ue_idx,
                msgData_p->s1ap_enb_ue_id,
                serviceReq.enb_fd,
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

	MmeDetachProcedureCtxt* prcdCtxt_p =
	        MmeContextManagerUtils::allocateDetachProcedureCtxt(cb, hssInitDetach_c);
	if(prcdCtxt_p == NULL)
	{
		log_msg(LOG_ERROR, "Failed to allocate Procedure Ctxt\n");
		return ActStatus::HALT;
	}
	prcdCtxt_p->setCancellationType(SUBSCRIPTION_WITHDRAWAL);
	prcdCtxt_p->setNasDetachType(reattachNotRequired);

	SM::Event evt(CLR_FROM_HSS, NULL);
	cb.addEventToProcQ(evt);
	
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
        log_msg(LOG_ERROR, "Failed to retrieve message buffer \n");
        return ActStatus::HALT;
    }

    s1_incoming_msg_data_t *msgData_p =
            static_cast<s1_incoming_msg_data_t*>(msgBuf->getDataPointer());
    if (msgData_p == NULL)
    {
        log_msg(LOG_ERROR, "Failed to retrieve data buffer \n");
        return ActStatus::HALT;
    }

	MmeS1RelProcedureCtxt* prcdCtxt_p = SubsDataGroupManager::Instance()->getMmeS1RelProcedureCtxt();
	if( prcdCtxt_p == NULL )
	{
		log_msg(LOG_ERROR, "Failed to allocate procedure Ctxt \n");
		return ActStatus::HALT;
	}
	
    prcdCtxt_p->setS1apEnbUeId(msgData_p->s1ap_enb_ue_id);
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
    log_msg(LOG_DEBUG, "default_tau_req_handler: Entry \n");

    ProcedureStats::num_of_tau_req_received++;

    MsgBuffer *msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
    if (msgBuf == NULL)
    {
        log_msg(LOG_DEBUG, "process_tau_req: msgBuf is NULL \n");
        return ActStatus::HALT;
    }

    const s1_incoming_msg_data_t *msgData_p =
            static_cast<const s1_incoming_msg_data_t*>(msgBuf->getDataPointer());
    if (msgData_p == NULL)
    {
        log_msg(LOG_ERROR, "Failed to retrieve data buffer \n");
        return ActStatus::HALT;
    }

    const struct tauReq_Q_msg &tauReq = (msgData_p->msg_data.tauReq_Q_msg_m);
    unsigned char emmCause = 0;

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

                tauReqProc_p->setS1apEnbUeId(msgData_p->s1ap_enb_ue_id);
                tauReqProc_p->setEnbFd(tauReq.enb_fd);

                //TAI and CGI obtained from s1ap ies.
                //Convert the PLMN in s1ap format to nas format before storing in procedure context.
                MmeCommonUtils::formatS1apPlmnId(const_cast<PLMN*>(&tauReq.tai.plmn_id));
                MmeCommonUtils::formatS1apPlmnId(const_cast<PLMN*>(&tauReq.eUtran_cgi.plmn_id));
                tauReqProc_p->setTai(Tai(tauReq.tai));
                tauReqProc_p->setEUtranCgi(Cgi(tauReq.eUtran_cgi));
                SM::Event evt(TAU_REQUEST_FROM_UE, NULL);
                cb.addEventToProcQ(evt);
            }
            else
            {
                log_msg(LOG_ERROR, "Failed to allocate procedure context \n");

                emmCause = emmCause_network_failure;
            }
        }
        else
        {
            log_msg(LOG_ERROR, "Invalid UE Context \n");

            emmCause = emmCause_ue_id_not_derived_by_network;
            MmeContextManagerUtils::deleteUEContext(cb.getCBIndex());
        }
    }
    else
    {
        log_msg(LOG_ERROR, "UE Context is NULL \n");

        emmCause = emmCause_ue_id_not_derived_by_network;
        MmeContextManagerUtils::deleteUEContext(cb.getCBIndex());
    }

    if (emmCause != 0)
    {
        struct commonRej_info tauRej =
        {
                tau_reject,
		tauReq.ue_m_tmsi,
                msgData_p->s1ap_enb_ue_id,
                tauReq.enb_fd,
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
        log_msg(LOG_DEBUG,"process_handover_required: msgBuf is NULL \n");
        return ActStatus::HALT;
    }

    const s1_incoming_msg_data_t* msgData_p =
            static_cast<const s1_incoming_msg_data_t*>(msgBuf->getDataPointer());
    if (msgData_p == NULL)
    {
        log_msg(LOG_ERROR, "Failed to retrieve data buffer \n");
        return ActStatus::HALT;
    }

	S1HandoverProcedureContext* hoReqProc_p = MmeContextManagerUtils::allocateHoContext(cb);
	if (hoReqProc_p == NULL)
	{
		log_msg(LOG_ERROR, "Failed to allocate procedure context"
				" for ho required cbIndex %d\n", cb.getCBIndex());
		return ActStatus::HALT;
	}

	UEContext *ueCtxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	if (ueCtxt == NULL)
	{
		log_msg(LOG_DEBUG, "ue context is NULL \n");
		return ActStatus::HALT;
	}

	const struct handover_required_Q_msg &hoReq = (msgData_p->msg_data.handover_required_Q_msg_m);

	hoReqProc_p->setS1HoCause(hoReq.cause);
	hoReqProc_p->setTargetEnbContextId(hoReq.target_enb_context_id);
	hoReqProc_p->setSrcToTargetTransContainer(hoReq.srcToTargetTranspContainer);
	hoReqProc_p->setTargetTai(hoReq.target_id.selected_tai);
	hoReqProc_p->setSrcS1apEnbUeId(hoReq.s1ap_enb_ue_id);
	hoReqProc_p->setSrcEnbContextId(hoReq.src_enb_context_id);

	ProcedureStats::num_of_ho_required_received++;

	SM::Event evt(INTRA_S1HO_START, NULL);
	cb.addEventToProcQ(evt);
    return ActStatus::PROCEED;
}

/******************************************************
* Action handler : default_erab_mod_indication_handler
*******************************************************/
ActStatus ActionHandlers::default_erab_mod_indication_handler(ControlBlock& cb)
{
    log_msg(LOG_DEBUG, "default_erab_mod_indication_handler: Entry \n");

    ProcedureStats::num_of_erab_mod_ind_received++;

    UEContext *ueCtxt = static_cast<UEContext*>(cb.getPermDataBlock());
    if (ueCtxt == NULL)
    {
        log_msg(LOG_ERROR, "UE Context is NULL \n");

        MmeContextManagerUtils::deleteUEContext(cb.getCBIndex());
        return ActStatus::HALT;
    }

    MsgBuffer *msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
    if (msgBuf == NULL)
    {
        log_msg(LOG_DEBUG, "process_erab_mod_indication: msgBuf is NULL \n");
        return ActStatus::PROCEED;
    }

    const s1_incoming_msg_data_t *msgData_p =
            static_cast<const s1_incoming_msg_data_t*>(msgBuf->getDataPointer());
    if (msgData_p == NULL)
    {
        log_msg(LOG_ERROR, "Failed to retrieve data buffer \n");
        return ActStatus::PROCEED;
    }

    const struct erab_mod_ind_Q_msg &erabModInd =
            (msgData_p->msg_data.erab_mod_ind_Q_msg_m);

    MmeErabModIndProcedureCtxt *erabModIndProc_p =
            MmeContextManagerUtils::allocateErabModIndProcedureCtxt(cb);
    if (erabModIndProc_p != NULL)
    {
        erabModIndProc_p->setErabToBeModifiedList(
                erabModInd.erab_to_be_mod_list.erab_to_be_mod_item,
                erabModInd.erab_to_be_mod_list.count);

        SM::Event evt(eRAB_MOD_IND_START, NULL);
        cb.addEventToProcQ(evt);
    }
    else
    {
        log_msg(LOG_INFO, "Failed to allocate procedure context \n");
    }

    return ActStatus::PROCEED;
}

