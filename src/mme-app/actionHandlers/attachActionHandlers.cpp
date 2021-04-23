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

#include <3gpp_24008.h>
#include <typeinfo>
#include "actionHandlers/actionHandlers.h"
#include "mme_app.h"
#include "controlBlock.h"
#include "msgType.h"
#include "contextManager/subsDataGroupManager.h"
#include "procedureStats.h"
#include "log.h"
#include "secUtils.h"
#include "state.h"
#include <string.h>
#include <sstream>
#include <mmeSmDefs.h>
#include <cstring>
#include <event.h>
#include <ipcTypes.h>
#include <tipcTypes.h>
#include <msgBuffer.h>
#include <interfaces/mmeIpcInterface.h>
#include <utils/mmeCommonUtils.h>
#include <utils/mmeContextManagerUtils.h>
#include <utils/mmeCauseUtils.h>
#include "mmeNasUtils.h"
#include "mme_app.h"
#include "gtpCauseTypes.h"
#include "mmeStatsPromClient.h"
#include <sstream> 
#include <err_codes.h>

using namespace SM;
using namespace mme;
using namespace cmn;
using namespace cmn::utils;

extern mme_config_t *mme_cfg;
extern mmeConfig *mme_tables;


ActStatus ActionHandlers::validate_imsi_in_ue_context(ControlBlock& cb)
{
    UEContext* ueCtxt_p = static_cast<UEContext*>(cb.getPermDataBlock());
    VERIFY_UE(cb, ueCtxt_p, "Invalid UE");

    if (ueCtxt_p->getImsi().isValid())
    {
        SM::Event evt(IMSI_VALIDATION_SUCCESS, NULL);
        cb.qInternalEvent(evt);
    }
    else
    {
        // TODO: If known GUTI, IMSI_VALIDATION_FAILURE_KNOWN_GUTI to trigger id req to UE
        // If unknown GUTI, IMSI_VALIDATION_FAILURE_UNKNOWN_GUTI to query old mme
        // when s10 is supported in MME
        SM::Event evt(IMSI_VALIDATION_FAILURE, NULL);
        cb.qInternalEvent(evt);
    }
    return ActStatus::PROCEED;
}

ActStatus ActionHandlers::send_identity_request_to_ue(ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside send_identity_request_to_ue ");

	UEContext* ueCtxt_p = static_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ueCtxt_p, "Invalid UE");

	struct attachIdReq_info idReqMsg;
	idReqMsg.msg_type = id_request;
	idReqMsg.enb_fd = ueCtxt_p->getEnbFd();
	idReqMsg.s1ap_enb_ue_id = ueCtxt_p->getS1apEnbUeId();
	idReqMsg.ue_idx = ueCtxt_p->getContextID();
	struct Buffer nasBuffer;
	struct nasPDU nas = {0};
	const uint8_t num_nas_elements = 1;
	nas.elements = (nas_pdu_elements *) calloc(num_nas_elements, sizeof(nas_pdu_elements)); // TODO : should i use new ?
	nas.elements_len = num_nas_elements;
	nas.header.security_header_type = Plain;
	nas.header.proto_discriminator = EPSMobilityManagementMessages;
	nas.header.message_type = IdentityRequest;
	nas.elements[0].pduElement.ue_id_type = ID_IMSI; 
	MmeNasUtils::encode_nas_msg(&nasBuffer, &nas, ueCtxt_p->getUeSecInfo());
	memcpy(&idReqMsg.nasMsgBuf[0], &nasBuffer.buf[0], nasBuffer.pos);
	idReqMsg.nasMsgSize = nasBuffer.pos;
	free(nas.elements);

	cmn::ipc::IpcAddress destAddr;
	destAddr.u32 = TipcServiceInstance::s1apAppInstanceNum_c;
	
    mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_NAS_IDENTITY_REQUEST);
	MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));        
	mmeIpcIf.dispatchIpcMsg((char *) &idReqMsg, sizeof(idReqMsg), destAddr);

    ProcedureStats::num_of_id_req_sent ++;
	
	return ActStatus::PROCEED;
}

ActStatus ActionHandlers::process_identity_response(ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside process_identity_response ");

	UEContext *ueCtxt_p = static_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ueCtxt_p, "Invalid UE");

	MsgBuffer* msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
	VERIFY(msgBuf, return ActStatus::ABORT, "Invalid message")

	const identityResp_Q_msg_t* id_resp = static_cast<const identityResp_Q_msg_t*>(msgBuf->getDataPointer());
	VERIFY(id_resp, return ActStatus::ABORT, "Invalid id response received")

	if(SUCCESS != id_resp->status)
	{
		log_msg(LOG_DEBUG, "process_identity_response: ID Response Failure NULL ");

		return ActStatus::ABORT;
	}

	uint8_t imsi[BINARY_IMSI_LEN] = {0};
	memcpy( imsi, id_resp->IMSI, BINARY_IMSI_LEN );

	// Only upper nibble of first octect in imsi need to be considered
	// Changing the lower nibble to 0x0f for handling
	uint8_t first = imsi[0] >> 4;
	imsi[0] = (uint8_t)(( first << 4 ) | 0x0f );

	DigitRegister15 IMSIInfo;
	IMSIInfo.convertFromBcdArray(imsi);
	ueCtxt_p->setImsi(IMSIInfo);

	SubsDataGroupManager::Instance()->addimsikey(ueCtxt_p->getImsi(), ueCtxt_p->getContextID());
	ProcedureStats::num_of_id_resp_received ++;
	
	return ActStatus::PROCEED;
}


ActStatus ActionHandlers::send_air_to_hss(SM::ControlBlock& cb)
{  
	log_msg(LOG_DEBUG, "Inside send_air_to_hss ");
	
	UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ue_ctxt, "Invalid UE");
	
	MmeProcedureCtxt* procedure_p = dynamic_cast<MmeProcedureCtxt*>(cb.getTempDataBlock());
	VERIFY(procedure_p, return ActStatus::ABORT, "Procedure Context is NULL");

	s6a_Q_msg s6a_req; 
	RESET_S6A_REQ_MSG(&s6a_req);
	 
	ue_ctxt->getImsi().getImsiDigits(s6a_req.imsi);

	memcpy(&(s6a_req.tai), &(ue_ctxt->getTai().tai_m), sizeof(struct TAI));

	s6a_req.ue_idx = ue_ctxt->getContextID();
	s6a_req.msg_type = auth_info_request;
	
	const Auts &auts = procedure_p->getAuts();	
	
	if((procedure_p->getAuthRespStatus() != SUCCESS) && (auts.auts_m.len > 0)){
		memcpy(&(s6a_req.auts), &(auts.auts_m), sizeof(struct AUTS));
	}

	cmn::ipc::IpcAddress destAddr;
	destAddr.u32 = TipcServiceInstance::s6AppInstanceNum_c;

    mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_S6A_AUTHENTICATION_INFO_REQUEST);
	MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));        
	mmeIpcIf.dispatchIpcMsg((char *) &s6a_req, sizeof(s6a_req), destAddr);

	ProcedureStats::num_of_air_sent ++;
	log_msg(LOG_DEBUG, "Leaving send_air_to_hss ");
	
	return ActStatus::PROCEED;

}

ActStatus ActionHandlers::send_ulr_to_hss(SM::ControlBlock& cb)
{  
	log_msg(LOG_DEBUG, "Inside send_ulr_to_hss ");
	
	UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ue_ctxt, "Invalid UE");

	s6a_Q_msg s6a_req;
    RESET_S6A_REQ_MSG(&s6a_req);

	ue_ctxt->getImsi().getImsiDigits(s6a_req.imsi);

	memcpy(&(s6a_req.tai), &(ue_ctxt->getTai().tai_m), sizeof(struct TAI));

	s6a_req.ue_idx = ue_ctxt->getContextID();
	s6a_req.msg_type = update_loc_request;
    
	// Check and populate feature lists belonging to id 2
	uint32_t featList = 0;
        
	if (mme_cfg->feature_list.dcnr_support)
        featList |= nrAsSecRatBitMask_c;
	
	if (featList != 0)
	{
	    s6a_req.supp_features_list.supp_features[s6a_req.supp_features_list.count].feature_list_id = 2;
	    s6a_req.supp_features_list.supp_features[s6a_req.supp_features_list.count].feature_list =  featList;
	    s6a_req.supp_features_list.count++;
	}

	cmn::ipc::IpcAddress destAddr;
	destAddr.u32 = TipcServiceInstance::s6AppInstanceNum_c;
	
    mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_S6A_UPDATE_LOCATION_REQUEST);
	MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));   
	mmeIpcIf.dispatchIpcMsg((char *) &s6a_req, sizeof(s6a_req), destAddr);

	ProcedureStats::num_of_ulr_sent ++;
	log_msg(LOG_DEBUG, "Leaving send_ulr_to_hss ");
	
	return ActStatus::PROCEED;
}


