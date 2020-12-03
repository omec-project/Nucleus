
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
#include <gtpCauseTypes.h>
#include "common_proc_info.h"
#include <ipcTypes.h>
#include <tipcTypes.h>
#include <msgBuffer.h>
#include <interfaces/mmeIpcInterface.h>
#include <event.h>
#include <stateMachineEngine.h>
#include <utils/mmeContextManagerUtils.h>
#include "mmeNasUtils.h"
//#include <utils/mmeCauseUtils.h>
#include "mmeStatsPromClient.h"

using namespace mme;
using namespace SM;
using namespace cmn;
using namespace cmn::utils;

/***************************************
* Action handler :send_paging_req_to_ue 
***************************************/
ActStatus ActionHandlers::send_paging_req_to_ue(ControlBlock& cb)
{	
	log_msg(LOG_INFO,"Inside send_paging_req\n");

	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
	
	if (ue_ctxt == NULL)
	{
		log_msg(LOG_DEBUG, "send_paging_req: ue context is NULL\n");
		return ActStatus::HALT;
	}
	
	struct paging_req_Q_msg pag_req;
	pag_req.msg_type = paging_request;
	pag_req.ue_idx = ue_ctxt->getMTmsi();
	pag_req.enb_s1ap_ue_id = ue_ctxt->getS1apEnbUeId();
	pag_req.enb_fd = ue_ctxt->getEnbFd();
	pag_req.cn_domain = CN_DOMAIN_PS;

	const DigitRegister15& ueImsi = ue_ctxt->getImsi();
	ueImsi.convertToBcdArray( pag_req.IMSI );
	memcpy(&pag_req.tai, &(ue_ctxt->getTai().tai_m), sizeof(struct TAI));

	cmn::ipc::IpcAddress destAddr;
	destAddr.u32 = TipcServiceInstance::s1apAppInstanceNum_c;
	
    mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_S1AP_PAGING_REQUEST);
	MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));
	mmeIpcIf.dispatchIpcMsg((char *) &pag_req, sizeof(pag_req), destAddr);

	ProcedureStats::num_of_paging_request_sent++;

	log_msg(LOG_INFO,"Leaving send_paging_req\n");

	return ActStatus::PROCEED;
}

/***************************************
* Action handler : process_service_request
***************************************/
ActStatus ActionHandlers::process_service_request(ControlBlock& cb)
{
        log_msg(LOG_DEBUG, "Inside process_service_request \n");
	
	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());

	if (ue_ctxt == NULL )
	{
		log_msg(LOG_DEBUG, "process_service_request: ue ctxt is NULL \n");
		return ActStatus::HALT;
	}
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
	
	ue_ctxt->setS1apEnbUeId(msgData_p->s1ap_enb_ue_id);
	log_msg(LOG_DEBUG, "Leaving process_service_request \n");
	ProcedureStats::num_of_service_request_received ++;

	return ActStatus::PROCEED;
}

/***************************************
* Action handler : send_ddn_ack_to_sgw
***************************************/
ActStatus ActionHandlers::send_ddn_ack_to_sgw(ControlBlock& cb)
{
        log_msg(LOG_DEBUG, "Inside send_ddn_ack_to_sgw \n");
		
	UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
	MmeSvcReqProcedureCtxt* srPrcdCtxt_p = dynamic_cast<MmeSvcReqProcedureCtxt*>(cb.getTempDataBlock());
	
	if (ue_ctxt == NULL || srPrcdCtxt_p == NULL)
	{
	    log_msg(LOG_DEBUG, "send_ddn_ack_to_sgw: ue ctxt or MmeSvcReqProcedureCtxt is NULL \n");
	    return ActStatus::HALT;
	}

	auto& sessionCtxtContainer = ue_ctxt->getSessionContextContainer();
	if(sessionCtxtContainer.size() < 1)
	{
	    log_msg(LOG_DEBUG, "send_ddn_ack_to_sgw:Session context list is empty\n");
	    return ActStatus::HALT;
	}

	SessionContext* sess_p = sessionCtxtContainer.front();

	DDN_ACK_Q_msg ddn_ack;
	ddn_ack.msg_type = ddn_acknowledgement;
	ddn_ack.seq_no= srPrcdCtxt_p->getDdnSeqNo();
	memcpy(&(ddn_ack.s11_sgw_c_fteid), 	
               &(sess_p->getS11SgwCtrlFteid().fteid_m), sizeof(struct Fteid));
	ddn_ack.cause = GTPV2C_CAUSE_REQUEST_ACCEPTED;
	
	cmn::ipc::IpcAddress destAddr;
	destAddr.u32 = TipcServiceInstance::s11AppInstanceNum_c;

    mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_S11_DOWNLINK_DATA_NOTIFICATION_ACK);
	MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));
	mmeIpcIf.dispatchIpcMsg((char *) &ddn_ack, sizeof(ddn_ack), destAddr);
	
	log_msg(LOG_DEBUG, "Leaving send_ddn_ack_to_sgw \n");

	ProcedureStats::num_of_ddn_ack_sent ++;
	
	return ActStatus::PROCEED;
}


