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

using namespace SM;
using namespace mme;
using namespace cmn;
using namespace cmn::utils;

extern mme_config_t *mme_cfg;
extern mmeConfig *mme_tables;

ActStatus ActionHandlers::validate_imsi_in_ue_context(ControlBlock& cb)
{
    UEContext* ueCtxt_p = static_cast<UEContext*>(cb.getPermDataBlock());
    if (ueCtxt_p == NULL)
    {
         log_msg(LOG_DEBUG, "send_identity_request_to_ue: ue context is NULL \n");
         return ActStatus::HALT;
    }

    if (ueCtxt_p->getImsi().isValid())
    {
        SM::Event evt(IMSI_VALIDATION_SUCCESS, NULL);
        cb.addEventToProcQ(evt);
    }
    else
    {
        // TODO: If known GUTI, IMSI_VALIDATION_FAILURE_KNOWN_GUTI to trigger id req to UE
        // If unknown GUTI, IMSI_VALIDATION_FAILURE_UNKNOWN_GUTI to query old mme
        // when s10 is supported in MME
        SM::Event evt(IMSI_VALIDATION_FAILURE, NULL);
        cb.addEventToProcQ(evt);
    }
    return ActStatus::PROCEED;
}

ActStatus ActionHandlers::send_identity_request_to_ue(ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside send_identity_request_to_ue \n");

	UEContext* ueCtxt_p = static_cast<UEContext*>(cb.getPermDataBlock());
	if (ueCtxt_p == NULL)
	{
		log_msg(LOG_DEBUG, "send_identity_request_to_ue: ue context is NULL \n");
		return ActStatus::HALT;
	}

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
	
	MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));        
	mmeIpcIf.dispatchIpcMsg((char *) &idReqMsg, sizeof(idReqMsg), destAddr);

    	ProcedureStats::num_of_id_req_sent ++;
	
	return ActStatus::PROCEED;
}

ActStatus ActionHandlers::process_identity_response(ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside process_identity_response \n");

	UEContext *ueCtxt_p = static_cast<UEContext*>(cb.getPermDataBlock());
	if (ueCtxt_p == NULL)
	{
		log_msg(LOG_DEBUG, "process_identity_response: ue context is NULL \n");
		return ActStatus::HALT;
	}

	MsgBuffer* msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
	if (msgBuf == NULL)
	{
		log_msg(LOG_DEBUG, "process_identity_response: msgBuf is NULL \n");
		return ActStatus::HALT;
	}

	const s1_incoming_msg_data_t* s1_msg_data = static_cast<const s1_incoming_msg_data_t*>(msgBuf->getDataPointer());
	if (s1_msg_data == NULL)
	{
		log_msg(LOG_DEBUG, "process_identity_response: s1MsgData is NULL \n");
		return ActStatus::HALT;
	}

	const struct identityResp_Q_msg &id_resp = s1_msg_data->msg_data.identityResp_Q_msg_m;
	if(SUCCESS != id_resp.status)
    	{
		log_msg(LOG_DEBUG, "process_identity_response: ID Response Failure NULL \n");
		return ActStatus::HALT;
	}

	uint8_t imsi[BINARY_IMSI_LEN] = {0};
    	memcpy( imsi, id_resp.IMSI, BINARY_IMSI_LEN );

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
	log_msg(LOG_DEBUG, "Inside send_air_to_hss \n");
	
	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	if (ue_ctxt == NULL)
	{
		log_msg(LOG_DEBUG, "send_air_to_hss: ue context is NULL \n");
		return ActStatus::HALT;
	}
	
	MmeProcedureCtxt* procedure_p = dynamic_cast<MmeProcedureCtxt*>(cb.getTempDataBlock());
	if (procedure_p == NULL)
	{
		log_msg(LOG_DEBUG, "send_air_to_hss: procedure context is NULL \n");
		return ActStatus::HALT;
	}

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

	MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));        
	mmeIpcIf.dispatchIpcMsg((char *) &s6a_req, sizeof(s6a_req), destAddr);

	ProcedureStats::num_of_air_sent ++;
	log_msg(LOG_DEBUG, "Leaving send_air_to_hss \n");
	
	return ActStatus::PROCEED;

}

ActStatus ActionHandlers::send_ulr_to_hss(SM::ControlBlock& cb)
{  
	log_msg(LOG_DEBUG, "Inside send_ulr_to_hss \n");
	
	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	if (ue_ctxt == NULL)
	{
		log_msg(LOG_DEBUG, "send_ulr_to_hss: ue context is NULL \n");
		return ActStatus::HALT;
	}

	s6a_Q_msg s6a_req;
    RESET_S6A_REQ_MSG(&s6a_req);

	ue_ctxt->getImsi().getImsiDigits(s6a_req.imsi);

	memcpy(&(s6a_req.tai), &(ue_ctxt->getTai().tai_m), sizeof(struct TAI));

	s6a_req.ue_idx = ue_ctxt->getContextID();
	s6a_req.msg_type = update_loc_request;

	cmn::ipc::IpcAddress destAddr;
	destAddr.u32 = TipcServiceInstance::s6AppInstanceNum_c;
	
	MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));   
	mmeIpcIf.dispatchIpcMsg((char *) &s6a_req, sizeof(s6a_req), destAddr);

	ProcedureStats::num_of_ulr_sent ++;
	log_msg(LOG_DEBUG, "Leaving send_ulr_to_hss \n");
	
	return ActStatus::PROCEED;
}


