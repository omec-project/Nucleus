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
#include <locale>
#include <memory.h>
#include <signal.h>

#include "epc/epctools.h"
#include "epc/etevent.h"
#include "epc/esocket.h"
#include "epc/einternal.h"

#include "epc/emgmt.h"
#include "epc/etimerpool.h"

#include "epc/epcdns.h"
#include "epc/dnscache.h"
#include "mme_app.h"

#include "controlBlock.h"
#include "msgType.h"
#include "contextManager/subsDataGroupManager.h"
#include "contextManager/dataBlocks.h"
#include "procedureStats.h"

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
#include "gtpCauseTypes.h"



#define MCC_MNC_LEN 4
#define LB_HB_LEN 8

using namespace SM;
using namespace mme;
using namespace cmn;
using namespace cmn::utils;

cmn::ipc::IpcAddress destAddr_dns;
struct CS_Q_msg cs_msg;
extern mme_config g_mme_cfg;
extern MmeIpcInterface* mmeIpcIf_g;
Void SetDNSConfiguration()
{
	DNS::Cache::setRefreshConcurrent( g_mme_cfg.dns_config.concurrent);
	DNS::Cache::setRefreshPercent( g_mme_cfg.dns_config.percentage);
	DNS::Cache::setRefreshInterval( g_mme_cfg.dns_config.interval_seconds );
	DNS::Cache::getInstance().addNamedServer(g_mme_cfg.dns_config.dns1_ip);
	DNS::Cache::getInstance().applyNamedServers();

}

	template <class T>
std::string hexFormatWithoutCommas(T value, int width = sizeof(T)*2)
{
	struct Numpunct : public std::numpunct<char>
	{
		protected:
			virtual char do_thousands_sep() const { return ' '; }
			virtual std::string do_grouping() const { return ""; }
	};
	std::stringstream ss;
	ss.imbue({std::locale(), new Numpunct});
	ss << std::setfill('0') << std::setw(width) << std::hex << value;
	return ss.str();
}



void Ipv4_gateway_ip( EPCDNS::StringVector &ipv4_ip, unsigned long * sgw_ip)
{
	EPCDNS::StringVector::const_iterator it = ipv4_ip.begin();
	std::cout<<*it <<"\n";
	char * result = (char*)ipv4_ip[0].c_str();
	std::cout << result << "\n";
	unsigned long tmp = htonl(inet_addr(result));
	memcpy(sgw_ip,&tmp, sizeof(unsigned long));
	std::cout << *sgw_ip << "\n";
}
void Extract_IPs( EPCDNS::NodeSelectorResultList &at, unsigned long  *sgw_ip)
{	
	EPCDNS::NodeSelectorResultList::const_iterator it = at.begin();
	Ipv4_gateway_ip((*it)->getIPv4Hosts(), sgw_ip);
}

void process_dns_req(void *node_obj, unsigned long *sgw_ip)
{
	EPCDNS::NodeSelector *ns = static_cast<EPCDNS::NodeSelector *>(node_obj);
	ns->process();
	ns->dump();
	Extract_IPs(ns->getResults(),sgw_ip);
}

#if 0
extern "C" Void NodeSelector_test_callback(EPCDNS::NodeSelector &ns, cpVoid data)
{
	unsigned long sgw_ip;
	std::cout << "*********** Asynchronous Node Selector ***********" << std::endl;
	std::cout << "NodeSelector_test_callback() - data = 0x" << hexFormatWithoutCommas((ULongLong)data) << std::endl;	
	ns.dump();
	UEContext *ue_ctxt = static_cast<UEContext*>(data);

	Extract_IPs(ns.getResults(),&sgw_ip);
	ue_ctxt->setSgwIp(sgw_ip);
        cs_msg.sgw_ip =sgw_ip;
        std::cout <<"\n" << "value of sgw in UE context is "<< ue_ctxt->getSgwIp();
	mmeIpcIf_g->dispatchIpcMsg((char *) &cs_msg, sizeof(cs_msg), destAddr_dns);

        ProcedureStats::num_of_cs_req_to_sgw_sent ++;
        log_msg(LOG_DEBUG, "Leaving cs_req_to_sgw \n");




	std::cout <<"\n*************************************************" << std::endl;

}