/***************************************************
* Action handler : send_init_ctxt_req_to_ue_svc_req
****************************************************/
ActStatus ActionHandlers::send_init_ctxt_req_to_ue_svc_req(ControlBlock& cb)
{
        log_msg(LOG_DEBUG, "Inside send_init_ctxt_req_to_ue_svc_req \n");

        UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
        if (ue_ctxt == NULL )
        {
                log_msg(LOG_DEBUG, "send_init_ctxt_req_to_ue_svc_req : ue context is NULL \n");
                return ActStatus::HALT;
        }

    	auto& sessionCtxtContainer = ue_ctxt->getSessionContextContainer();
    	if(sessionCtxtContainer.size() < 1)
    	{
    		log_msg(LOG_DEBUG, "send_init_ctxt_req_to_ue_svc_req : Session context list is empty\n");
    		return ActStatus::HALT;
    	}

    	SessionContext* sessionCtxt = sessionCtxtContainer.front();

        unsigned int nas_count = 0;
        E_UTRAN_sec_vector* secVect = const_cast<E_UTRAN_sec_vector*>(ue_ctxt->getAiaSecInfo().AiaSecInfo_mp);
        secinfo& secInfo = const_cast<secinfo&>(ue_ctxt->getUeSecInfo().secinfo_m);

        SecUtils::create_kenb_key(secVect->kasme.val, secInfo.kenb_key, nas_count);
    	secInfo.next_hop_chaining_count = 0;
    	memcpy(secInfo.next_hop_nh, secInfo.kenb_key, KENB_SIZE);
        ics_req_paging_Q_msg icr_msg;
        icr_msg.msg_type = ics_req_paging;
        icr_msg.ue_idx = ue_ctxt->getContextID();
        icr_msg.enb_fd = ue_ctxt->getEnbFd();
        icr_msg.enb_s1ap_ue_id = ue_ctxt->getS1apEnbUeId();

        icr_msg.ueag_max_ul_bitrate = (ue_ctxt->getAmbr().ambr_m).max_requested_bw_dl;
        icr_msg.ueag_max_dl_bitrate = (ue_ctxt->getAmbr().ambr_m).max_requested_bw_ul;

        auto &bearerCtxtCont = sessionCtxt->getBearerContextContainer();
	icr_msg.erab_su_list.count = bearerCtxtCont.size();

        uint8_t i = 0;

        for (auto &bearerCtxt: bearerCtxtCont)
        {
     	    if (bearerCtxt != NULL)
            {
        	icr_msg.erab_su_list.erab_su_item[i].e_RAB_ID =
        		bearerCtxt->getBearerId();
        	icr_msg.erab_su_list.erab_su_item[i].e_RAB_QoS_Params.qci =
        		bearerCtxt->getBearerQos().qci;
        	icr_msg.erab_su_list.erab_su_item[i].e_RAB_QoS_Params.arPrio.prioLevel =
        		bearerCtxt->getBearerQos().arp.prioLevel;
        	icr_msg.erab_su_list.erab_su_item[i].e_RAB_QoS_Params.arPrio.preEmptionCapab =
        		bearerCtxt->getBearerQos().arp.preEmptionCapab;
        	icr_msg.erab_su_list.erab_su_item[i].e_RAB_QoS_Params.arPrio.preEmptionVulnebility =
        		bearerCtxt->getBearerQos().arp.preEmptionVulnebility;
        	icr_msg.erab_su_list.erab_su_item[i].transportLayerAddress =
        		bearerCtxt->getS1uSgwUserFteid().fteid_m.ip.ipv4.s_addr;
        	icr_msg.erab_su_list.erab_su_item[i].gtp_teid =
        		bearerCtxt->getS1uSgwUserFteid().fteid_m.header.teid_gre;
        	i++;
            }
        }
        memcpy(&(icr_msg.sec_key), &((ue_ctxt->getUeSecInfo().secinfo_m).kenb_key), KENB_SIZE);

        //ue_ctxt->setdwnLnkSeqNo(icr_msg.dl_seq_no+1);

        cmn::ipc::IpcAddress destAddr;
        destAddr.u32 = TipcServiceInstance::s1apAppInstanceNum_c;

        mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_S1AP_ICS_REQUEST_PAGING);
        MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));
	    mmeIpcIf.dispatchIpcMsg((char *) &icr_msg, sizeof(icr_msg), destAddr);

        ProcedureStats::num_of_init_ctxt_req_to_ue_sent ++;
        log_msg(LOG_DEBUG, "Leaving send_init_ctxt_req_to_ue_svc_req_ \n");

    	return ActStatus::PROCEED;
}