ActStatus ActionHandlers::process_aia(SM::ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside handle_aia ");

	UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ue_ctxt, "Invalid UE");
	
	MmeProcedureCtxt *procedure_p = dynamic_cast<MmeProcedureCtxt*>(cb.getTempDataBlock());
	VERIFY(procedure_p, return ActStatus::ABORT, "Procedure Context is NULL");

	MsgBuffer* msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
	VERIFY(msgBuf, return ActStatus::ABORT, "Invalid message buffer ");

	const aia_Q_msg_t* msgData_p = static_cast<const aia_Q_msg_t *>(msgBuf->getDataPointer());

	if (msgData_p->res == S6A_AIA_FAILED)
	{	
		/* send attach reject and release UE */
        	log_msg(LOG_INFO, "AIA failed. UE %d", ue_ctxt->getContextID());
        	procedure_p->setMmeErrorCause(S6A_AIA_FAILED);
        	return ActStatus::ABORT;
	}

	ue_ctxt->setAiaSecInfo(E_utran_sec_vector(msgData_p->sec));
	
	ProcedureStats::num_of_processed_aia ++;
	log_msg(LOG_DEBUG, "Leaving handle_aia ");
	
	return ActStatus::PROCEED;
}

ActStatus ActionHandlers::process_ula(SM::ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside handle_ula ");

	UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ue_ctxt, "Invalid UE");

	auto& sessionCtxtContainer = ue_ctxt->getSessionContextContainer();
	VERIFY(sessionCtxtContainer.size() > 0, return ActStatus::ABORT, "SessionContainer is empty ");

	SessionContext* sessionCtxt = sessionCtxtContainer.front();
	VERIFY(sessionCtxt, return ActStatus::ABORT, "Session Context is NULL ");
	
	MsgBuffer* msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
	VERIFY(msgBuf, return ActStatus::ABORT, "Invalid message buffer ")

	const ula_Q_msg_t *ula_msg = static_cast<const ula_Q_msg_t*>(msgBuf->getDataPointer());

	sessionCtxt->setApnConfigProfileCtxId(ula_msg->apn_config_profile_ctx_id);
	DigitRegister15 ueMSISDN;
	ueMSISDN.convertFromBcdArray( reinterpret_cast<const uint8_t*>( ula_msg->MSISDN ));
	ue_ctxt->setMsisdn(ueMSISDN);
	ue_ctxt->setRauTauTimer(ula_msg->RAU_TAU_timer);
	ue_ctxt->setSubscriptionStatus(ula_msg->subscription_status);
	ue_ctxt->setNetAccessMode(ula_msg->net_access_mode);
	ue_ctxt->setAccessRestrictionData(ula_msg->access_restriction_data);
	ue_ctxt->setSubscribedApn(Apn_name(ula_msg->selected_apn));

    for (int i = 0; i < ula_msg->supp_features_list.count; i++)
    {
        if (ula_msg->supp_features_list.supp_features[i].feature_list_id == 2)
        {
            log_msg(LOG_DEBUG,"received feature_list_id2 from hss %u %u ",ula_msg->supp_features_list.supp_features[i].feature_list_id, ula_msg->supp_features_list.supp_features[i].feature_list);
            ue_ctxt->setHssFeatList2(
                    ula_msg->supp_features_list.supp_features[i]);
            break;
        }
    }
        
	ue_ctxt->setDcnrCapable(MmeCommonUtils::isUeNRCapable(*ue_ctxt));
	
	// UE-AMBR received from HSS. Treated as APN-AMBR as well.
	// Bitrate values beyond 4.2 Gbps will be set in extended AMBR fields.
	// Extended AMBR fields can store bit rate values upto 4.2 Tbps.
	struct AMBR ambr;
	ambr.max_requested_bw_dl = ula_msg->max_requested_bw_dl;
	ambr.max_requested_bw_ul = ula_msg->max_requested_bw_ul;
	if(ula_msg->extended_max_requested_bw_dl > 0 || ula_msg->extended_max_requested_bw_ul > 0)
	{
	    ambr.ext_max_requested_bw_dl = ula_msg->extended_max_requested_bw_dl;
	    ambr.ext_max_requested_bw_ul = ula_msg->extended_max_requested_bw_ul;
	}

	ue_ctxt->setAmbr(Ambr(ambr));
	
	struct PAA pdn_addr;
	pdn_addr.pdn_type = 1;
	pdn_addr.ip_type.ipv4.s_addr = ntohl(ula_msg->static_addr); // network byte order
	
	ue_ctxt->setPdnAddr(Paa(pdn_addr));
	
	ProcedureStats::num_of_processed_ula ++;
	log_msg(LOG_DEBUG, "Leaving handle_ula_v ");
	
	return ActStatus::PROCEED;
}

ActStatus ActionHandlers::auth_req_to_ue(SM::ControlBlock& cb)
{
	UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ue_ctxt, "Invalid UE");

	authreq_info authreq;
	authreq.msg_type = auth_request;
	authreq.ue_idx = ue_ctxt->getContextID();
	authreq.enb_fd = ue_ctxt->getEnbFd();
	authreq.enb_s1ap_ue_id = ue_ctxt->getS1apEnbUeId();

	ue_ctxt->getUeSecInfo().resetUplinkCount();
	ue_ctxt->getUeSecInfo().resetDownlinkCount();

	E_UTRAN_sec_vector *secVect = const_cast<E_UTRAN_sec_vector*>(ue_ctxt->getAiaSecInfo().AiaSecInfo_mp);

	struct Buffer nasBuffer;
	struct nasPDU nas = {0};
	const uint8_t num_nas_elements = 2;
	nas.elements = (nas_pdu_elements *) calloc(num_nas_elements, sizeof(nas_pdu_elements)); // TODO : should i use new ?
	nas.elements_len = num_nas_elements;
	nas.header.message_type = AuthenticationRequest;
	nas.header.proto_discriminator = EPSMobilityManagementMessages;
	nas.header.nas_security_param = AUTHREQ_NAS_SECURITY_PARAM;
	memcpy(&(nas.elements[0].pduElement.rand[0]), &(secVect->rand.val[0]), NAS_RAND_SIZE);
	memcpy(&(nas.elements[1].pduElement.autn[0]), &(secVect->autn.val[0]), NAS_AUTN_SIZE);
	MmeNasUtils::encode_nas_msg(&nasBuffer, &nas, ue_ctxt->getUeSecInfo());
	memcpy(&authreq.nasMsgBuf[0], &nasBuffer.buf[0], nasBuffer.pos);
	authreq.nasMsgSize = nasBuffer.pos;
	free(nas.elements);

	cmn::ipc::IpcAddress destAddr;
	destAddr.u32 = TipcServiceInstance::s1apAppInstanceNum_c;
	
    mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_NAS_AUTHENTICATION_REQUEST);
	MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));   
	mmeIpcIf.dispatchIpcMsg((char *) &authreq, sizeof(authreq), destAddr);
	
	ProcedureStats::num_of_auth_req_to_ue_sent ++;
	log_msg(LOG_DEBUG, "Leaving auth_req_to_ue_v sent message of length %d",sizeof(authreq));
		
	return ActStatus::PROCEED;
}

ActStatus ActionHandlers::auth_response_validate(SM::ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside auth_response_validate ");
	SM::ControlBlock* controlBlk_p = SubsDataGroupManager::Instance()->findControlBlock(cb.getCBIndex());

	UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ue_ctxt, "Invalid UE");
	
	MmeProcedureCtxt* procedure_p = dynamic_cast<MmeProcedureCtxt*>(cb.getTempDataBlock());
	VERIFY(procedure_p, return ActStatus::ABORT, "Procedure Context is NULL ");
		
	MsgBuffer* msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
	VERIFY(msgBuf, return ActStatus::ABORT, "Invalid message buffer ");

	const authresp_Q_msg_t *auth_resp = static_cast<const authresp_Q_msg_t*>(msgBuf->getDataPointer());	
	
	/*Check the state*/
	if(SUCCESS != auth_resp->status) {
		log_msg(LOG_ERROR, "eNB authentication failure for UE-%d.", ue_ctxt->getContextID());
		if(auth_resp->auts.len == 0)
		{
			log_msg(LOG_ERROR,"No AUTS.Not Synch Failure");
			procedure_p->setMmeErrorCause(S1AP_AUTH_FAILED);
			SM::Event evt(AUTH_RESP_FAILURE,NULL);
        		controlBlk_p->qInternalEvent(evt);
		}
		else
		{
			log_msg(LOG_INFO,"AUTS recvd.  Synch failure. send AIR");
			procedure_p->setAuthRespStatus(auth_resp->status);
			procedure_p->setAuts(Auts(auth_resp->auts));
			SM::Event evt(AUTH_RESP_SYNC_FAILURE,NULL);
            		controlBlk_p->qInternalEvent(evt);
		}
	}
    else
    {
        log_msg(LOG_INFO,
                "Auth response validation success. Proceeding to Sec mode Command");
        uint64_t xres = 0;
        memcpy(&xres, ue_ctxt->getAiaSecInfo().AiaSecInfo_mp->xres.val,
                sizeof(uint64_t));
        uint64_t res = 0;
        memcpy(&res, auth_resp->res.val, sizeof(uint64_t));
        log_msg(LOG_DEBUG, "Auth response Comparing received result from UE " 
                " (%lu) with xres (%lu). Length %d", res, 
                xres, auth_resp->res.len);

        if(memcmp((ue_ctxt->getAiaSecInfo().AiaSecInfo_mp->xres.val),
                  (auth_resp->res.val),
                  auth_resp->res.len) != 0) {
            log_msg(LOG_ERROR, "Invalid Auth response Comparing received result "
                    "from UE (%lu) with xres (%lu). Length %d", 
                    res, xres, auth_resp->res.len);
            procedure_p->setMmeErrorCause(MME_AUTH_VALIDATION_FAILURE);
            return ActStatus::ABORT;
        }

        ProcedureStats::num_of_processed_auth_response++;
        SM::Event evt(AUTH_RESP_SUCCESS, NULL);
        controlBlk_p->qInternalEvent(evt);

    }
	
	log_msg(LOG_DEBUG, "Leaving auth_response_validate ");
	
	return ActStatus::PROCEED;
}