#endif

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
	idReqMsg.ue_type = ID_IMSI;

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
	
	memset(s6a_req.imsi, '\0', sizeof(s6a_req.imsi));
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
    memset(&s6a_req, 0, sizeof(s6a_req));

	memset(s6a_req.imsi, '\0', sizeof(s6a_req.imsi));
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

	ue_ctxt->setDwnLnkSeqNo(0);

	E_UTRAN_sec_vector *secVect = const_cast<E_UTRAN_sec_vector*>(ue_ctxt->getAiaSecInfo().AiaSecInfo_mp);

	secinfo& secInfo = const_cast<secinfo&>(ue_ctxt->getUeSecInfo().secinfo_m);

	SecUtils::create_integrity_key(secVect->kasme.val, secInfo.int_key);

	memcpy(&(authreq.rand), &(secVect->rand.val), NAS_RAND_SIZE);
	memcpy(&(authreq.autn), &(secVect->autn.val), NAS_AUTN_SIZE);
	
	cmn::ipc::IpcAddress destAddr;
	destAddr.u32 = TipcServiceInstance::s1apAppInstanceNum_c;
	
	MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));   
	mmeIpcIf.dispatchIpcMsg((char *) &authreq, sizeof(authreq), destAddr);
	
	ProcedureStats::num_of_auth_req_to_ue_sent ++;
	log_msg(LOG_DEBUG, "Leaving auth_req_to_ue_v \n");
		
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
                SM::Event evt(AUTH_RESP_SUCCESS,NULL);
                controlBlk_p->addEventToProcQ(evt);

	}
	//TODO: XRES comparison
	#if 0
	log_msg(LOG_ERROR, "stage 3 processing memcmp - %d, %d, %d", &(ue_ctxt->getaiaSecInfo().AiaSecInfo_mp->xres.val),
                &(auth_resp->res.val),
                auth_resp->res.len);
	if(memcmp(&(ue_ctxt->getaiaSecInfo().AiaSecInfo_mp->xres.val),
		&(auth_resp->res.val),
		auth_resp->res.len) != 0) {
		log_msg(LOG_ERROR, "Invalid auth result received for UE %d",
			auth_resp->ue_idx);
		return E_FAIL;//report failure
	}
	#endif
	
	ProcedureStats::num_of_processed_auth_response ++;
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
	

ActStatus ActionHandlers::sec_mode_cmd_to_ue(SM::ControlBlock& cb)
{
	log_msg(LOG_DEBUG, "Inside sec_mode_cmd_to_ue \n");

	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	if (ue_ctxt == NULL)
	{
		log_msg(LOG_DEBUG, "sec_mode_cmd_to_ue: ue context is NULL \n");
		return ActStatus::HALT;
	}
	sec_mode_Q_msg sec_mode_msg;
	sec_mode_msg.msg_type  = sec_mode_command;
	sec_mode_msg.ue_idx = ue_ctxt->getContextID();
	sec_mode_msg.enb_fd = ue_ctxt->getEnbFd();
	sec_mode_msg.enb_s1ap_ue_id = ue_ctxt->getS1apEnbUeId();
	
	memcpy(&(sec_mode_msg.ue_network), &(ue_ctxt->getUeNetCapab().ue_net_capab_m),
		sizeof(struct UE_net_capab));

	memcpy(&(sec_mode_msg.ms_net_capab), &(ue_ctxt->getMsNetCapab().ms_net_capab_m),
                sizeof(struct MS_net_capab));

	memcpy(&(sec_mode_msg.key), &(ue_ctxt->getAiaSecInfo().AiaSecInfo_mp->kasme),
			sizeof(struct KASME));

	memcpy(&(sec_mode_msg.int_key), &(ue_ctxt->getUeSecInfo().secinfo_m.int_key),
			NAS_INT_KEY_SIZE);

	sec_mode_msg.dl_seq_no = ue_ctxt->getDwnLnkSeqNo();
	ue_ctxt->setDwnLnkSeqNo(sec_mode_msg.dl_seq_no + 1);

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

	ue_ctxt->setUpLnkSeqNo(0);

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
	esmreq.pti = sessionCtxt->getPti();
	esmreq.dl_seq_no = ue_ctxt->getDwnLnkSeqNo();
	memcpy(&(esmreq.int_key), &((ue_ctxt->getUeSecInfo().secinfo_m).int_key),
			NAS_INT_KEY_SIZE);
	ue_ctxt->setDwnLnkSeqNo(esmreq.dl_seq_no+1);

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
	ue_ctxt->setUpLnkSeqNo(ue_ctxt->getUpLnkSeqNo()+1);

	ProcedureStats::num_of_handled_esm_info_resp++;
	return ActStatus::PROCEED;
}