/***************************************
* Action handler : process_init_ctxt_resp_svc_req
***************************************/
ActStatus ActionHandlers::process_init_ctxt_resp_svc_req(ControlBlock& cb)
{
    	log_msg(LOG_DEBUG, "Inside process_init_ctxt_resp_svc_req \n");

        UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
        MmeProcedureCtxt *procCtxt = dynamic_cast<MmeProcedureCtxt*>(cb.getTempDataBlock());

        if (ue_ctxt == NULL || procCtxt == NULL)
        {
                log_msg(LOG_DEBUG, "process_init_ctxt_resp_svc_req: ue context or procedure ctxt is NULL \n");
                return ActStatus::HALT;
        }

    	auto& sessionCtxtContainer = ue_ctxt->getSessionContextContainer();
    	if(sessionCtxtContainer.size() < 1)
    	{
    		log_msg(LOG_DEBUG, "process_init_ctxt_resp_svc_req:Session context list is empty\n");
    		return ActStatus::HALT;
    	}

    	SessionContext* sessionCtxt = sessionCtxtContainer.front();

        MsgBuffer* msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());

        if (msgBuf == NULL)
                return ActStatus::HALT;

        const s1_incoming_msg_data_t* s1_msg_data = static_cast<const s1_incoming_msg_data_t*>(msgBuf->getDataPointer());
        const struct initctx_resp_Q_msg &ics_res =s1_msg_data->msg_data.initctx_resp_Q_msg_m;

        auto &bearerCtxtCont = sessionCtxt->getBearerContextContainer();

        uint8_t i = 0;

        for (auto &bearerCtxt: bearerCtxtCont)
        {
            if ((bearerCtxt != NULL) && (i < ics_res.erab_setup_resp_list.count))
            {
                fteid S1uEnbUserFteid;
                S1uEnbUserFteid.header.iface_type = 0;
                S1uEnbUserFteid.header.v4 = 1;
                S1uEnbUserFteid.header.teid_gre = ics_res.erab_setup_resp_list.erab_su_res_item[i].gtp_teid;
                S1uEnbUserFteid.ip.ipv4 = *(struct in_addr*)&ics_res.erab_setup_resp_list.erab_su_res_item[i].transportLayerAddress;

                if (bearerCtxt->getBearerId() == ics_res.erab_setup_resp_list.erab_su_res_item[i].e_RAB_ID)
                {
                    bearerCtxt->setS1uEnbUserFteid(Fteid(S1uEnbUserFteid));
                }
                i++;
            }
        }

        ProcedureStats::num_of_processed_init_ctxt_resp ++;
        log_msg(LOG_DEBUG, "Leaving process_init_ctxt_resp_svc_req \n");

        return ActStatus::PROCEED;
}