ActStatus ActionHandlers::send_auth_reject(SM::ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside send_auth_reject ");

	UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ue_ctxt, "Invalid UE");
	
	ProcedureStats::num_of_auth_reject_sent ++;
	return ActStatus::PROCEED;
}

ActStatus ActionHandlers::sec_mode_cmd_to_ue(SM::ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside sec_mode_cmd_to_ue ");

	UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ue_ctxt, "Invalid UE");
	
    E_UTRAN_sec_vector *secVect = const_cast<E_UTRAN_sec_vector*>(ue_ctxt->getAiaSecInfo().AiaSecInfo_mp);
	secinfo& secInfo = const_cast<secinfo&>(ue_ctxt->getUeSecInfo().secinfo_m);
	sec_mode_Q_msg sec_mode_msg;
	sec_mode_msg.msg_type  = sec_mode_command;
	sec_mode_msg.ue_idx = ue_ctxt->getContextID();
	sec_mode_msg.enb_fd = ue_ctxt->getEnbFd();
	sec_mode_msg.enb_s1ap_ue_id = ue_ctxt->getS1apEnbUeId();

    MmeNasUtils::select_sec_alg(ue_ctxt);
	SecUtils::create_integrity_key(ue_ctxt->getUeSecInfo().getSelectIntAlg(), 
                                   secVect->kasme.val, secInfo.int_key);
	
	struct Buffer nasBuffer;
	struct nasPDU nas = {0};
	nas.header.message_type = SecurityModeCommand;
	nas.header.security_header_type = IntegrityProtectedEPSSecCntxt;
	nas.header.proto_discriminator = EPSMobilityManagementMessages;
	uint8_t mac[MAC_SIZE] = {0};
	memcpy(nas.header.mac, mac, MAC_SIZE);

	nas.header.seq_no = ue_ctxt->getUeSecInfo().getDownlinkSeqNo(); 
	nas.dl_count = ue_ctxt->getUeSecInfo().getDownlinkCount();	
	ue_ctxt->getUeSecInfo().increment_downlink_count();

	nas.header.security_encryption_algo = ue_ctxt->getUeSecInfo().getSelectSecAlg();
	nas.header.security_integrity_algo = ue_ctxt->getUeSecInfo().getSelectIntAlg();
	nas.header.nas_security_param = AUTHREQ_NAS_SECURITY_PARAM;
        uint8_t num_nas_elements = SEC_MODE_NO_OF_NAS_IES;
	/* UE additional security capability is replayed in sec_mode_command when MME supports the handling of the same
         * Spec 24.301 v 15.6.0  sec:8.2.20.6*/
	if(mme_cfg->feature_list.dcnr_support && ue_ctxt->getUeAddSecCapabPres())
    	{
            num_nas_elements += 1;
    	}
	nas.elements = (nas_pdu_elements *) calloc(num_nas_elements, sizeof(nas_pdu_elements)); // TODO : should i use new ?
	nas.elements_len = num_nas_elements;
	nas.elements->pduElement.ue_network.len = ue_ctxt->getUeNetCapab().ue_net_capab_m.len;
	if(ue_ctxt->getUeNetCapab().ue_net_capab_m.len >= 4)
	{
        /*Copy first 4 bytes of security algo info*/
	    memcpy(nas.elements->pduElement.ue_network.u.octets, ue_ctxt->getUeNetCapab().ue_net_capab_m.u.octets, 4);
	   
        if(ue_ctxt->getMsNetCapab().ms_net_capab_m.pres == true)
	    {
	        /*The MS Network capability contains the GEA
		* capability. The MSB of 1st Byte and the 2nd to
		* 7th Bit of 2nd byte contain the GEA info.
		* Thus the masks 0x7F : for GEA/1
		* and mask 0x7D: for GEA2 -GEA7
		*/
            log_msg(LOG_DEBUG, "MS network present"); 
	        nas.elements->pduElement.ue_network.len = 5;
	    	unsigned char val = 0x00;
		    val = ue_ctxt->getMsNetCapab().ms_net_capab_m.capab[0]&0x80;
            val |= ue_ctxt->getMsNetCapab().ms_net_capab_m.capab[1]&0x7E;
            val >>= 1;
	        nas.elements->pduElement.ue_network.u.octets[4] = val;
	    }
	    else
	    {
	        /*If MS capability is not present. Then only 
		* Capability till UMTS Algorithms is sent.*/
            log_msg(LOG_DEBUG, "MS network not present"); 
	        nas.elements->pduElement.ue_network.len = 4;
	    }
	}
	else
	{
	    /*Copy as much info of UE network capability 
	    * as received.
	    */
            memcpy(nas.elements->pduElement.ue_network.u.octets,
				   	ue_ctxt->getUeNetCapab().ue_net_capab_m.u.octets,
					ue_ctxt->getUeNetCapab().ue_net_capab_m.len);
	}

	if (mme_cfg->feature_list.dcnr_support && ue_ctxt->getUeAddSecCapabPres())
    {
        nas.opt_ies_flags.ue_add_sec_cap_presence = true;
        memcpy(&(nas.elements->pduElement.ue_add_sec_capab),
                &(ue_ctxt->getUeAddSecCapab()),
                sizeof(ue_add_sec_capabilities));
    }
    
	MmeNasUtils::encode_nas_msg(&nasBuffer, &nas, ue_ctxt->getUeSecInfo());
	memcpy(&sec_mode_msg.nasMsgBuf[0], &nasBuffer.buf[0], nasBuffer.pos);
	sec_mode_msg.nasMsgSize = nasBuffer.pos;
	free(nas.elements); 

	cmn::ipc::IpcAddress destAddr;
	destAddr.u32 = TipcServiceInstance::s1apAppInstanceNum_c;
	
    mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_NAS_SECURITY_MODE_COMMAND);
	MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));   
	mmeIpcIf.dispatchIpcMsg((char *) &sec_mode_msg, sizeof(sec_mode_msg), destAddr);
	
	ProcedureStats::num_of_sec_mode_cmd_to_ue_sent ++;
	log_msg(LOG_DEBUG, "Leaving sec_mode_cmd_to_ue ");
	
	return ActStatus::PROCEED;
}


ActStatus ActionHandlers::process_sec_mode_resp(SM::ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside handle_sec_mode_resp ");

	UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ue_ctxt, "Invalid UE");

	MmeProcedureCtxt* procedure_p = dynamic_cast<MmeProcedureCtxt*>(cb.getTempDataBlock());
	VERIFY(procedure_p, return ActStatus::ABORT, "Procedure Context is NULL ");

	MsgBuffer* msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
	VERIFY(msgBuf, return ActStatus::ABORT, "Invalid message buffer ")

	const secmode_resp_Q_msg_t* secmode_resp = static_cast<const secmode_resp_Q_msg_t*>(msgBuf->getDataPointer());
	if(SUCCESS == secmode_resp->status)
	{
		log_msg(LOG_INFO, "Sec mode complete rcv. UE - %d.",
				ue_ctxt->getContextID());
		
	}	
	else
	{
		log_msg(LOG_ERROR, "Sec mode failed. UE %d", ue_ctxt->getContextID());
		procedure_p->setMmeErrorCause((ERROR_CODES)secmode_resp->status);
		return ActStatus::ABORT;
	}

	ProcedureStats::num_of_processed_sec_mode_resp ++;
	log_msg(LOG_DEBUG, "Leaving handle_sec_mode_resp ");
	
	return ActStatus::PROCEED;
}