ActStatus ActionHandlers::process_aia(SM::ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside handle_aia \n");

	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	if (ue_ctxt == NULL)
	{
		log_msg(LOG_DEBUG, "handle_aia: ue context is NULL \n");
		return ActStatus::HALT;
	}
	
	MmeProcedureCtxt *procedure_p = dynamic_cast<MmeProcedureCtxt*>(cb.getTempDataBlock());
	if (procedure_p == NULL)
	{
        	log_msg(LOG_DEBUG, "handle_aia: procedure context is NULL \n");
        	return ActStatus::HALT;
        }

	MsgBuffer* msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());

	if (msgBuf == NULL)
		return ActStatus::HALT;

	const s6_incoming_msg_data_t* msgData_p = static_cast<const s6_incoming_msg_data_t*>(msgBuf->getDataPointer());

	if (msgData_p->msg_data.aia_Q_msg_m.res == S6A_AIA_FAILED)
	{	
		/* send attach reject and release UE */
        	log_msg(LOG_INFO, "AIA failed. UE %d", ue_ctxt->getContextID());
        	procedure_p->setMmeErrorCause(s6AiaFailure_c);
        	return ActStatus::ABORT;
	}

	ue_ctxt->setAiaSecInfo(E_utran_sec_vector(msgData_p->msg_data.aia_Q_msg_m.sec));
	
	ProcedureStats::num_of_processed_aia ++;
	log_msg(LOG_DEBUG, "Leaving handle_aia \n");
	
	return ActStatus::PROCEED;
}

ActStatus ActionHandlers::process_ula(SM::ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside handle_ula \n");

	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	if (ue_ctxt == NULL)
	{
		log_msg(LOG_DEBUG, "handle_ula: ue context is NULL \n");
		return ActStatus::HALT;
	}

	SessionContext* sessionCtxt = ue_ctxt->getSessionContext();
	if( sessionCtxt == NULL )
    	{
		log_msg(LOG_ERROR, "Failed to retrieve Session Context for UE IDX %d\n", cb.getCBIndex());
        	return ActStatus::HALT;
    	}
	
	MsgBuffer* msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());

	if (msgBuf == NULL)
		return ActStatus::HALT;

	const s6_incoming_msg_data_t* s6_msg_data = static_cast<const s6_incoming_msg_data_t*>(msgBuf->getDataPointer());
	const struct ula_Q_msg &ula_msg = s6_msg_data->msg_data.ula_Q_msg_m;

	sessionCtxt->setApnConfigProfileCtxId(ula_msg.apn_config_profile_ctx_id);
	DigitRegister15 ueMSISDN;
	ueMSISDN.convertFromBcdArray( reinterpret_cast<const uint8_t*>( ula_msg.MSISDN ));
	ue_ctxt->setMsisdn(ueMSISDN);
	ue_ctxt->setRauTauTimer(ula_msg.RAU_TAU_timer);
	ue_ctxt->setSubscriptionStatus(ula_msg.subscription_status);
	ue_ctxt->setNetAccessMode(ula_msg.net_access_mode);
	ue_ctxt->setAccessRestrictionData(ula_msg.access_restriction_data);
  ue_ctxt->setSubscribedApn(Apn_name(ula_msg.selected_apn));
  log_msg(LOG_DEBUG, "Selected APN %s \n",ula_msg.selected_apn.val);

	struct AMBR ambr;
	ambr.max_requested_bw_dl = ula_msg.max_requested_bw_dl;
	ambr.max_requested_bw_ul = ula_msg.max_requested_bw_ul;
	
	ue_ctxt->setAmbr(Ambr(ambr));
	
	struct PAA pdn_addr;
	pdn_addr.pdn_type = 1;
	pdn_addr.ip_type.ipv4.s_addr = ntohl(ula_msg.static_addr); // network byte order
	
	ue_ctxt->setPdnAddr(Paa(pdn_addr));
	
	ProcedureStats::num_of_processed_ula ++;
	log_msg(LOG_DEBUG, "Leaving handle_ula_v \n");
	
	return ActStatus::PROCEED;
}

ActStatus ActionHandlers::auth_req_to_ue(SM::ControlBlock& cb)
{
	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());

	if (ue_ctxt == NULL)
	{
		log_msg(LOG_DEBUG, "auth_req_to_ue: ue context is NULL \n");
		return ActStatus::HALT;
	}

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
	
	MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));   
	mmeIpcIf.dispatchIpcMsg((char *) &authreq, sizeof(authreq), destAddr);
	
	ProcedureStats::num_of_auth_req_to_ue_sent ++;
	log_msg(LOG_DEBUG, "Leaving auth_req_to_ue_v sent message of length %d\n",sizeof(authreq));
		
	return ActStatus::PROCEED;
}