/***************************************
* Action handler : send_mb_req_to_sgw_svc_req
***************************************/
ActStatus ActionHandlers::send_mb_req_to_sgw_svc_req(ControlBlock& cb)
{
    log_msg(LOG_DEBUG, "Inside send_mb_req_to_sgw_svc_req \n");

    UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
    if (ue_ctxt == NULL)
    {
        log_msg(LOG_INFO, "send_mb_req_to_sgw_svc_req: ue context is NULL \n");

        MmeContextManagerUtils::deleteUEContext(cb.getCBIndex());
        return ActStatus::HALT;
    }

    ActStatus actStatus = ActStatus::PROCEED;

    MmeProcedureCtxt *procCtxt =
            dynamic_cast<MmeProcedureCtxt*>(cb.getTempDataBlock());

    if (procCtxt != NULL)
    {
    	auto& sessionCtxtContainer = ue_ctxt->getSessionContextContainer();
    	if(sessionCtxtContainer.size() > 0)
    	{
    	    SessionContext* sessionCtxt = sessionCtxtContainer.front();

            //Support dedicated bearers
            auto& bearerCtxtCont = sessionCtxt->getBearerContextContainer();

            if (bearerCtxtCont.size() > 0)  // if bearers > 0
            {
                uint8_t i =0;
            	struct MB_Q_msg mb_msg;
                memset(&mb_msg, 0, sizeof (struct MB_Q_msg));

                mb_msg.msg_type = modify_bearer_request;
                mb_msg.ue_idx = ue_ctxt->getContextID();
                mb_msg.bearer_ctx_list.bearers_count = bearerCtxtCont.size();

                for (auto &bearerCtxt : bearerCtxtCont)
                {
                    if (bearerCtxt != NULL)
                    {
                        mb_msg.bearer_ctx_list.bearer_ctxt[i].eps_bearer_id =
                                bearerCtxt->getBearerId();

                        memcpy(&(mb_msg.s11_sgw_c_fteid),
                                &(sessionCtxt->getS11SgwCtrlFteid().fteid_m),
                                sizeof(struct fteid));

                        if (procCtxt->getCtxtType() == erabModInd_c)
                        {
                            MmeErabModIndProcedureCtxt *prcdCtxt =
                                    static_cast<MmeErabModIndProcedureCtxt*>(cb.getTempDataBlock());

                            if (prcdCtxt->getErabToBeModifiedListLen() > 0)
                            {
                                for (int j = 0;
                                        j < prcdCtxt->getErabToBeModifiedListLen(); j++)
                                {
                                    if (prcdCtxt->getErabToBeModifiedList()[j].e_RAB_ID
                                            == bearerCtxt->getBearerId())
                                    {
                                        fteid S1uEnbUserFteid;
                                        S1uEnbUserFteid.header.iface_type = 0;
                                        S1uEnbUserFteid.header.v4 = 1;
                                        S1uEnbUserFteid.header.teid_gre =
                                                prcdCtxt->getErabToBeModifiedList()[j].dl_gtp_teid;
                                        S1uEnbUserFteid.ip.ipv4.s_addr =
                                                prcdCtxt->getErabToBeModifiedList()[j].transportLayerAddress;

                                        bearerCtxt->setS1uEnbUserFteid(
                                                Fteid(S1uEnbUserFteid));
                                    }
                                }
                            }
                        }

                        memcpy(
                                &(mb_msg.bearer_ctx_list.bearer_ctxt[i].s1u_enb_fteid),
                                &(bearerCtxt->getS1uEnbUserFteid().fteid_m),
                                sizeof(struct fteid));

                        i++;
                    }
                }
                cmn::ipc::IpcAddress destAddr;
                destAddr.u32 = TipcServiceInstance::s11AppInstanceNum_c;

                mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_S11_MODIFY_BEARER_REQUEST);
                MmeIpcInterface &mmeIpcIf =
                        static_cast<MmeIpcInterface&>(compDb.getComponent(
                                MmeIpcInterfaceCompId));
                mmeIpcIf.dispatchIpcMsg((char*) &mb_msg, sizeof(mb_msg), destAddr);

                ProcedureStats::num_of_mb_req_to_sgw_sent++;
            }
            else
            {
                log_msg(LOG_INFO,
                        "send_mb_req_to_sgw_svc_req: bearer context is NULL \n");

                actStatus = ActStatus::ABORT;
            }
        }
        else
        {
            log_msg(LOG_INFO,
                    "send_mb_req_to_sgw_svc_req: session context is NULL \n");
            actStatus = ActStatus::ABORT;
        }
    }
    else
    {
        log_msg(LOG_INFO,
                "send_mb_req_to_sgw_svc_req: procedure context is NULL \n");
        actStatus = ActStatus::ABORT;
    }

    log_msg(LOG_DEBUG, "Leaving send_mb_req_to_sgw_svc_req \n");

    return actStatus;
}