ActStatus ActionHandlers::check_esm_info_req_required(SM::ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside check_esm_info_req_required ");

	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ue_ctxt, "Invalid UE");
	
	MmeAttachProcedureCtxt* procedure_p = dynamic_cast<MmeAttachProcedureCtxt*>(cb.getTempDataBlock());
	VERIFY(procedure_p, return ActStatus::ABORT, "Procedure Context is NULL ");
	
	MmeContextManagerUtils::deleteAllSessionContext(cb);

	SessionContext* sessionCtxt = MmeContextManagerUtils::allocateSessionContext(cb, *ue_ctxt);
	VERIFY(sessionCtxt, return ActStatus::ABORT, "Session Context is NULL ");

	sessionCtxt->setPti(procedure_p->getPti());
	
	if (procedure_p->getEsmInfoTxRequired() == false)
	{
		SM::Event evt(ESM_INFO_NOT_REQUIRED, NULL);
		cb.qInternalEvent(evt);
	} 
	else
	{
		SM::Event evt(ESM_INFO_REQUIRED, NULL);
		cb.qInternalEvent(evt);
	}
	
	return  ActStatus::PROCEED;
}

ActStatus ActionHandlers::send_esm_info_req_to_ue(SM::ControlBlock& cb)
{
	UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ue_ctxt, "Invalid UE");

	auto& sessionCtxtContainer = ue_ctxt->getSessionContextContainer();
	if(sessionCtxtContainer.size() < 1)
	{
		log_msg(LOG_ERROR, "Session context list is empty for UE IDX %d", cb.getCBIndex());
		return ActStatus::HALT;
	}
	log_msg(LOG_DEBUG,"Sending ESM info request UE index %u ", ue_ctxt->getContextID());

	SessionContext* sessionCtxt = sessionCtxtContainer.front();
	esm_req_Q_msg esmreq;
	esmreq.msg_type = esm_info_request;
	esmreq.ue_idx = ue_ctxt->getContextID();
	esmreq.enb_fd = ue_ctxt->getEnbFd();
	esmreq.enb_s1ap_ue_id = ue_ctxt->getS1apEnbUeId();
	struct Buffer nasBuffer;
	struct nasPDU nas = {0};
	uint8_t mac[MAC_SIZE] = {0};
	const uint8_t num_nas_elements = 2;
	nas.elements = (nas_pdu_elements *) calloc(num_nas_elements, sizeof(nas_pdu_elements)); // TODO : should i use new ?
	nas.elements_len = num_nas_elements;
	nas.header.message_type = ESMInformationRequest;
	nas.header.proto_discriminator = EPSMobilityManagementMessages;
	memcpy(nas.header.mac, mac, MAC_SIZE);
	nas.header.security_header_type = IntegrityProtectedCiphered;
	nas.header.nas_security_param = AUTHREQ_NAS_SECURITY_PARAM;
	nas.header.seq_no = ue_ctxt->getUeSecInfo().getDownlinkSeqNo(); 
	nas.dl_count = ue_ctxt->getUeSecInfo().getDownlinkCount();	
	ue_ctxt->getUeSecInfo().increment_downlink_count();
	nas.header.eps_bearer_identity = 0;
	nas.header.procedure_trans_identity = sessionCtxt->getPti();
	MmeNasUtils::encode_nas_msg(&nasBuffer, &nas, ue_ctxt->getUeSecInfo());
	memcpy(&esmreq.nasMsgBuf[0], &nasBuffer.buf[0], nasBuffer.pos);
	esmreq.nasMsgSize = nasBuffer.pos;
	free(nas.elements); 

	cmn::ipc::IpcAddress destAddr;
	destAddr.u32 = TipcServiceInstance::s1apAppInstanceNum_c;

    mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_NAS_ESM_INFORMATION_REQUEST);
	MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));   
	mmeIpcIf.dispatchIpcMsg((char *) &esmreq, sizeof(esmreq), destAddr);

	log_msg(LOG_DEBUG, "Leaving send_esm_info_req_to_ue ");
	ProcedureStats::num_of_esm_info_req_to_ue_sent ++;
	return ActStatus::PROCEED;
}

ActStatus ActionHandlers::process_esm_info_resp(SM::ControlBlock& cb)
{
	UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ue_ctxt, "Invalid UE");

    	MmeAttachProcedureCtxt* procedure_p = dynamic_cast<MmeAttachProcedureCtxt*>(cb.getTempDataBlock());
    	VERIFY(procedure_p, return ActStatus::ABORT, "Procedure Context is NULL ");

	MsgBuffer* msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
	VERIFY(msgBuf, return ActStatus::ABORT, "Invalid message buffer ")

	const esm_resp_Q_msg_t *esm_res = static_cast<const esm_resp_Q_msg_t *>(msgBuf->getDataPointer());
    	if (esm_res->status != SUCCESS)
    	{
        	log_msg(LOG_ERROR, "ESM Response failed ");
    	}

	procedure_p->setRequestedApn(Apn_name(esm_res->apn));

	ProcedureStats::num_of_handled_esm_info_resp++;
	return ActStatus::PROCEED;
}

ActStatus ActionHandlers::cs_req_to_sgw(SM::ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside cs_req_to_sgw ");

	UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ue_ctxt, "Invalid UE");

	MmeAttachProcedureCtxt *procCtxt = dynamic_cast<MmeAttachProcedureCtxt*>(cb.getTempDataBlock());
	VERIFY(procCtxt, return ActStatus::ABORT, "Procedure Context is NULL ");

	auto& sessionCtxtContainer = ue_ctxt->getSessionContextContainer();
	VERIFY(sessionCtxtContainer.size() > 0, return ActStatus::ABORT, "Sessions Container is empty");

	SessionContext* sessionCtxt = sessionCtxtContainer.front();
	VERIFY(sessionCtxt, return ActStatus::ABORT, "Session Context is NULL ");

	BearerContext* bearerCtxt_p = sessionCtxt->findBearerContextByBearerId(sessionCtxt->getLinkedBearerId());
	VERIFY(bearerCtxt_p, return ActStatus::ABORT, "Bearer Context is NULL ");

	struct CS_Q_msg cs_msg;
    memset(&cs_msg, 0, sizeof(cs_msg));
	cs_msg.msg_type = create_session_request;
	cs_msg.ue_idx = ue_ctxt->getContextID();
	
	const DigitRegister15& ueImsi = ue_ctxt->getImsi();
	ueImsi.convertToBcdArray( cs_msg.IMSI );
	
	/*uint8_t  plmn_id[3] = {0};
	memcpy(plmn_id, ue_ctxt->gettai().tai_m.plmn_id.idx, 3);
	if ((plmn_id[1] & 0xF0) == 0xF0)
		plmn_id[1] = plmn_id[1] & 0x0F;

	const Apn_name &apnName = sessionCtxt->getaccessPtName();
	std::string apnStr((const char *)apnName.apnname_m.val, apnName.apnname_m.len);

	stringstream formattedApn;
	formattedApn << apnStr  <<
					 "\x6" << "mnc" <<
					((plmn_id[1] & 0xF0) >> 4) <<
					(plmn_id[2] & 0x0F) <<
					((plmn_id[2] & 0xF0) >> 4) <<
					"\x6" << "mcc" <<
					(plmn_id[0] & 0x0F) <<
					((plmn_id[0] & 0xF0) >> 4) <<
					((plmn_id[1] & 0x0F)) <<
					"\x4" << "gprs";

	uint32_t formattedApnLen = formattedApn.str().length();
	cs_msg.apn.len = formattedApnLen + 1;
	cs_msg.apn.val[0] = apnStr.length();
	memcpy(&cs_msg.apn.val[1], formattedApn.str().c_str(),
	formattedApn.str().length()); */

	const Apn_name &apnName = ue_ctxt->getSubscribedApn();
    log_msg(LOG_DEBUG, "apn = %s length = %d ", apnName.apnname_m.val, apnName.apnname_m.len);
	// TODO: ApnSelection
	// Set the subscribed apn to selected apn for now
	sessionCtxt->setAccessPtName(apnName);
	memcpy(&(cs_msg.selected_apn), &(apnName.apnname_m), sizeof(struct apn_name));
    bool local_mapping = true;
    if(local_mapping)
    {
        //unsigned char temp_name[128];
        //strcpy(temp_name, apnName.apnname_m.val);
        log_msg(LOG_DEBUG, "APN %s length = %d ",apnName.apnname_m.val,apnName.apnname_m.len);
        const unsigned char *ptr = &apnName.apnname_m.val[1]; /*BUG*/
        std::string temp_str((char *)ptr);
        apn_config *temp = mme_tables->find_apn(temp_str); 
        if(temp != NULL)
        {
            log_msg(LOG_DEBUG, "Found APN mapping in static table %x ",cs_msg.sgw_ip);
            cs_msg.sgw_ip = temp->get_sgw_addr(); 
            cs_msg.pgw_ip = temp->get_pgw_addr(); 
        } 
        else
        {
            log_msg(LOG_DEBUG, "APN not found in static apn configuration ");
        }
    }
	memcpy(&(cs_msg.tai), &(ue_ctxt->getTai().tai_m), sizeof(struct TAI));
	memcpy(&(cs_msg.utran_cgi), &(ue_ctxt->getUtranCgi().cgi_m), sizeof(struct CGI));
	cs_msg.pco_length = procCtxt->getPcoOptionsLen();
	if(procCtxt->getPcoOptionsLen() > 0){
		memcpy(&(cs_msg.pco_options[0]), procCtxt->getPcoOptions(),cs_msg.pco_length);
	}

	const AMBR& ambr = ue_ctxt->getAmbr().ambr_m;

	// Currently we are treating UE-AMBR as APN-AMBR.
	// Is this correct?
	sessionCtxt->setApnAmbr(Ambr(ambr));

	// AMBR in UEContext/SessionContext is value received from HSS and is in bps.
	// SGW expects it in Kbps
	cs_msg.max_requested_bw_dl = ambr.max_requested_bw_dl/1000;
	cs_msg.max_requested_bw_ul = ambr.max_requested_bw_ul/1000;
	
	// If the AMBR received from HSS is beyond 4.2 Gbps, set the AMBR from the extended AMBR fields.
	if (ambr.ext_max_requested_bw_dl > 0)
	    cs_msg.max_requested_bw_dl = ambr.ext_max_requested_bw_dl; // already in kbps
	if (ambr.ext_max_requested_bw_ul > 0)
	    cs_msg.max_requested_bw_ul = ambr.ext_max_requested_bw_ul; // already in kbps

	const PAA& pdn_addr = ue_ctxt->getPdnAddr().paa_m;
	
	cs_msg.paa_v4_addr = pdn_addr.ip_type.ipv4.s_addr; /* host order */

	memset(cs_msg.MSISDN, 0, BINARY_IMSI_LEN);
	
	const DigitRegister15& ueMSISDN = ue_ctxt->getMsisdn();
	ueMSISDN.convertToBcdArray(cs_msg.MSISDN);
	
	if(ue_ctxt->getDcnrCapable())
	   cs_msg.dcnr_flag = true;

	cmn::ipc::IpcAddress destAddr;
	destAddr.u32 = TipcServiceInstance::s11AppInstanceNum_c;

    mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_S11_CREATE_SESSION_REQUEST);
	MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));   
	mmeIpcIf.dispatchIpcMsg((char *) &cs_msg, sizeof(cs_msg), destAddr);

	ProcedureStats::num_of_cs_req_to_sgw_sent ++;
	log_msg(LOG_DEBUG, "Leaving cs_req_to_sgw ");

    	return ActStatus::PROCEED;
}