ActStatus ActionHandlers::auth_response_validate(SM::ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside auth_response_validate \n");
	SM::ControlBlock* controlBlk_p = SubsDataGroupManager::Instance()->findControlBlock(cb.getCBIndex());

	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	if (ue_ctxt == NULL)
	{
		log_msg(LOG_DEBUG, "auth_response_validate: ue context or procedure ctxt is NULL \n");
		return ActStatus::HALT;
	}
	
	MmeProcedureCtxt* procedure_p = dynamic_cast<MmeProcedureCtxt*>(cb.getTempDataBlock());
	if (procedure_p == NULL)
	{
		log_msg(LOG_DEBUG, "auth_response_validate: procedure context is NULL \n");
		return ActStatus::HALT;
	}
		
	MsgBuffer* msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());

	if (msgBuf == NULL)
		return ActStatus::HALT;

	const s1_incoming_msg_data_t* s1_msg_data = static_cast<const s1_incoming_msg_data_t*>(msgBuf->getDataPointer());	
	const struct authresp_Q_msg &auth_resp = s1_msg_data->msg_data.authresp_Q_msg_m;
	
	/*Check the state*/
	if(SUCCESS != auth_resp.status) {
		log_msg(LOG_ERROR, "eNB authentication failure for UE-%d.\n", ue_ctxt->getContextID());
		if(auth_resp.auts.len == 0)
		{
			log_msg(LOG_ERROR,"No AUTS.Not Synch Failure\n");
			SM::Event evt(AUTH_RESP_FAILURE,NULL);
        		controlBlk_p->addEventToProcQ(evt);
		}
		else
		{
			log_msg(LOG_INFO,"AUTS recvd.  Synch failure. send AIR\n");
			procedure_p->setAuthRespStatus(auth_resp.status);
			procedure_p->setAuts(Auts(auth_resp.auts));
			SM::Event evt(AUTH_RESP_SYNC_FAILURE,NULL);
            		controlBlk_p->addEventToProcQ(evt);
		}
	}
	else{
        log_msg(LOG_INFO,"Auth response validation success. Proceeding to Sec mode Command\n");
        uint64_t xres = 0;
        memcpy(&xres, 
               ue_ctxt->getAiaSecInfo().AiaSecInfo_mp->xres.val, sizeof(uint64_t));
        uint64_t res = 0;
        memcpy(&res, auth_resp.res.val, sizeof(uint64_t));
        log_msg(LOG_DEBUG, "Auth response Comparing received result from UE " 
                " (%lu) with xres (%lu). Length %d", res, 
                xres, auth_resp.res.len);

        if(memcmp((ue_ctxt->getAiaSecInfo().AiaSecInfo_mp->xres.val),
                  (auth_resp.res.val),
                  auth_resp.res.len) != 0) {
            log_msg(LOG_ERROR, "Invalid Auth response Comparing received result "
                    "from UE (%lu) with xres (%lu). Length %d", 
                    res, xres, auth_resp.res.len);
            return ActStatus::HALT;
        }

        ProcedureStats::num_of_processed_auth_response ++;
        SM::Event evt(AUTH_RESP_SUCCESS,NULL);
        controlBlk_p->addEventToProcQ(evt);
	}
	
	log_msg(LOG_DEBUG, "Leaving auth_response_validate \n");
	
	return ActStatus::PROCEED;
}

ActStatus ActionHandlers::send_auth_reject(SM::ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside send_auth_reject \n");

	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	if (ue_ctxt == NULL)
	{
		log_msg(LOG_DEBUG, "send_auth_reject: ue context is NULL \n");
		return ActStatus::HALT;
	}
	
	ProcedureStats::num_of_auth_reject_sent ++;
	return ActStatus::HALT;
}

ActStatus ActionHandlers::select_sec_alg(UEContext *ue_ctxt)
{
	log_msg(LOG_DEBUG, "Inside select_sec_alg \n");

	uint8_t eea;
    uint8_t eia;
    nas_int_algo_enum int_alg;
    nas_ciph_algo_enum sec_alg;
    memcpy(&eea, 
           &ue_ctxt->getUeNetCapab().ue_net_capab_m.capab[0],sizeof(uint8_t));
    memcpy(&eia, 
           &ue_ctxt->getUeNetCapab().ue_net_capab_m.capab[1],sizeof(uint8_t));

    int_alg = (nas_int_algo_enum)MmeCommonUtils::select_preferred_int_algo(eia);
    sec_alg = (nas_ciph_algo_enum)MmeCommonUtils::select_preferred_sec_algo(eea);

    ue_ctxt->getUeSecInfo().setSelectedIntAlg(int_alg);
    ue_ctxt->getUeSecInfo().setSelectedSecAlg(sec_alg);
	
	return ActStatus::PROCEED;
}

ActStatus ActionHandlers::sec_mode_cmd_to_ue(SM::ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside sec_mode_cmd_to_ue \n");

	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	if (ue_ctxt == NULL)
	{
		log_msg(LOG_DEBUG, "sec_mode_cmd_to_ue: ue context is NULL \n");
		return ActStatus::HALT;
	}
	
    E_UTRAN_sec_vector *secVect = const_cast<E_UTRAN_sec_vector*>(ue_ctxt->getAiaSecInfo().AiaSecInfo_mp);
	secinfo& secInfo = const_cast<secinfo&>(ue_ctxt->getUeSecInfo().secinfo_m);
	sec_mode_Q_msg sec_mode_msg;
	sec_mode_msg.msg_type  = sec_mode_command;
	sec_mode_msg.ue_idx = ue_ctxt->getContextID();
	sec_mode_msg.enb_fd = ue_ctxt->getEnbFd();
	sec_mode_msg.enb_s1ap_ue_id = ue_ctxt->getS1apEnbUeId();

    select_sec_alg(ue_ctxt);
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
	const uint8_t num_nas_elements = SEC_MODE_NO_OF_NAS_IES;
	nas.elements = (nas_pdu_elements *) calloc(num_nas_elements, sizeof(nas_pdu_elements)); // TODO : should i use new ?
	nas.elements_len = num_nas_elements;
	nas.elements->pduElement.ue_network.len = ue_ctxt->getUeNetCapab().ue_net_capab_m.len;
	if(ue_ctxt->getUeNetCapab().ue_net_capab_m.len >= 4)
	{
        /*Copy first 4 bytes of security algo info*/
	    memcpy(nas.elements->pduElement.ue_network.capab, ue_ctxt->getUeNetCapab().ue_net_capab_m.capab, 4);
	   
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
	        nas.elements->pduElement.ue_network.capab[4] = val;
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
            memcpy(nas.elements->pduElement.ue_network.capab,
				   	ue_ctxt->getUeNetCapab().ue_net_capab_m.capab,
					ue_ctxt->getUeNetCapab().ue_net_capab_m.len);
	}

	MmeNasUtils::encode_nas_msg(&nasBuffer, &nas, ue_ctxt->getUeSecInfo());
	memcpy(&sec_mode_msg.nasMsgBuf[0], &nasBuffer.buf[0], nasBuffer.pos);
	sec_mode_msg.nasMsgSize = nasBuffer.pos;
	free(nas.elements); 

	cmn::ipc::IpcAddress destAddr;
	destAddr.u32 = TipcServiceInstance::s1apAppInstanceNum_c;
	
	MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));   
	mmeIpcIf.dispatchIpcMsg((char *) &sec_mode_msg, sizeof(sec_mode_msg), destAddr);
	
	ProcedureStats::num_of_sec_mode_cmd_to_ue_sent ++;
	log_msg(LOG_DEBUG, "Leaving sec_mode_cmd_to_ue \n");
	
	return ActStatus::PROCEED;
}