/***************************************
 * Action handler : process_mb_resp_svc_req
 ***************************************/
ActStatus ActionHandlers::process_mb_resp_svc_req(ControlBlock &cb)
{
    log_msg(LOG_DEBUG, "Inside process_mb_resp_svc_req \n");

    ProcedureStats::num_of_processed_mb_resp++;

    UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
    if (ue_ctxt == NULL)
    {
        log_msg(LOG_INFO, "process_mb_resp_svc_req: ue context is NULL \n");

        MmeContextManagerUtils::deleteUEContext(cb.getCBIndex());
        return ActStatus::HALT;
    }

    MsgBuffer *msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
    VERIFY(msgBuf, return ActStatus::ABORT, "Invalid message buffer \n");
    VERIFY(msgBuf->getLength() >= sizeof(struct MB_resp_Q_msg), return ActStatus::ABORT, "Invalid MBResp message length \n");

    const struct MB_resp_Q_msg *mbr_info =
    		static_cast<const MB_resp_Q_msg*>(msgBuf->getDataPointer());

    ActStatus actStatus = ActStatus::PROCEED;

    MmeProcedureCtxt *procCtxt =
            dynamic_cast<MmeProcedureCtxt*>(cb.getTempDataBlock());

    if (procCtxt != NULL)
    {
        if (mbr_info->cause != GTPV2C_CAUSE_REQUEST_ACCEPTED)
        {
            log_msg(LOG_INFO, "process_mb_resp_svc_req: MB Response Failure\n");

            procCtxt->setMmeErrorCause(S11_MODIFY_BEARER_RESP_FAILURE);
            actStatus = ActStatus::ABORT;
        }
        else
        {
        	auto& sessionCtxtContainer = ue_ctxt->getSessionContextContainer();
        	if(sessionCtxtContainer.size() > 0)
        	{
        		SessionContext* sessionCtxt = sessionCtxtContainer.front();
                //TODO: Support dedicated bearers
                BearerContext *bearerCtxt = sessionCtxt->findBearerContextByBearerId(sessionCtxt->getLinkedBearerId());
                if (bearerCtxt != NULL)
                {
                    if (procCtxt->getCtxtType() == erabModInd_c)
                    {
                        MmeErabModIndProcedureCtxt *procCtxt =
                                static_cast<MmeErabModIndProcedureCtxt*>(cb.getTempDataBlock());

                        uint8_t eRabsModifiedList[1] =
                        { bearerCtxt->getBearerId() };
                        procCtxt->setErabModifiedList(eRabsModifiedList, 1);
                    }
                }
                else
                {
                    log_msg(LOG_INFO,
                            "send_mb_req_to_sgw_svc_req: bearer context is NULL \n");

                    actStatus = ActStatus::ABORT;
                }
            }
            else
            {
                log_msg(LOG_INFO,
                        "process_mb_resp_svc_req: session context is NULL \n");
                actStatus = ActStatus::ABORT;
            }
        }
    }
    else
    {
        log_msg(LOG_INFO,
                "process_mb_resp_svc_req: procedure context is NULL \n");
        actStatus = ActStatus::ABORT;
    }

    return actStatus;
}