ActStatus ActionHandlers::process_cs_resp(SM::ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Entering handle_cs_resp ");

	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ue_ctxt, "Invalid UE");
	
	MmeAttachProcedureCtxt* procedure_p = dynamic_cast<MmeAttachProcedureCtxt*>(cb.getTempDataBlock());
	VERIFY(procedure_p, return ActStatus::ABORT, "Procedure Context is NULL ");

	auto& sessionCtxtContainer = ue_ctxt->getSessionContextContainer();
	VERIFY(sessionCtxtContainer.size() > 0, return ActStatus::ABORT, "Sessions Container is empty");

	SessionContext* sessionCtxt = sessionCtxtContainer.front();
	VERIFY(sessionCtxt, return ActStatus::ABORT, "Session Context is NULL ");

	MsgBuffer* msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
	VERIFY(msgBuf, return ActStatus::ABORT, "Invalid message buffer ");
	VERIFY(msgBuf->getLength() >= sizeof(struct csr_Q_msg), return ActStatus::ABORT, "Invalid CSRsp message length ");

	const struct csr_Q_msg* csr_info= static_cast<const struct csr_Q_msg*>(msgBuf->getDataPointer());

    if(csr_info->status != GTPV2C_CAUSE_REQUEST_ACCEPTED)
    {
		log_msg(LOG_DEBUG, "CSRsp rejected by SGW with cause %d ",csr_info->status);
        std::ostringstream reason;
        reason<<"CSRsp_reject_cause_"<<csr_info->status;
        mmeStats::Instance()->increment(mmeStatsCounter::MME_PROCEDURES_ATTACH_PROC_FAILURE, {{"failure_reason", reason.str()}});
       	return ActStatus::ABORT;
    }

	BearerContext* bearerCtxt = sessionCtxt->findBearerContextByBearerId(sessionCtxt->getLinkedBearerId());
	VERIFY(bearerCtxt, return ActStatus::ABORT, "Bearer Context is NULL ");

	bearerCtxt->setBearerQos(csr_info->bearerQos);
	procedure_p->setPcoOptions(csr_info->pco_options,csr_info->pco_length);
	
	sessionCtxt->setS11SgwCtrlFteid(Fteid(csr_info->s11_sgw_fteid));
	sessionCtxt->setS5S8PgwCtrlFteid(Fteid(csr_info->s5s8_pgwc_fteid));

	bearerCtxt->setS1uSgwUserFteid(Fteid(csr_info->s1u_sgw_fteid));
	bearerCtxt->setS5S8PgwUserFteid(Fteid(csr_info->s5s8_pgwu_fteid));

	sessionCtxt->setPdnAddr(Paa(csr_info->pdn_addr));
		
	ProcedureStats::num_of_processed_cs_resp ++;
	log_msg(LOG_DEBUG, "Leaving handle_cs_resp ");
	
	return ActStatus::PROCEED;
}