ActStatus ActionHandlers::process_sec_mode_resp(SM::ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside handle_sec_mode_resp \n");

	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	if (ue_ctxt == NULL)
	{
		log_msg(LOG_DEBUG, "handle_sec_mode_resp: ue context is NULL \n");
		return ActStatus::HALT;
	}

	MmeProcedureCtxt* procedure_p = dynamic_cast<MmeProcedureCtxt*>(cb.getTempDataBlock());
        if (procedure_p == NULL)
        {
                log_msg(LOG_DEBUG, "check_esm_info_req_required: procedure context is NULL \n");
                return ActStatus::HALT;
        }

	MsgBuffer* msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());

	if (msgBuf == NULL)
		return ActStatus::HALT;

	const s1_incoming_msg_data_t* s1_msg_data = static_cast<const s1_incoming_msg_data_t*>(msgBuf->getDataPointer());
	const secmode_resp_Q_msg &secmode_resp = s1_msg_data->msg_data.secmode_resp_Q_msg_m;
	if(SUCCESS == secmode_resp.status)
	{
		log_msg(LOG_INFO, "Sec mode complete rcv. UE - %d.\n",
				ue_ctxt->getContextID());
		
	}	
	else
	{
		log_msg(LOG_INFO, "Sec mode failed. UE %d", ue_ctxt->getContextID());
		procedure_p->setMmeErrorCause(secModeRespFailure_c);
		return ActStatus::ABORT;
	}

	ProcedureStats::num_of_processed_sec_mode_resp ++;
	log_msg(LOG_DEBUG, "Leaving handle_sec_mode_resp \n");
	
	return ActStatus::PROCEED;
}

ActStatus ActionHandlers::check_esm_info_req_required(SM::ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside check_esm_info_req_required \n");

	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	if (ue_ctxt == NULL)
	{
		log_msg(LOG_DEBUG, "check_esm_info_req_required: ue context is NULL \n");
		return ActStatus::HALT;
	}
	
	MmeAttachProcedureCtxt* procedure_p = dynamic_cast<MmeAttachProcedureCtxt*>(cb.getTempDataBlock());
	if (procedure_p == NULL)
	{
		log_msg(LOG_DEBUG, "check_esm_info_req_required: procedure context is NULL \n");
		return ActStatus::HALT;		
	}
	SessionContext* sessionCtxt = SubsDataGroupManager::Instance()->getSessionContext();
	if( sessionCtxt == NULL )
	{
	    log_msg(LOG_ERROR, "Failed to allocate Session Context for UE IDX %d\n", cb.getCBIndex());

	    return ActStatus::HALT;
	}
	BearerContext* bearerCtxt_p = SubsDataGroupManager::Instance()->getBearerContext();
	if( bearerCtxt_p == NULL )
	{
	    log_msg(LOG_ERROR, "Failed to allocate Bearer context for UE IDx %d\n", cb.getCBIndex());

	    return ActStatus::HALT;
	}

	bearerCtxt_p->setBearerId(5);
	sessionCtxt->setPti(procedure_p->getPti());
	sessionCtxt->setBearerContext( bearerCtxt_p );
	ue_ctxt->setSessionContext(sessionCtxt);
	
	if (procedure_p->getEsmInfoTxRequired() == false)
	{
		SM::Event evt(ESM_INFO_NOT_REQUIRED, NULL);
		cb.addEventToProcQ(evt);
	} 
	else
	{
		SM::Event evt(ESM_INFO_REQUIRED, NULL);
		cb.addEventToProcQ(evt);
	}
	
	return  ActStatus::PROCEED;
}

ActStatus ActionHandlers::send_esm_info_req_to_ue(SM::ControlBlock& cb)
{
	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());

	if (ue_ctxt == NULL)
	{
		log_msg(LOG_DEBUG, "send_esm_info_req_to_ue: ue context is NULL \n");
		return ActStatus::HALT;
	}

	SessionContext *sessionCtxt = ue_ctxt->getSessionContext();
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

	MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));   
	mmeIpcIf.dispatchIpcMsg((char *) &esmreq, sizeof(esmreq), destAddr);

	log_msg(LOG_DEBUG, "Leaving send_esm_info_req_to_ue \n");
	ProcedureStats::num_of_esm_info_req_to_ue_sent ++;
	return ActStatus::PROCEED;
}

ActStatus ActionHandlers::process_esm_info_resp(SM::ControlBlock& cb)
{
	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	if (ue_ctxt == NULL)
	{
		log_msg(LOG_DEBUG, "handle_ula: ue context is NULL \n");
		return ActStatus::HALT;
	}

    	MmeAttachProcedureCtxt* procedure_p = dynamic_cast<MmeAttachProcedureCtxt*>(cb.getTempDataBlock());
    	if (procedure_p == NULL)
    	{
        	log_msg(LOG_DEBUG, "process_esm_info_resp: procedure context is NULL \n");
        	return ActStatus::HALT;
    	}

	MsgBuffer* msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());

	if (msgBuf == NULL)
		return ActStatus::HALT;

	const s1_incoming_msg_data_t* s1_msg_data = static_cast<const s1_incoming_msg_data_t*>(msgBuf->getDataPointer());
	const struct esm_resp_Q_msg &esm_res =s1_msg_data->msg_data.esm_resp_Q_msg_m;
    
    	if (esm_res.status != SUCCESS)
    	{
        	log_msg(LOG_ERROR, "ESM Response failed \n");
    	}

	procedure_p->setRequestedApn(Apn_name(esm_res.apn));

	ProcedureStats::num_of_handled_esm_info_resp++;
	return ActStatus::PROCEED;
}