/***************************************
* Action handler : send_service_reject
***************************************/
ActStatus ActionHandlers::send_service_reject(ControlBlock& cb)
{
  	log_msg(LOG_DEBUG, "Inside send_service_reject \n");
        
	UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
        if (ue_ctxt == NULL)
        {
                log_msg(LOG_DEBUG, "send_service_reject: ue context is NULL \n");
                return ActStatus::HALT;
        }

	MmeProcedureCtxt *procCtxt = dynamic_cast<MmeProcedureCtxt*>(cb.getTempDataBlock());
	if (procCtxt == NULL)
        {
                log_msg(LOG_DEBUG, "send_service_reject: procedure ctxt is NULL \n");
                return ActStatus::HALT;
        }

	struct commonRej_info service_rej;
	
	service_rej.msg_type = service_reject;
	service_rej.ue_idx = ue_ctxt->getContextID(); 
	service_rej.s1ap_enb_ue_id = ue_ctxt->getS1apEnbUeId();
	service_rej.enb_fd = ue_ctxt->getEnbFd();
	service_rej.cause = emmCause_ue_id_not_derived_by_network;

	struct Buffer nasBuffer;
	struct nasPDU nas = {0};
	const uint8_t num_nas_elements = 1;
	nas.elements = (nas_pdu_elements *) calloc(num_nas_elements, sizeof(nas_pdu_elements)); // TODO : should i use new ?
	nas.elements_len = num_nas_elements;
	nas.header.security_header_type = Plain;
	nas.header.proto_discriminator = EPSMobilityManagementMessages;
	nas.header.message_type = ServiceReject;
	nas.elements[0].pduElement.attach_res = 0x09;
	MmeNasUtils::encode_nas_msg(&nasBuffer, &nas, ue_ctxt->getUeSecInfo());
	memcpy(&service_rej.nasMsgBuf[0], &nasBuffer.buf[0], nasBuffer.pos);
	service_rej.nasMsgSize = nasBuffer.pos;
	free(nas.elements);

	cmn::ipc::IpcAddress destAddr;
    destAddr.u32 = TipcServiceInstance::s1apAppInstanceNum_c;

    mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_NAS_SERVICE_REJECT);
    MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));
    mmeIpcIf.dispatchIpcMsg((char *) &service_rej, sizeof(service_rej), destAddr);

	ProcedureStats::num_of_service_reject_sent ++;

        return ActStatus::PROCEED;

}

/***************************************
* Action handler : abort_service_req_procedure
***************************************/
ActStatus ActionHandlers::abort_service_req_procedure(ControlBlock& cb)
{
    log_msg(LOG_DEBUG,"abort_service_req_procedure : Entry \n");

    MmeSvcReqProcedureCtxt *procedure_p =
            static_cast<MmeSvcReqProcedureCtxt*>(cb.getTempDataBlock());
    if (procedure_p != NULL)
    {
        MmeContextManagerUtils::deallocateProcedureCtxt(cb, procedure_p);
    }

    // Fire a page failure event, so that interested procedures
    // can take appropriate actions.
    SM::Event evt(PAGING_FAILURE, NULL);
    cb.qInternalEvent(evt);

    log_msg(LOG_DEBUG,"abort_service_req_procedure : Exit \n");

    return ActStatus::PROCEED;
}
/***************************************
* Action handler : service_request_complete
***************************************/
ActStatus ActionHandlers::service_request_complete(ControlBlock& cb)
{
    mmeStats::Instance()->increment(mmeStatsCounter::MME_PROCEDURES_SERVICE_REQUEST_PROC_SUCCESS);

    MmeSvcReqProcedureCtxt *procedure_p =
            static_cast<MmeSvcReqProcedureCtxt*>(cb.getTempDataBlock());
    if (procedure_p != NULL)
    {
        if (procedure_p->checkPagingTriggerBit(pgwInit_c))
        {
            SM::Event evt(PAGING_COMPLETE, NULL);
            cb.qInternalEvent(evt);
        }
    }
	
    MmeContextManagerUtils::deallocateProcedureCtxt(cb, procedure_p);

    return ActStatus::PROCEED;
}