ActStatus ActionHandlers::send_init_ctxt_req_to_ue(SM::ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside send_init_ctxt_req_to_ue ");

	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ue_ctxt, "Invalid UE");

	MmeAttachProcedureCtxt* procedure_p = dynamic_cast<MmeAttachProcedureCtxt*>(cb.getTempDataBlock());
	VERIFY(procedure_p, return ActStatus::ABORT, "Procedure Context is NULL ");

	if (procedure_p->getAttachType() == imsiAttach_c ||
			procedure_p->getAttachType() == unknownGutiAttach_c)
	{
		uint32_t mTmsi = MmeCommonUtils::allocateMtmsi();
		if (mTmsi == 0)
		{
			log_msg(LOG_DEBUG, "send_init_ctxt_req_to_ue: Failed to allocate mTmsi ");
			return ActStatus::ABORT;
		}

		ue_ctxt->setMTmsi(mTmsi);

		// TODO: Should this be done here or attach_done method
		SubsDataGroupManager::Instance()->addmTmsikey(mTmsi, ue_ctxt->getContextID());
	}

	auto& sessionCtxtContainer = ue_ctxt->getSessionContextContainer();
	VERIFY(sessionCtxtContainer.size() > 0, return ActStatus::ABORT, "Sessions Container is empty");

	SessionContext* sessionCtxt = sessionCtxtContainer.front();
	VERIFY(sessionCtxt, return ActStatus::ABORT, "Session Context is NULL ");

	/* 33.401 7.2.6.2	Establishment of keys for cryptographically protected 
       radio bearers
       Only in case of AKA procedures having run. the nas_count(Uplink) is used
       as 0. In case when no AKA has run, the nas_count should be used from 
       current security context. : This is not done yet. But should be added.
     */
    unsigned int nas_count = 0;
	E_UTRAN_sec_vector* secVect = const_cast<E_UTRAN_sec_vector*>(ue_ctxt->getAiaSecInfo().AiaSecInfo_mp);
	secinfo& secInfo = const_cast<secinfo&>(ue_ctxt->getUeSecInfo().secinfo_m);

    SecUtils::create_kenb_key(secVect->kasme.val, secInfo.kenb_key, nas_count);
    unsigned char nh[SECURITY_KEY_SIZE] = { 0 };
    secInfo.next_hop_chaining_count = 1;
    SecUtils::create_nh_key(secVect->kasme.val, nh, secInfo.kenb_key);
    memcpy(secInfo.next_hop_nh , nh, KENB_SIZE);

    init_ctx_req_Q_msg icr_msg;
	memset(&icr_msg, 0, sizeof(init_ctx_req_Q_msg));

	bool eps_nw_feature_supp_presence = false;
	icr_msg.msg_type = init_ctxt_request;
	icr_msg.ue_idx = ue_ctxt->getContextID();
	icr_msg.enb_fd = ue_ctxt->getEnbFd();
	icr_msg.enb_s1ap_ue_id = ue_ctxt->getS1apEnbUeId();

	icr_msg.exg_max_dl_bitrate = (ue_ctxt->getAmbr().ambr_m).max_requested_bw_dl;
	icr_msg.exg_max_ul_bitrate = (ue_ctxt->getAmbr().ambr_m).max_requested_bw_ul;

	BearerContext* bearerCtxt = sessionCtxt->findBearerContextByBearerId(sessionCtxt->getLinkedBearerId());
	VERIFY(bearerCtxt, return ActStatus::ABORT, "Bearer Context is NULL ");

	icr_msg.bearer_id = bearerCtxt->getBearerId();
	icr_msg.qci = bearerCtxt->getBearerQos().qci;
	icr_msg.pl = bearerCtxt->getBearerQos().arp.prioLevel;
	icr_msg.pci = bearerCtxt->getBearerQos().arp.preEmptionCapab;
	icr_msg.pvi = bearerCtxt->getBearerQos().arp.preEmptionVulnebility;


	memcpy(&(icr_msg.gtp_teid), &(bearerCtxt->getS1uSgwUserFteid().fteid_m), sizeof(struct fteid));
	memcpy(&(icr_msg.sec_key), &((ue_ctxt->getUeSecInfo().secinfo_m).kenb_key),
			KENB_SIZE);	

	bool nsa_enabled = false;
    
	// If UE supports DCNR
	if (ue_ctxt->getUeNetCapab().ue_net_capab_m.u.bits.dcnr)
	{
		// But the network does not allow DCNR for the UE
		if (ue_ctxt->getDcnrCapable() == false)
		{
			eps_nw_feature_supp_presence = true;

			icr_msg.ho_restrict_list_presence = true;
			memcpy(&(icr_msg.ho_restrict_list.serving_plmn),
					&(ue_ctxt->getTai().tai_m.plmn_id), 3);

			// send HO Restriction list with NR Restriction in EPS as Secondary RAT
			icr_msg.ho_restrict_list.nr_restricted_in_eps =
					nRrestrictedinEPSasSecondaryRAT;
		}
		else
		{
		    // Ext-APN-AMBR fields in the UE context stores the value received from HSS and
		    // can hold bitrate values beyond 4.2 Gbps.
		    // In the S1 layer, bit rate values till 10Gbps can be sent in UE-AMBR IE.
		    // and a bitrate beyond 10Gbps needs to use Ext-UE-AMBR IE.
		    nsa_enabled = true;
		    uint64_t ueAmbrExt = 0;
		    if (ue_ctxt->getAmbr().ambr_m.ext_max_requested_bw_dl > 0)
		    {
		        ueAmbrExt = ((uint64_t)ue_ctxt->getAmbr().ambr_m.ext_max_requested_bw_dl) * 1000;
		        if (ueAmbrExt > UEAMBR_MAX)
		        {
		            icr_msg.ext_ue_ambr.ext_ue_ambr_DL = ueAmbrExt;
		            icr_msg.exg_max_dl_bitrate = UEAMBR_MAX;
		        }
		        else
		        {
		            icr_msg.exg_max_dl_bitrate = ueAmbrExt;
		        }
		    }

		    if (ue_ctxt->getAmbr().ambr_m.ext_max_requested_bw_ul > 0)
		    {
		        ueAmbrExt = ((uint64_t)ue_ctxt->getAmbr().ambr_m.ext_max_requested_bw_ul) * 1000;
		        if (ueAmbrExt > UEAMBR_MAX)
		        {
		            icr_msg.ext_ue_ambr.ext_ue_ambr_UL = ueAmbrExt;
		            icr_msg.exg_max_ul_bitrate = UEAMBR_MAX;
		        }
		        else
		        {
		            icr_msg.exg_max_ul_bitrate = ueAmbrExt;
		        }
		    }
		}
	}
    
	struct Buffer nasBuffer;
	struct nasPDU nas = {0};
    nasBuffer.pos = 0; 
	nas.header.security_header_type = IntegrityProtectedCiphered; 
 	nas.header.proto_discriminator = EPSMobilityManagementMessages;
	uint8_t mac[MAC_SIZE] = {0};
	memcpy(nas.header.mac, mac, MAC_SIZE);

	nas.header.seq_no = ue_ctxt->getUeSecInfo().getDownlinkSeqNo(); 
	nas.dl_count = ue_ctxt->getUeSecInfo().getDownlinkCount();	
	ue_ctxt->getUeSecInfo().increment_downlink_count();

	nas.header.message_type = AttachAccept;
	nas.header.eps_bearer_identity = 0;
	nas.header.procedure_trans_identity = 1;

	nas.elements_len = ICS_REQ_NO_OF_NAS_IES;
	if (eps_nw_feature_supp_presence)
	    nas.elements_len ++;

	nas.elements = (nas_pdu_elements *) calloc(nas.elements_len,sizeof(nas_pdu_elements));
	nas.elements[0].pduElement.attach_res = 2; /* EPS Only */
	nas.elements[1].pduElement.t3412 = 224; 
	nas.elements[2].pduElement.tailist.type = 1;
	nas.elements[2].pduElement.tailist.num_of_elements = 0;
   /* S1AP TAI mcc 123, mnc 456 : 214365 */
   /* NAS TAI mcc 123, mnc 456 : 216354 */
	memcpy(&(nas.elements[2].pduElement.tailist.partial_list[0]),
			&(ue_ctxt->getTai().tai_m), sizeof(struct TAI));
	
	/* Fill ESM info */
	nas.elements[3].pduElement.esm_msg.eps_bearer_id = bearerCtxt->getBearerId(); /* TODO: revisit */
	nas.elements[3].pduElement.esm_msg.proto_discriminator = EPSSessionManagementMessage;
	nas.elements[3].pduElement.esm_msg.procedure_trans_identity = sessionCtxt->getPti();
	nas.elements[3].pduElement.esm_msg.session_management_msgs = ESM_MSG_ACTV_DEF_BEAR__CTX_REQ;
	nas.elements[3].pduElement.esm_msg.eps_qos.qci = bearerCtxt->getBearerQos().qci;
	nas.elements[3].pduElement.esm_msg.apn.len = sessionCtxt->getAccessPtName().apnname_m.len;
	memcpy(nas.elements[3].pduElement.esm_msg.apn.val, sessionCtxt->getAccessPtName().apnname_m.val, sessionCtxt->getAccessPtName().apnname_m.len);
    log_msg(LOG_DEBUG, "ESM apn length = %d ",nas.elements[3].pduElement.esm_msg.apn.len);

	nas.elements[3].pduElement.esm_msg.pco_opt.pco_length = procedure_p->getPcoOptionsLen();
	memcpy(nas.elements[3].pduElement.esm_msg.pco_opt.pco_options, procedure_p->getPcoOptions(), nas.elements[3].pduElement.pco_opt.pco_length);

	nas.elements[3].pduElement.esm_msg.pdn_addr.type = 1;
	nas.elements[3].pduElement.esm_msg.pdn_addr.ipv4 = htonl(sessionCtxt->getPdnAddr().paa_m.ip_type.ipv4.s_addr);
	nas.elements[3].pduElement.esm_msg.linked_ti.flag = 0;
	nas.elements[3].pduElement.esm_msg.linked_ti.val = 0;
	MmeNasUtils::get_negotiated_qos_value(&nas.elements[3].pduElement.esm_msg.negotiated_qos);

    const Ambr& apnAmbr = sessionCtxt->getApnAmbr();

    // NAS Ext-APN-AMBR IE is included when the APN AMBR to be sent
    // exceeds 65.2 Gbps.
    if (nsa_enabled && (apnAmbr.ambr_m.ext_max_requested_bw_dl >= NAS_EXT_APN_AMBR_MIN
            || apnAmbr.ambr_m.ext_max_requested_bw_ul >= NAS_EXT_APN_AMBR_MIN))
    {
        uint8_t unit = 0;
        uint16_t convExtApnAmbr = 0;

        nas.elements[3].pduElement.esm_msg.extd_apn_ambr.length = 6;

        if (apnAmbr.ambr_m.ext_max_requested_bw_dl > NAS_EXT_APN_AMBR_MIN)
        {
            MmeNasUtils::calculate_ext_apn_ambr(
                    (apnAmbr.ambr_m.ext_max_requested_bw_dl / 1000 ), unit, 
                    convExtApnAmbr);
            convExtApnAmbr = htons(convExtApnAmbr);

            nas.elements[3].pduElement.esm_msg.extd_apn_ambr.ext_apn_ambr[0] =
                    unit;
            memcpy(
                    &nas.elements[3].pduElement.esm_msg.extd_apn_ambr.ext_apn_ambr[1],
                    &convExtApnAmbr, sizeof(convExtApnAmbr));
        }
        if (apnAmbr.ambr_m.ext_max_requested_bw_ul > NAS_EXT_APN_AMBR_MIN)
        {
            MmeNasUtils::calculate_ext_apn_ambr(
                    (apnAmbr.ambr_m.ext_max_requested_bw_ul / 1000), unit,
                    convExtApnAmbr);
            convExtApnAmbr = htons(convExtApnAmbr);

            nas.elements[3].pduElement.esm_msg.extd_apn_ambr.ext_apn_ambr[3] =
                    unit;
            memcpy(
                    &nas.elements[3].pduElement.esm_msg.extd_apn_ambr.ext_apn_ambr[4],
                    &convExtApnAmbr, sizeof(convExtApnAmbr));
        }
    }

    /* Send the allocated GUTI to UE  */
	nas.elements[4].pduElement.mi_guti.odd_even_indication = 0;
	nas.elements[4].pduElement.mi_guti.id_type = 6;

	memcpy(&(nas.elements[4].pduElement.mi_guti.plmn_id),
			&(ue_ctxt->getTai().tai_m.plmn_id), 3); // ajaymerge - sizeof(struct PLMN)); dont use size..it has extra fields
	nas.elements[4].pduElement.mi_guti.mme_grp_id = htons(mme_cfg->mme_group_id);
	nas.elements[4].pduElement.mi_guti.mme_code = mme_cfg->mme_code;
    nas.elements[4].pduElement.mi_guti.m_TMSI = htonl(ue_ctxt->getMTmsi());

    // NAS Optional IEs starts from index 5
    uint8_t index = 5;
    if (eps_nw_feature_supp_presence)
    {
        nas.opt_ies_flags.eps_nw_feature_supp_presence = true;
        // add EPS Nwk Feature Support with restrict dcnr flag set
        nas.elements[index++].pduElement.eps_nw_feature_supp.restrictDcnr = 1;
    }
        
	MmeNasUtils::encode_nas_msg(&nasBuffer, &nas, ue_ctxt->getUeSecInfo());
	memcpy(&icr_msg.nasMsgBuf[0], &nasBuffer.buf[0], nasBuffer.pos);
	icr_msg.nasMsgSize = nasBuffer.pos;
	log_msg(LOG_DEBUG, "nas message size %d ",icr_msg.nasMsgSize);
	free(nas.elements);

	cmn::ipc::IpcAddress destAddr;
	destAddr.u32 = TipcServiceInstance::s1apAppInstanceNum_c;
	
    mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_NAS_INITIAL_CONTEXT_REQUEST);
	MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));   
	mmeIpcIf.dispatchIpcMsg((char *) &icr_msg, sizeof(icr_msg), destAddr);
	
	ProcedureStats::num_of_init_ctxt_req_to_ue_sent ++;
	log_msg(LOG_DEBUG, "Leaving send_init_ctxt_req_to_ue_v ");
		
	return ActStatus::PROCEED;
}