ActStatus ActionHandlers::cs_req_to_sgw(SM::ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside cs_req_to_sgw \n");

	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	MmeAttachProcedureCtxt *procCtxt = dynamic_cast<MmeAttachProcedureCtxt*>(cb.getTempDataBlock());
	if (ue_ctxt == NULL  || procCtxt == NULL)
	{
		log_msg(LOG_DEBUG, "handle_ula: UE context or Procedure Context is NULL \n");

		return ActStatus::HALT;
	}

   	SessionContext* sessionCtxt = ue_ctxt->getSessionContext();
	if( sessionCtxt == NULL )
	{
		log_msg(LOG_ERROR, "Failed to allocate Session Context for UE IDX %d\n", cb.getCBIndex());

		return ActStatus::HALT;
	}

	BearerContext* bearerCtxt_p = sessionCtxt->getBearerContext();
	if( bearerCtxt_p == NULL )
	{
		log_msg(LOG_ERROR, "Failed to allocate Bearer context for UE IDx %d\n", cb.getCBIndex());

		return ActStatus::HALT;
	}

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
        log_msg(LOG_DEBUG, "APN %s length = %d \n",apnName.apnname_m.val,apnName.apnname_m.len);
        const unsigned char *ptr = &apnName.apnname_m.val[1]; /*BUG*/
        std::string temp_str((char *)ptr);
        apn_config *temp = mme_tables->find_apn(temp_str); 
        if(temp != NULL)
        {
            log_msg(LOG_DEBUG, "Found APN mapping in static table %x \n",cs_msg.sgw_ip);
            cs_msg.sgw_ip = temp->get_sgw_addr(); 
            cs_msg.pgw_ip = temp->get_pgw_addr(); 
        } 
        else
        {
            log_msg(LOG_DEBUG, "APN not found in static apn configuration \n");
        }
    }
	memcpy(&(cs_msg.tai), &(ue_ctxt->getTai().tai_m), sizeof(struct TAI));
	memcpy(&(cs_msg.utran_cgi), &(ue_ctxt->getUtranCgi().cgi_m), sizeof(struct CGI));
	cs_msg.pco_length = procCtxt->getPcoOptionsLen();
	if(procCtxt->getPcoOptionsLen() > 0){
		memcpy(&(cs_msg.pco_options[0]), procCtxt->getPcoOptions(),cs_msg.pco_length);
	}
	const AMBR& ambr = ue_ctxt->getAmbr().ambr_m;

	cs_msg.max_requested_bw_dl = ambr.max_requested_bw_dl;
	cs_msg.max_requested_bw_ul = ambr.max_requested_bw_ul;
	
	const PAA& pdn_addr = ue_ctxt->getPdnAddr().paa_m;
	
	cs_msg.paa_v4_addr = pdn_addr.ip_type.ipv4.s_addr; /* host order */

	memset(cs_msg.MSISDN, 0, BINARY_IMSI_LEN);
	
	const DigitRegister15& ueMSISDN = ue_ctxt->getMsisdn();
	ueMSISDN.convertToBcdArray(cs_msg.MSISDN);

	cmn::ipc::IpcAddress destAddr;
	destAddr.u32 = TipcServiceInstance::s11AppInstanceNum_c;

    gtp_outgoing_msgs_t top_msg;
    top_msg.msg_type = cs_msg.msg_type;
    top_msg.ue_idx = cs_msg.ue_idx;
    memcpy(&top_msg.csr_req_msg, &cs_msg, sizeof(cs_msg)); 

	MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));   
	mmeIpcIf.dispatchIpcMsg((char *) &top_msg, sizeof(top_msg), destAddr);

	ProcedureStats::num_of_cs_req_to_sgw_sent ++;
	log_msg(LOG_DEBUG, "Leaving cs_req_to_sgw \n");

    return ActStatus::PROCEED;
}

ActStatus ActionHandlers::process_cs_resp(SM::ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Entering handle_cs_resp \n");

	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	if (ue_ctxt == NULL)
	{
		log_msg(LOG_DEBUG, "handle_cs_resp: ue context is NULL \n");
		return ActStatus::HALT;
	}
	
	MmeAttachProcedureCtxt* procedure_p = dynamic_cast<MmeAttachProcedureCtxt*>(cb.getTempDataBlock());
	if (procedure_p == NULL)
	{
		log_msg(LOG_DEBUG, "send_init_ctxt_req_to_ue: procedure context is NULL \n");
		return ActStatus::HALT;
	}

	SessionContext* sessionCtxt = ue_ctxt->getSessionContext();
	if (sessionCtxt == NULL)
	{
		log_msg(LOG_DEBUG, "handle_cs_resp: session ctxt is NULL \n");
		return ActStatus::HALT;
	}

	MsgBuffer* msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());

	if (msgBuf == NULL)
		return ActStatus::HALT;

	const gtp_incoming_msg_data_t* gtp_msg_data= static_cast<const gtp_incoming_msg_data_t*>(msgBuf->getDataPointer());
	const struct csr_Q_msg& csr_info = gtp_msg_data->msg_data.csr_Q_msg_m;

    if(csr_info.status != GTPV2C_CAUSE_REQUEST_ACCEPTED)
    {
		log_msg(LOG_DEBUG, "CSRsp rejected by SGW with cause %d \n",csr_info.status);
       	return ActStatus::ABORT;
    }

	BearerContext* bearerCtxt = sessionCtxt->getBearerContext();
	if( bearerCtxt == NULL )
	{
		log_msg(LOG_ERROR, "Failed to retrive Bearer context for UE IDx %d\n", cb.getCBIndex());

		return ActStatus::HALT;
	}

	procedure_p->setPcoOptions(csr_info.pco_options,csr_info.pco_length);
	log_msg(LOG_DEBUG, "Process CSRsp - PCO length %d\n", csr_info.pco_options,csr_info.pco_length);
	
	sessionCtxt->setS11SgwCtrlFteid(Fteid(csr_info.s11_sgw_fteid));
	sessionCtxt->setS5S8PgwCtrlFteid(Fteid(csr_info.s5s8_pgwc_fteid));

	bearerCtxt->setS1uSgwUserFteid(Fteid(csr_info.s1u_sgw_fteid));
	bearerCtxt->setS5S8PgwUserFteid(Fteid(csr_info.s5s8_pgwu_fteid));

	sessionCtxt->setPdnAddr(Paa(csr_info.pdn_addr));
		
	ProcedureStats::num_of_processed_cs_resp ++;
	log_msg(LOG_DEBUG, "Leaving handle_cs_resp \n");
	
	return ActStatus::PROCEED;
}