ActStatus ActionHandlers::cs_req_to_sgw(SM::ControlBlock& cb)
{
	uint8_t plmn_id[3] = {0};

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
	// TODO: ApnSelection
	// Set the subscribed apn to selected apn for now
	sessionCtxt->setAccessPtName(apnName);
	memcpy(&(cs_msg.selected_apn), &(apnName.apnname_m), sizeof(struct apn_name));
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

	//cmn::ipc::IpcAddress destAddr;
	destAddr_dns.u32 = TipcServiceInstance::s11AppInstanceNum_c;
	if(g_mme_cfg.dns_config.dns_flag == 1)	
	{

		SetDNSConfiguration();
		/* Query DNS based on lb and hb of tac */
		char lb[LB_HB_LEN] = {0};
		char hb[LB_HB_LEN] = {0};
		char mnc[MCC_MNC_LEN] = {0};
		char mcc[MCC_MNC_LEN] = {0};
		short tac=0;
		unsigned long sgw_ip = 0;
		char mccDigit1 =  ue_ctxt->getTai().tai_m.plmn_id.idx[0] & 0x0F;
		char mccDigit2 = (ue_ctxt->getTai().tai_m.plmn_id.idx[0] & 0xF0) >> 4;
		char mccDigit3 = ue_ctxt->getTai().tai_m.plmn_id.idx[1] & 0x0F;
		char mncDigit1 = ue_ctxt->getTai().tai_m.plmn_id.idx[2] & 0x0F;
		char mncDigit2 = (ue_ctxt->getTai().tai_m.plmn_id.idx[2] & 0xF0) >> 4;
		char mncDigit3 = (ue_ctxt->getTai().tai_m.plmn_id.idx[1] & 0xF0) >> 4;



		if (mncDigit3 == 15)
			snprintf(mnc, MCC_MNC_LEN, "%u%u", mncDigit1,mncDigit2);
		else
			snprintf(mnc, MCC_MNC_LEN, "%u%u%u", mncDigit1,mncDigit2,mncDigit3);


		snprintf(mcc,MCC_MNC_LEN, "%u%u%u", mccDigit1, mccDigit2, mccDigit3);

		log_msg(LOG_ERROR,"%u %u  %u %u %u %u mcc and mnc digits\n", mccDigit1, mccDigit2, mccDigit2, mncDigit1, mncDigit2, mncDigit3);
		log_msg(LOG_ERROR, "%s %s: mnc and mcc\n", mnc,mcc);
		memcpy(plmn_id, ue_ctxt->getTai().tai_m.plmn_id.idx, 3);
		log_msg(LOG_ERROR, "%s %s: plmn_id\n", plmn_id,ue_ctxt->getTai().tai_m.plmn_id.idx);
		tac = ntohs(ue_ctxt->getTai().tai_m.tac);
		log_msg(LOG_ERROR, "%d %d tac value* \n",tac, ue_ctxt->getTai().tai_m.tac);

		if ( tac != 1) {
			log_msg(LOG_DEBUG, "Could not get SGW-U list using DNS query. TAC missing in CSR.\n");
			return ActStatus::HALT;
		}

		sprintf(lb, "%u", ue_ctxt->getTai().tai_m.tac & 0xFF);
		sprintf(hb, "%u", (ue_ctxt->getTai().tai_m.tac >> 8) & 0xFF);	
		log_msg(LOG_ERROR, "%s %s lb and hb value is\n", lb,hb);


		EPCDNS::SGWNodeSelector s2(lb, hb,mnc,mcc);
		s2.setNamedServerID(DNS::NS_DEFAULT);
		s2.addDesiredProtocol( EPCDNS::SGWAppProtocolEnum::sgw_x_s11 );
		s2.process();
		s2.dump();
                Extract_IPs(s2.getResults(), &sgw_ip);
	
		std::cout << " gatway is "<<sgw_ip; 
		ue_ctxt->setSgwCtrlPIP(sgw_ip);
		cs_msg.sgw_ip =sgw_ip;
		std::cout <<"\n" << "value of sgw in UE context is "<< ue_ctxt->getSgwCtrlPIP();
		//TODO Async call needs to write here	
		 //s2.process(ue_ctxt, NodeSelector_test_callback);
	}
	
	
	MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));   
	mmeIpcIf.dispatchIpcMsg((char *) &cs_msg, sizeof(cs_msg), destAddr_dns);

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

	icr_msg.dl_seq_no = ue_ctxt->getDwnLnkSeqNo();
	memcpy(&(icr_msg.tai), &(ue_ctxt->getTai().tai_m), sizeof(struct TAI));
	memcpy(&(icr_msg.gtp_teid), &(bearerCtxt->getS1uSgwUserFteid().fteid_m), sizeof(struct fteid));
	memcpy(&(icr_msg.apn), &(sessionCtxt->getAccessPtName().apnname_m), sizeof(struct apn_name));
	memcpy(&(icr_msg.pdn_addr), &(sessionCtxt->getPdnAddr().paa_m), sizeof(struct PAA));
	memcpy(&(icr_msg.int_key), &((ue_ctxt->getUeSecInfo().secinfo_m).int_key),
			NAS_INT_KEY_SIZE);
	memcpy(&(icr_msg.sec_key), &((ue_ctxt->getUeSecInfo().secinfo_m).kenb_key),
			KENB_SIZE);	
	icr_msg.pti = sessionCtxt->getPti();
        icr_msg.m_tmsi = ue_ctxt->getMTmsi();
	ue_ctxt->setDwnLnkSeqNo(icr_msg.dl_seq_no+1);

	icr_msg.pco_length = procedure_p->getPcoOptionsLen();
	if(procedure_p->getPcoOptionsLen() > 0)
		memcpy(&(icr_msg.pco_options[0]), procedure_p->getPcoOptions(), icr_msg.pco_length);

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

	ue_ctxt->setUpLnkSeqNo(ue_ctxt->getUpLnkSeqNo()+1);

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
    	temp.dl_seq_no = ue_ctxt->getDwnLnkSeqNo();
    	ue_ctxt->setDwnLnkSeqNo(temp.dl_seq_no+1);
    	/*Logically MME should have TAC database. and based on TAC
     	* MME can send different name. For now we are sending Aether for
     	* all TACs
     	*/
    	strcpy(temp.short_network_name, "Aether");
    	strcpy(temp.full_network_name, "Aether");
    	memcpy(&(temp.int_key), &((ue_ctxt->getUeSecInfo().secinfo_m).int_key),
    	NAS_INT_KEY_SIZE);

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