ActStatus ActionHandlers::process_init_ctxt_resp(SM::ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside process_init_ctxt_resp ");

	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ue_ctxt, "Invalid UE");

	MmeAttachProcedureCtxt *procCtxt = dynamic_cast<MmeAttachProcedureCtxt*>(cb.getTempDataBlock());
	VERIFY(procCtxt, return ActStatus::ABORT, "Procedure Context is NULL ");

	auto& sessionCtxtContainer = ue_ctxt->getSessionContextContainer();
	VERIFY(sessionCtxtContainer.size() > 0, return ActStatus::ABORT, "Sessions Container is empty");

	SessionContext* sessionCtxt = sessionCtxtContainer.front();
	VERIFY(sessionCtxt, return ActStatus::ABORT, "Session Context is NULL ");
	
	MsgBuffer* msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
	VERIFY(msgBuf, return ActStatus::ABORT, "Invalid message buffer ")

	const initctx_resp_Q_msg_t *ics_res = static_cast<const initctx_resp_Q_msg_t*>(msgBuf->getDataPointer());
	
	fteid S1uEnbUserFteid;
	S1uEnbUserFteid.header.iface_type = 0;
	S1uEnbUserFteid.header.v4 = 1;
	S1uEnbUserFteid.header.teid_gre = ics_res->erab_setup_resp_list.erab_su_res_item[0].gtp_teid;
	S1uEnbUserFteid.ip.ipv4 = *(struct in_addr*)&ics_res->erab_setup_resp_list.erab_su_res_item[0].transportLayerAddress;
	
	BearerContext* bearerCtxt = sessionCtxt->findBearerContextByBearerId(sessionCtxt->getLinkedBearerId());
	VERIFY(bearerCtxt, return ActStatus::ABORT, "Bearer Context is NULL ");

	bearerCtxt->setS1uEnbUserFteid(Fteid(S1uEnbUserFteid));

	ProcedureStats::num_of_processed_init_ctxt_resp ++;
	log_msg(LOG_DEBUG, "Leaving process_init_ctxt_resp ");
	
	return ActStatus::PROCEED;
}

ActStatus ActionHandlers::send_mb_req_to_sgw(SM::ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside send_mb_req_to_sgw ");

	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ue_ctxt, "Invalid UE");

	auto& sessionCtxtContainer = ue_ctxt->getSessionContextContainer();
	VERIFY(sessionCtxtContainer.size() > 0, return ActStatus::ABORT, "Sessions Container is empty");

	SessionContext* sessionCtxt = sessionCtxtContainer.front();
	VERIFY(sessionCtxt, return ActStatus::ABORT, "Session Context is NULL ");
	
	struct MB_Q_msg mb_msg;
	mb_msg.msg_type = modify_bearer_request;
	mb_msg.ue_idx = ue_ctxt->getContextID();
	
	memset(mb_msg.indication, 0, S11_MB_INDICATION_FLAG_SIZE); /*TODO : future*/

	BearerContext* bearerCtxt = sessionCtxt->findBearerContextByBearerId(sessionCtxt->getLinkedBearerId());
	VERIFY(bearerCtxt, return ActStatus::ABORT, "Bearer Context is NULL ");

	mb_msg.bearer_ctx_list.bearers_count = 1;
	mb_msg.bearer_ctx_list.bearer_ctxt[0].eps_bearer_id = bearerCtxt->getBearerId();

	memcpy(&(mb_msg.s11_sgw_c_fteid), &(sessionCtxt->getS11SgwCtrlFteid().fteid_m),
		sizeof(struct fteid));

	memcpy(&(mb_msg.bearer_ctx_list.bearer_ctxt[0].s1u_enb_fteid), &(bearerCtxt->getS1uEnbUserFteid().fteid_m),
		sizeof(struct fteid));
	mb_msg.servingNetworkIePresent = false;
	mb_msg.userLocationInformationIePresent = false;

	cmn::ipc::IpcAddress destAddr;
	destAddr.u32 = TipcServiceInstance::s11AppInstanceNum_c;

    mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_S11_MODIFY_BEARER_REQUEST);
	MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));   
	mmeIpcIf.dispatchIpcMsg((char *) &mb_msg, sizeof(mb_msg), destAddr);
		
	ProcedureStats::num_of_mb_req_to_sgw_sent ++;
	log_msg(LOG_DEBUG, "Leaving send_mb_req_to_sgw ");
	
	return ActStatus::PROCEED;

}

ActStatus ActionHandlers::process_attach_cmp_from_ue(SM::ControlBlock& cb)
{	
	log_msg(LOG_DEBUG, "Inside handle_attach_cmp_from_ue ");

	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ue_ctxt, "Invalid UE");

	//ue_ctxt->getUeSecInfo().increment_uplink_count();

	ProcedureStats::num_of_processed_attach_cmp_from_ue ++;
	log_msg(LOG_DEBUG, "Leaving handle_attach_cmp_from_ue ");
	
	return ActStatus::PROCEED;
}

ActStatus ActionHandlers::process_mb_resp(SM::ControlBlock& cb)
{	
	log_msg(LOG_DEBUG, "Inside handle_mb_resp ");
	
	ProcedureStats::num_of_processed_mb_resp ++;
	return ActStatus::PROCEED;
}

ActStatus ActionHandlers::check_and_send_emm_info(SM::ControlBlock& cb)
{
    log_msg(LOG_DEBUG, "Inside check_and_send_emm_info ");

    UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
    VERIFY_UE(cb, ue_ctxt, "Invalid UE");
    
    MmeProcedureCtxt *procCtxt = dynamic_cast<MmeProcedureCtxt*>(cb.getTempDataBlock());
    VERIFY(procCtxt, return ActStatus::ABORT, "Procedure Context is NULL ");

    if (MmeCommonUtils::isEmmInfoRequired(cb, *ue_ctxt, *procCtxt))
    {
    	struct ue_emm_info temp;
    	temp.msg_type = emm_info_request;
    	temp.enb_fd = ue_ctxt->getEnbFd();
    	temp.enb_s1ap_ue_id = ue_ctxt->getS1apEnbUeId();
    	temp.mme_s1ap_ue_id = ue_ctxt->getContextID();

		struct Buffer nasBuffer;
		struct nasPDU nas = {0};
		const uint8_t num_nas_elements = 1;
		nas.elements = (nas_pdu_elements *) calloc(num_nas_elements, sizeof(nas_pdu_elements)); // TODO : should i use new ?
		nas.elements_len = num_nas_elements;
		nas.header.security_header_type = IntegrityProtectedCiphered;
		nas.header.proto_discriminator = EPSMobilityManagementMessages;
		uint8_t mac[MAC_SIZE] = {0};
		memcpy(nas.header.mac, mac, MAC_SIZE);


		nas.header.seq_no = ue_ctxt->getUeSecInfo().getDownlinkSeqNo(); 
		nas.dl_count = ue_ctxt->getUeSecInfo().getDownlinkCount();	
		ue_ctxt->getUeSecInfo().increment_downlink_count();

		nas.header.message_type = EMMInformation;
		/* passing network name in apn */
		// TODO - network name configurable 
		std::string network("Aether");
		nas.elements[0].pduElement.apn.len = network.length(); 
		strcpy((char *)nas.elements[0].pduElement.apn.val, (char *)network.c_str()); 
		MmeNasUtils::encode_nas_msg(&nasBuffer, &nas, ue_ctxt->getUeSecInfo());
		memcpy(&temp.nasMsgBuf[0], &nasBuffer.buf[0], nasBuffer.pos);
		temp.nasMsgSize = nasBuffer.pos;
		free(nas.elements);

    	cmn::ipc::IpcAddress destAddr;
    	destAddr.u32 = TipcServiceInstance::s1apAppInstanceNum_c;
    	
        mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_NAS_EMM_INFORMATION_REQ);
        MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));
        mmeIpcIf.dispatchIpcMsg((char*) &temp, sizeof(temp), destAddr);

    	ProcedureStats::num_of_emm_info_sent++;
    }

    log_msg(LOG_DEBUG, "Leaving check_and_send_emm_info ");

    return ActStatus::PROCEED;

}