ActStatus ActionHandlers::send_init_ctxt_req_to_ue(SM::ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside send_init_ctxt_req_to_ue \n");

	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	if (ue_ctxt == NULL )
	{
		log_msg(LOG_DEBUG, "send_init_ctxt_req_to_ue: ue context is NULL \n");
		return ActStatus::HALT;
	}

	MmeAttachProcedureCtxt* procedure_p = dynamic_cast<MmeAttachProcedureCtxt*>(cb.getTempDataBlock());
	if (procedure_p == NULL)
	{
		log_msg(LOG_DEBUG, "send_init_ctxt_req_to_ue: procedure context is NULL \n");
		return ActStatus::HALT;
	}

	if (procedure_p->getAttachType() == imsiAttach_c ||
			procedure_p->getAttachType() == unknownGutiAttach_c)
	{
		uint32_t mTmsi = MmeCommonUtils::allocateMtmsi();
		if (mTmsi == 0)
		{
			log_msg(LOG_DEBUG, "send_init_ctxt_req_to_ue: Failed to allocate mTmsi \n");
			return ActStatus::HALT;
		}

		ue_ctxt->setMTmsi(mTmsi);

		// TODO: Should this be done here or attach_done method
		SubsDataGroupManager::Instance()->addmTmsikey(mTmsi, ue_ctxt->getContextID());
	}

	SessionContext* sessionCtxt = ue_ctxt->getSessionContext();
	if (sessionCtxt == NULL)
	{
		log_msg(LOG_DEBUG, "send_init_ctxt_req_to_ue: session ctxt is NULL \n");
		return ActStatus::HALT;
	}

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
	secInfo.next_hop_chaining_count = 0 ;
	memcpy(secInfo.next_hop_nh , secInfo.kenb_key, KENB_SIZE);
	
	init_ctx_req_Q_msg icr_msg;
	icr_msg.msg_type = init_ctxt_request;
	icr_msg.ue_idx = ue_ctxt->getContextID();
	icr_msg.enb_fd = ue_ctxt->getEnbFd();
	icr_msg.enb_s1ap_ue_id = ue_ctxt->getS1apEnbUeId();

	icr_msg.exg_max_dl_bitrate = (ue_ctxt->getAmbr().ambr_m).max_requested_bw_dl;
	icr_msg.exg_max_ul_bitrate = (ue_ctxt->getAmbr().ambr_m).max_requested_bw_ul;
	BearerContext* bearerCtxt = sessionCtxt->getBearerContext();
	if( bearerCtxt == NULL )
	{
		log_msg(LOG_ERROR, "Failed to retrive Bearer context for UE IDx %d\n", cb.getCBIndex());

		return ActStatus::HALT;
	}

	icr_msg.bearer_id = bearerCtxt->getBearerId();

	memcpy(&(icr_msg.gtp_teid), &(bearerCtxt->getS1uSgwUserFteid().fteid_m), sizeof(struct fteid));
	memcpy(&(icr_msg.sec_key), &((ue_ctxt->getUeSecInfo().secinfo_m).kenb_key),
			KENB_SIZE);	
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
	nas.elements = (nas_pdu_elements *) calloc(ICS_REQ_NO_OF_NAS_IES,sizeof(nas_pdu_elements));
	nas.elements[0].pduElement.attach_res = 2; /* EPS Only */
	nas.elements[1].pduElement.t3412 = 224; 
	nas.elements[2].pduElement.tailist.type = 1;
	nas.elements[2].pduElement.tailist.num_of_elements = 0;
   /* S1AP TAI mcc 123, mnc 456 : 214365 */
   /* NAS TAI mcc 123, mnc 456 : 216354 */
	memcpy(&(nas.elements[2].pduElement.tailist.partial_list[0]),
			&(ue_ctxt->getTai().tai_m), sizeof(struct TAI));
	
	/* Fill ESM info */
	nas.elements[3].pduElement.esm_msg.eps_bearer_id = 5; /* TODO: revisit */
	nas.elements[3].pduElement.esm_msg.proto_discriminator = EPSSessionManagementMessage;
	nas.elements[3].pduElement.esm_msg.procedure_trans_identity = sessionCtxt->getPti();
	nas.elements[3].pduElement.esm_msg.session_management_msgs = ESM_MSG_ACTV_DEF_BEAR__CTX_REQ;
	nas.elements[3].pduElement.esm_msg.eps_qos = 9;
	nas.elements[3].pduElement.esm_msg.apn.len = sessionCtxt->getAccessPtName().apnname_m.len;
	memcpy(nas.elements[3].pduElement.esm_msg.apn.val, sessionCtxt->getAccessPtName().apnname_m.val, sessionCtxt->getAccessPtName().apnname_m.len);
    log_msg(LOG_DEBUG, "ESM apn length = %d \n",nas.elements[3].pduElement.esm_msg.apn.len);

	log_msg(LOG_DEBUG, "PCO length %d\n", procedure_p->getPcoOptionsLen());
	nas.elements[3].pduElement.esm_msg.pco_opt.pco_length = procedure_p->getPcoOptionsLen();
	memcpy(nas.elements[3].pduElement.esm_msg.pco_opt.pco_options, procedure_p->getPcoOptions(), nas.elements[3].pduElement.pco_opt.pco_length);

	nas.elements[3].pduElement.esm_msg.pdn_addr.type = 1;
	nas.elements[3].pduElement.esm_msg.pdn_addr.ipv4 = htonl(sessionCtxt->getPdnAddr().paa_m.ip_type.ipv4.s_addr);
	nas.elements[3].pduElement.esm_msg.linked_ti.flag = 0;
	nas.elements[3].pduElement.esm_msg.linked_ti.val = 0;
	MmeNasUtils::get_negotiated_qos_value(&nas.elements[3].pduElement.esm_msg.negotiated_qos);

    /* Send the allocated GUTI to UE  */
	nas.elements[4].pduElement.mi_guti.odd_even_indication = 0;
	nas.elements[4].pduElement.mi_guti.id_type = 6;

	memcpy(&(nas.elements[4].pduElement.mi_guti.plmn_id),
			&(ue_ctxt->getTai().tai_m.plmn_id), 3); // ajaymerge - sizeof(struct PLMN)); dont use size..it has extra fields 
	nas.elements[4].pduElement.mi_guti.mme_grp_id = htons(mme_cfg->mme_group_id);
	nas.elements[4].pduElement.mi_guti.mme_code = mme_cfg->mme_code;
	nas.elements[4].pduElement.mi_guti.m_TMSI = htonl(ue_ctxt->getMTmsi());

	MmeNasUtils::encode_nas_msg(&nasBuffer, &nas, ue_ctxt->getUeSecInfo());
	memcpy(&icr_msg.nasMsgBuf[0], &nasBuffer.buf[0], nasBuffer.pos);
	icr_msg.nasMsgSize = nasBuffer.pos;
	log_msg(LOG_DEBUG, "nas message size %d \n",icr_msg.nasMsgSize);
	free(nas.elements);

	cmn::ipc::IpcAddress destAddr;
	destAddr.u32 = TipcServiceInstance::s1apAppInstanceNum_c;
	
	MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));   
	mmeIpcIf.dispatchIpcMsg((char *) &icr_msg, sizeof(icr_msg), destAddr);
	
	ProcedureStats::num_of_init_ctxt_req_to_ue_sent ++;
	log_msg(LOG_DEBUG, "Leaving send_init_ctxt_req_to_ue_v \n");
		
	return ActStatus::PROCEED;
}