ActStatus ActionHandlers::attach_done(SM::ControlBlock& cb)
{	
	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	VERIFY_UE(cb, ue_ctxt, "Invalid UE");

	MmContext* mmCtxt = ue_ctxt->getMmContext();
	VERIFY(mmCtxt, return ActStatus::ABORT, "MM Context is NULL ");

	mmCtxt->setMmState(EpsAttached);
	
	mmeStats::Instance()->increment(mmeStatsCounter::MME_PROCEDURES_ATTACH_PROC_SUCCESS);

	MmeProcedureCtxt* procedure_p = static_cast<MmeProcedureCtxt*>(cb.getTempDataBlock());
	MmeContextManagerUtils::deallocateProcedureCtxt(cb, procedure_p);

	ProcedureStats::num_of_attach_done++;
	ProcedureStats::num_of_subscribers_attached ++;

	log_msg(LOG_DEBUG,"Leaving attach done");

    mmeStats::Instance()->increment(mmeStatsCounter::MME_NUM_ACTIVE_SUBSCRIBERS);

	return ActStatus::PROCEED;
}

ActStatus ActionHandlers::send_attach_reject(ControlBlock& cb)
{
        UEContext* ueCtxt_p = static_cast<UEContext*>(cb.getPermDataBlock());
        VERIFY_UE(cb, ueCtxt_p, "Invalid UE");
	    log_msg(LOG_DEBUG,"Sending attach reject UE index %u ", ueCtxt_p->getContextID());

        MmeAttachProcedureCtxt *procCtxt = dynamic_cast<MmeAttachProcedureCtxt*>(cb.getTempDataBlock());
        
        struct commonRej_info attach_rej;
        attach_rej.msg_type = attach_reject;
        attach_rej.ue_idx = ueCtxt_p->getContextID();
        attach_rej.s1ap_enb_ue_id = ueCtxt_p->getS1apEnbUeId();
        attach_rej.enb_fd = ueCtxt_p->getEnbFd();
        if (procCtxt != NULL)
        {
            attach_rej.cause = MmeCauseUtils::convertToNasEmmCause(procCtxt->getMmeErrorCause());
        }
        else
        {
            attach_rej.cause = emmCause_network_failure;
        }
		struct Buffer nasBuffer;
		struct nasPDU nas = {0};
		const uint8_t num_nas_elements = 1;
		nas.elements = (nas_pdu_elements *) calloc(num_nas_elements, sizeof(nas_pdu_elements)); // TODO : should i use new ?
		nas.elements_len = num_nas_elements;
		nas.header.security_header_type = Plain;
		nas.header.proto_discriminator = EPSMobilityManagementMessages;
		nas.header.message_type = AttachReject;
		nas.elements[0].pduElement.attach_res = 0x09;
		MmeNasUtils::encode_nas_msg(&nasBuffer, &nas, ueCtxt_p->getUeSecInfo());
		memcpy(&attach_rej.nasMsgBuf[0], &nasBuffer.buf[0], nasBuffer.pos);
		attach_rej.nasMsgSize = nasBuffer.pos;
		free(nas.elements);

        cmn::ipc::IpcAddress destAddr;
        destAddr.u32 = TipcServiceInstance::s1apAppInstanceNum_c;
        
        mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_NAS_ATTACH_REJECT);
        MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));   
        mmeIpcIf.dispatchIpcMsg((char *) & attach_rej, sizeof(attach_rej), destAddr);

        ProcedureStats::num_of_attach_reject_sent ++;

        return ActStatus::PROCEED;
}

ActStatus ActionHandlers::abort_attach(ControlBlock& cb)
{
    UEContext* ueCtxt_p = static_cast<UEContext*>(cb.getPermDataBlock());
    VERIFY_UE(cb, ueCtxt_p, "Invalid UE");

    ERROR_CODES errorCause = SUCCESS;
    uint16_t stateId = 0;

    MmeProcedureCtxt *procCtxt =
            dynamic_cast<MmeProcedureCtxt*>(cb.getTempDataBlock());
    if (procCtxt != NULL)
    {
        errorCause = procCtxt->getMmeErrorCause();
        stateId = procCtxt->getCurrentState()->getStateId();

        log_msg(LOG_INFO,
                "Abort ongoing Attach for subscriber %s. "
                "Current State %s, Error Cause %d, ",
                ueCtxt_p->getImsi().getDigitsArray(),
                procCtxt->getCurrentState()->getStateName(), errorCause);

        MmeContextManagerUtils::deallocateProcedureCtxt(cb, procCtxt);
    }

    mmeStats::Instance()->increment(
            mmeStatsCounter::MME_PROCEDURES_ATTACH_PROC_FAILURE);

    switch (errorCause)
    {
        case ABORT_DUE_TO_ATTACH_COLLISION:
        {
            MmeContextManagerUtils::deleteUEContext(cb.getCBIndex(), false); // retain control block
        }break;
        case ABORT_DUE_TO_S1_REL_COLLISION:
        {
            /* We intend to start a normal s1 release after the MME sends out init_ctxt_setup_req,
             * since the attach has already been accepted by the MME by this stage.
             * In all other cases, reject the attach, fire s1 release command,
             * and delete the UE Context*/
            if ((stateId != attach_wf_mb_resp) &&
                    (stateId != attach_wf_att_cmp) &&
                    (stateId != attach_wf_init_ctxt_resp) &&
                    (stateId != attach_wf_init_ctxt_resp_att_cmp))
            {
                MmeContextManagerUtils::deleteUEContext(cb.getCBIndex());
            }
        }break;
        default:
        {
            MmeContextManagerUtils::deleteUEContext(cb.getCBIndex());
        }break;
    }
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : handle_attach_request
***************************************/
ActStatus ActionHandlers::handle_attach_request(ControlBlock& cb)
{
    MmeProcedureCtxt *procCtxt = dynamic_cast<MmeProcedureCtxt*>(cb.getTempDataBlock());
    if (procCtxt != NULL)
    {
        log_msg(LOG_DEBUG, "Received Attach Req when procedure % in progress",
                procCtxt->getCtxtType());

        // abort current procedure. Set appropriate error cause for aborting the procedure
        procCtxt->setMmeErrorCause(ABORT_DUE_TO_ATTACH_COLLISION);
    }

    return ActStatus::PROCEED;
}

/***************************************
* Action handler : handle_state_guard_timeouts
***************************************/
ActStatus ActionHandlers::handle_state_guard_timeouts(ControlBlock& cb)
{
    log_msg(LOG_DEBUG,"State guard timeout.Abort.");
    MmeProcedureCtxt *procCtxt = dynamic_cast<MmeProcedureCtxt*>(cb.getTempDataBlock());
    if (procCtxt != NULL)
    {
	procCtxt->setMmeErrorCause(NETWORK_TIMEOUT);
    }
    return ActStatus::ABORT;
}
/***************************************
* Action handler : handle_state_guard_timeouts_for_csreq_ind
***************************************/
ActStatus ActionHandlers::handle_state_guard_timeouts_for_csreq_ind(ControlBlock& cb)
{
	UEContext* ueCtxt_p = static_cast<UEContext*>(cb.getPermDataBlock());
    log_msg(LOG_ERROR,"CSRsp not received from SPGW, guard timer expired. "
                      "Let's try to resolve spgw address - Ue Index %d ", 
                      ueCtxt_p != NULL ? ueCtxt_p->getContextID():0);
    // invalidate dns entries 
    mme_tables->invalidate_dns();
    mme_tables->initiate_spgw_resolution();
    return ActStatus::ABORT;
}

/***************************************
* Action handler : handle_s1_rel_req_during_attach
***************************************/
ActStatus ActionHandlers::handle_s1_rel_req_during_attach(ControlBlock& cb)
{
    MmeProcedureCtxt *procCtxt =
            dynamic_cast<MmeProcedureCtxt*>(cb.getTempDataBlock());
    if (procCtxt != NULL)
    {
        log_msg(LOG_DEBUG,
                "S1 Release Collision during Attach. Current State %s",
                procCtxt->getCurrentState()->getStateName());

        // Set appropriate error cause for aborting the procedure
        procCtxt->setMmeErrorCause(ABORT_DUE_TO_S1_REL_COLLISION);
    }

    return ActStatus::PROCEED;
}