ActStatus ActionHandlers::process_init_ctxt_resp(SM::ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside process_init_ctxt_resp \n");

	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	MmeAttachProcedureCtxt *procCtxt = dynamic_cast<MmeAttachProcedureCtxt*>(cb.getTempDataBlock());

	if (ue_ctxt == NULL || procCtxt == NULL)
	{
		log_msg(LOG_DEBUG, "process_init_ctxt_resp: ue context or procedure ctxt is NULL \n");
		return ActStatus::HALT;
	}

	SessionContext* sessionCtxt = ue_ctxt->getSessionContext();
	if (sessionCtxt == NULL)
	{
		log_msg(LOG_DEBUG, "process_init_ctxt_resp: session ctxt is NULL \n");
		return ActStatus::HALT;
	}
	
	MsgBuffer* msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());

	if (msgBuf == NULL)
		return ActStatus::HALT;

	const s1_incoming_msg_data_t* s1_msg_data = static_cast<const s1_incoming_msg_data_t*>(msgBuf->getDataPointer());
	const struct initctx_resp_Q_msg &ics_res =s1_msg_data->msg_data.initctx_resp_Q_msg_m;
	
	fteid S1uEnbUserFteid;
	S1uEnbUserFteid.header.iface_type = 0;
	S1uEnbUserFteid.header.v4 = 1;
	S1uEnbUserFteid.header.teid_gre = ics_res.gtp_teid;
	S1uEnbUserFteid.ip.ipv4 = *(struct in_addr*)&ics_res.transp_layer_addr;
	
	BearerContext* bearerCtxt = sessionCtxt->getBearerContext();
	if (bearerCtxt == NULL)
	{
		log_msg(LOG_DEBUG, "process_init_ctxt_resp: bearer ctxt is NULL \n");
		return ActStatus::HALT;
	}

	bearerCtxt->setS1uEnbUserFteid(Fteid(S1uEnbUserFteid));

	ProcedureStats::num_of_processed_init_ctxt_resp ++;
	log_msg(LOG_DEBUG, "Leaving process_init_ctxt_resp \n");
	
	return ActStatus::PROCEED;
}

ActStatus ActionHandlers::send_mb_req_to_sgw(SM::ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside send_mb_req_to_sgw \n");

	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	if (ue_ctxt == NULL)
	{
		log_msg(LOG_DEBUG, "send_mb_req_to_sgw: ue context or procedure ctxt is NULL \n");
		return ActStatus::HALT;
	}

	SessionContext* sessionCtxt = ue_ctxt->getSessionContext();
	if (sessionCtxt == NULL)
	{
		log_msg(LOG_DEBUG, "send_mb_req_to_sgw: session ctxt is NULL \n");
		return ActStatus::HALT;
	}
	
	struct MB_Q_msg mb_msg;
	mb_msg.msg_type = modify_bearer_request;
	mb_msg.ue_idx = ue_ctxt->getContextID();
	
	memset(mb_msg.indication, 0, S11_MB_INDICATION_FLAG_SIZE); /*TODO : future*/
	BearerContext* bearerCtxt = sessionCtxt->getBearerContext();
	if (bearerCtxt == NULL)
	{
		log_msg(LOG_DEBUG, "send_mb_req_to_sgw: bearer ctxt is NULL \n");
		return ActStatus::HALT;
	}

	mb_msg.bearer_id = bearerCtxt->getBearerId();

	memcpy(&(mb_msg.s11_sgw_c_fteid), &(sessionCtxt->getS11SgwCtrlFteid().fteid_m),
		sizeof(struct fteid));

	memcpy(&(mb_msg.s1u_enb_fteid), &(bearerCtxt->getS1uEnbUserFteid().fteid_m),
		sizeof(struct fteid));
	mb_msg.servingNetworkIePresent = false;
	mb_msg.userLocationInformationIePresent = false;

	cmn::ipc::IpcAddress destAddr;
	destAddr.u32 = TipcServiceInstance::s11AppInstanceNum_c;

	MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));   
	mmeIpcIf.dispatchIpcMsg((char *) &mb_msg, sizeof(mb_msg), destAddr);
		
	ProcedureStats::num_of_mb_req_to_sgw_sent ++;
	log_msg(LOG_DEBUG, "Leaving send_mb_req_to_sgw \n");
	
	return ActStatus::PROCEED;

}

ActStatus ActionHandlers::process_attach_cmp_from_ue(SM::ControlBlock& cb)
{	
	log_msg(LOG_DEBUG, "Inside handle_attach_cmp_from_ue \n");

	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	if (ue_ctxt == NULL)
	{
		log_msg(LOG_ERROR, "attach_done: ue context is NULL \n");
		return ActStatus::HALT;
	}

	//ue_ctxt->getUeSecInfo().increment_uplink_count();

	ProcedureStats::num_of_processed_attach_cmp_from_ue ++;
	log_msg(LOG_DEBUG, "Leaving handle_attach_cmp_from_ue \n");
	
	return ActStatus::PROCEED;
}

ActStatus ActionHandlers::process_mb_resp(SM::ControlBlock& cb)
{	
	log_msg(LOG_DEBUG, "Inside handle_mb_resp \n");
	
	ProcedureStats::num_of_processed_mb_resp ++;
	return ActStatus::PROCEED;
}

ActStatus ActionHandlers::check_and_send_emm_info(SM::ControlBlock& cb)
{
    log_msg(LOG_DEBUG, "Inside check_and_send_emm_info \n");

    UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
    if (ue_ctxt == NULL)
    {
        log_msg(LOG_DEBUG, "check_and_send_emm_info: ue context is NULL \n");
        return ActStatus::HALT;
    }
    
    MmeProcedureCtxt *procCtxt = dynamic_cast<MmeProcedureCtxt*>(cb.getTempDataBlock());
    if (procCtxt == NULL)
    {
	    log_msg(LOG_DEBUG, "check_and_send_emm_info: Procedure context is NULL\n");
	    return ActStatus::HALT;
    }

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
    	
        MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));
        mmeIpcIf.dispatchIpcMsg((char*) &temp, sizeof(temp), destAddr);

    	ProcedureStats::num_of_emm_info_sent++;
    }

    log_msg(LOG_DEBUG, "Leaving check_and_send_emm_info \n");

    return ActStatus::PROCEED;

}

ActStatus ActionHandlers::attach_done(SM::ControlBlock& cb)
{	
	log_msg(LOG_DEBUG, "Inside attach_done \n");
	
	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	if (ue_ctxt == NULL)
	{
		log_msg(LOG_ERROR, "attach_done: ue context is NULL \n");
		return ActStatus::HALT;
	}

	MmContext* mmCtxt = ue_ctxt->getMmContext();
	if (mmCtxt == NULL)
	{
		log_msg(LOG_ERROR, "attach_done: MMcontext is NULL \n");
		return ActStatus::HALT;
	}

	mmCtxt->setMmState(EpsAttached);
	
	MmeContextManagerUtils::deallocateProcedureCtxt(cb, attach_c);

	ProcedureStats::num_of_attach_done++;
	ProcedureStats::num_of_subscribers_attached ++;

	log_msg(LOG_DEBUG,"Leaving attach done\n");


	return ActStatus::PROCEED;
}

ActStatus ActionHandlers::send_attach_reject(ControlBlock& cb)
{
        UEContext* ueCtxt_p = static_cast<UEContext*>(cb.getPermDataBlock());
        if (ueCtxt_p == NULL)
        {
                log_msg(LOG_ERROR, " send_attach_reject: UE context is NULL %d\n",cb.getCBIndex());
                return ActStatus::HALT;
        }

        MmeAttachProcedureCtxt *procCtxt = dynamic_cast<MmeAttachProcedureCtxt*>(cb.getTempDataBlock());
        if (procCtxt == NULL)
        {
                log_msg(LOG_DEBUG, "send_attach_reject: Procedure context is NULL\n");
                return ActStatus::HALT;
        }
        
        struct commonRej_info attach_rej;

        attach_rej.msg_type = attach_reject;
        attach_rej.ue_idx = ueCtxt_p->getContextID();
        attach_rej.s1ap_enb_ue_id = ueCtxt_p->getS1apEnbUeId();
        attach_rej.enb_fd = ueCtxt_p->getEnbFd();
        attach_rej.cause = MmeCauseUtils::convertToNasEmmCause(procCtxt->getMmeErrorCause());
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
        
        MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));   
        mmeIpcIf.dispatchIpcMsg((char *) & attach_rej, sizeof(attach_rej), destAddr);

        ProcedureStats::num_of_attach_reject_sent ++;

        return ActStatus::PROCEED;
}

ActStatus ActionHandlers::abort_attach(ControlBlock& cb)
{
	MmeContextManagerUtils::deleteUEContext(cb.getCBIndex());
	return ActStatus::PROCEED;
}
