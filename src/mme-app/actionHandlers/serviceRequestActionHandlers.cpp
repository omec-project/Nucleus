
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
#include <utils/mmeCauseUtils.h>
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
    log_msg(LOG_INFO, "Inside send_paging_req");

    UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
    VERIFY_UE(cb, ue_ctxt, "Invalid UE");

    struct paging_req_Q_msg pag_req;
    pag_req.msg_type = paging_request;
    pag_req.ue_idx = ue_ctxt->getMTmsi();
    pag_req.enb_s1ap_ue_id = ue_ctxt->getS1apEnbUeId();
    pag_req.enb_fd = ue_ctxt->getEnbFd();
    pag_req.cn_domain = CN_DOMAIN_PS;

    const DigitRegister15& ueImsi = ue_ctxt->getImsi();
    ueImsi.convertToBcdArray(pag_req.IMSI);
    memcpy(&pag_req.tai, &(ue_ctxt->getTai().tai_m), sizeof(struct TAI));

    cmn::ipc::IpcAddress destAddr;
    destAddr.u32 = TipcServiceInstance::s1apAppInstanceNum_c;

    mmeStats::Instance()->increment(
            mmeStatsCounter::MME_MSG_TX_S1AP_PAGING_REQUEST);
    MmeIpcInterface &mmeIpcIf =
            static_cast<MmeIpcInterface&>(compDb.getComponent(
                    MmeIpcInterfaceCompId));
    mmeIpcIf.dispatchIpcMsg((char *) &pag_req, sizeof(pag_req), destAddr);

    ProcedureStats::num_of_paging_request_sent++;

    log_msg(LOG_INFO, "Leaving send_paging_req");

    return ActStatus::PROCEED;
}

/***************************************
* Action handler : process_service_request
***************************************/
ActStatus ActionHandlers::process_service_request(ControlBlock& cb)
{
    log_msg(LOG_DEBUG, "Inside process_service_request ");
	
	UEContext *ue_ctxt = dynamic_cast<UEContext*>(cb.getPermDataBlock());
    VERIFY_UE(cb, ue_ctxt, "Invalid UE");

    MmContext *mmCtxt = ue_ctxt->getMmContext();
    VERIFY_UE(cb, mmCtxt, "Invalid MM Context");

    MmeProcedureCtxt *procCtxt =
            dynamic_cast<MmeProcedureCtxt*>(cb.getTempDataBlock());
    VERIFY(procCtxt, return ActStatus::ABORT, "Procedure Context is NULL ");


    MsgBuffer* msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
    VERIFY(msgBuf,
            procCtxt->setMmeErrorCause(INVALID_MSG_BUFFER); return ActStatus::ABORT,
            "process_service_request : Invalid message buffer ");


    const service_req_Q_msg_t* msgData_p =
           static_cast<const service_req_Q_msg_t*>(msgBuf->getDataPointer());

    VERIFY(msgData_p,
            procCtxt->setMmeErrorCause(INVALID_DATA_BUFFER); return ActStatus::ABORT,
            "process_service_request : Invalid data buffer ");

    mmCtxt->setEcmState(ecmConnected_c);
	ue_ctxt->setS1apEnbUeId(msgData_p->header.s1ap_enb_ue_id);
	log_msg(LOG_DEBUG, "Leaving process_service_request ");
	ProcedureStats::num_of_service_request_received ++;

	return ActStatus::PROCEED;
}

/***************************************
* Action handler : send_ddn_ack_to_sgw
***************************************/
ActStatus ActionHandlers::send_ddn_ack_to_sgw(ControlBlock& cb)
{
    log_msg(LOG_DEBUG, "Inside send_ddn_ack_to_sgw ");

    UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
    VERIFY_UE(cb, ue_ctxt, "Invalid UE");

    MmeSvcReqProcedureCtxt* srPrcdCtxt_p =
            dynamic_cast<MmeSvcReqProcedureCtxt*>(cb.getTempDataBlock());
    VERIFY(srPrcdCtxt_p, return ActStatus::ABORT,
            "Procedure Context is NULL ");

    auto& sessionCtxtContainer = ue_ctxt->getSessionContextContainer();
    VERIFY(sessionCtxtContainer.size() > 0,
            srPrcdCtxt_p->setMmeErrorCause(SESSION_CONTAINER_EMPTY); return ActStatus::ABORT,
            "Sessions Container is empty");

    SessionContext* sess_p = sessionCtxtContainer.front();
    VERIFY(sess_p,
            srPrcdCtxt_p->setMmeErrorCause(SESSION_CONTEXT_NOT_FOUND); return ActStatus::ABORT,
            "Session Context is NULL ");

    DDN_ACK_Q_msg ddn_ack;
    ddn_ack.msg_type = ddn_acknowledgement;
    ddn_ack.seq_no = srPrcdCtxt_p->getDdnSeqNo();
    memcpy(&(ddn_ack.s11_sgw_c_fteid), &(sess_p->getS11SgwCtrlFteid().fteid_m),
            sizeof(struct Fteid));
    ddn_ack.cause = GTPV2C_CAUSE_REQUEST_ACCEPTED;

    cmn::ipc::IpcAddress destAddr;
    destAddr.u32 = TipcServiceInstance::s11AppInstanceNum_c;

    mmeStats::Instance()->increment(
            mmeStatsCounter::MME_MSG_TX_S11_DOWNLINK_DATA_NOTIFICATION_ACK);
    MmeIpcInterface &mmeIpcIf =
            static_cast<MmeIpcInterface&>(compDb.getComponent(
                    MmeIpcInterfaceCompId));
    mmeIpcIf.dispatchIpcMsg((char *) &ddn_ack, sizeof(ddn_ack), destAddr);

    log_msg(LOG_DEBUG, "Leaving send_ddn_ack_to_sgw ");

    ProcedureStats::num_of_ddn_ack_sent++;

    return ActStatus::PROCEED;
}


/***************************************************
* Action handler : send_init_ctxt_req_to_ue_svc_req
****************************************************/
ActStatus ActionHandlers::send_init_ctxt_req_to_ue_svc_req(ControlBlock& cb)
{
    log_msg(LOG_DEBUG, "Inside send_init_ctxt_req_to_ue_svc_req ");

    UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
    VERIFY_UE(cb, ue_ctxt, "Invalid UE");

    MmeProcedureCtxt *procCtxt =
            dynamic_cast<MmeProcedureCtxt*>(cb.getTempDataBlock());
    VERIFY(procCtxt, return ActStatus::ABORT, "Procedure Context is NULL ");

    auto& sessionCtxtContainer = ue_ctxt->getSessionContextContainer();
    VERIFY(sessionCtxtContainer.size() > 0,
            procCtxt->setMmeErrorCause(SESSION_CONTAINER_EMPTY); return ActStatus::ABORT,
            "Sessions Container is empty");

    SessionContext *sessionCtxt = sessionCtxtContainer.front();
    VERIFY(sessionCtxt,
            procCtxt->setMmeErrorCause(SESSION_CONTEXT_NOT_FOUND); return ActStatus::ABORT,
            "Session Context is NULL ");

    unsigned int nas_count = 0;
    E_UTRAN_sec_vector* secVect =
            const_cast<E_UTRAN_sec_vector*>(ue_ctxt->getAiaSecInfo().AiaSecInfo_mp);
    secinfo& secInfo = const_cast<secinfo&>(ue_ctxt->getUeSecInfo().secinfo_m);

    SecUtils::create_kenb_key(secVect->kasme.val, secInfo.kenb_key, nas_count);

    unsigned char nh[SECURITY_KEY_SIZE] = { 0 };
    secInfo.next_hop_chaining_count = 1;
    SecUtils::create_nh_key(secVect->kasme.val, nh, secInfo.kenb_key);
    memcpy(secInfo.next_hop_nh , nh, KENB_SIZE);

    ics_req_paging_Q_msg icr_msg;
    icr_msg.msg_type = ics_req_paging;
    icr_msg.ue_idx = ue_ctxt->getContextID();
    icr_msg.enb_fd = ue_ctxt->getEnbFd();
    icr_msg.enb_s1ap_ue_id = ue_ctxt->getS1apEnbUeId();

    icr_msg.ueag_max_ul_bitrate =
            (ue_ctxt->getAmbr().ambr_m).max_requested_bw_dl;
    icr_msg.ueag_max_dl_bitrate =
            (ue_ctxt->getAmbr().ambr_m).max_requested_bw_ul;

    auto &bearerCtxtCont = sessionCtxt->getBearerContextContainer();
    VERIFY(bearerCtxtCont.size() > 0,
            procCtxt->setMmeErrorCause(BEARER_CONTAINER_EMPTY); return ActStatus::ABORT,
            "Bearers Container is empty");

    icr_msg.erab_su_list.count = bearerCtxtCont.size();

    uint8_t i = 0;

    for (auto &bearerCtxt : bearerCtxtCont)
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
    memcpy(&(icr_msg.sec_key), &((ue_ctxt->getUeSecInfo().secinfo_m).kenb_key),
            KENB_SIZE);

    //ue_ctxt->setdwnLnkSeqNo(icr_msg.dl_seq_no+1);

    cmn::ipc::IpcAddress destAddr;
    destAddr.u32 = TipcServiceInstance::s1apAppInstanceNum_c;

    mmeStats::Instance()->increment(
            mmeStatsCounter::MME_MSG_TX_S1AP_ICS_REQUEST_PAGING);
    MmeIpcInterface &mmeIpcIf =
            static_cast<MmeIpcInterface&>(compDb.getComponent(
                    MmeIpcInterfaceCompId));
    mmeIpcIf.dispatchIpcMsg((char *) &icr_msg, sizeof(icr_msg), destAddr);

    ProcedureStats::num_of_init_ctxt_req_to_ue_sent++;
    log_msg(LOG_DEBUG, "Leaving send_init_ctxt_req_to_ue_svc_req_ ");

    return ActStatus::PROCEED;
}

/***************************************
* Action handler : process_init_ctxt_resp_svc_req
***************************************/
ActStatus ActionHandlers::process_init_ctxt_resp_svc_req(ControlBlock& cb)
{
    log_msg(LOG_DEBUG, "Inside process_init_ctxt_resp_svc_req ");

    UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
    VERIFY_UE(cb, ue_ctxt, "Invalid UE");

    MmeProcedureCtxt *procCtxt =
            dynamic_cast<MmeProcedureCtxt*>(cb.getTempDataBlock());
    VERIFY(procCtxt, return ActStatus::ABORT, "Procedure Context is NULL ");

    auto &sessionCtxtContainer = ue_ctxt->getSessionContextContainer();
    VERIFY(sessionCtxtContainer.size() > 0,
            procCtxt->setMmeErrorCause(SESSION_CONTAINER_EMPTY); return ActStatus::ABORT,
            "Sessions Container is empty");

    SessionContext *sessionCtxt = sessionCtxtContainer.front();
    VERIFY(sessionCtxt,
            procCtxt->setMmeErrorCause(SESSION_CONTEXT_NOT_FOUND); return ActStatus::ABORT,
            "Session Context is NULL ");

    MsgBuffer *msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
    VERIFY(msgBuf,
            procCtxt->setMmeErrorCause(INVALID_MSG_BUFFER); return ActStatus::ABORT,
            "process_init_ctxt_resp_svc_req : Invalid message buffer ");

    const s1_incoming_msg_header_t *s1_msg_data =
            static_cast<const s1_incoming_msg_header_t*>(msgBuf->getDataPointer());
    VERIFY(s1_msg_data,
            procCtxt->setMmeErrorCause(INVALID_DATA_BUFFER); return ActStatus::ABORT,
            "process_init_ctxt_resp_svc_req : Invalid data buffer ");

    const initctx_resp_Q_msg_t *ics_res = static_cast<const initctx_resp_Q_msg_t*>(msgBuf->getDataPointer());

    auto &bearerCtxtCont = sessionCtxt->getBearerContextContainer();
    VERIFY(bearerCtxtCont.size() > 0,
            procCtxt->setMmeErrorCause(BEARER_CONTAINER_EMPTY); return ActStatus::ABORT,
            "Bearers Container is empty");

    uint8_t i = 0;

    for (auto &bearerCtxt : bearerCtxtCont)
    {
        if ((bearerCtxt != NULL) && (i < ics_res->erab_setup_resp_list.count))
        {
            fteid S1uEnbUserFteid;
            S1uEnbUserFteid.header.iface_type = 0;
            S1uEnbUserFteid.header.v4 = 1;
            S1uEnbUserFteid.header.teid_gre = ics_res->erab_setup_resp_list.erab_su_res_item[i].gtp_teid;
            S1uEnbUserFteid.ip.ipv4 = *(struct in_addr*)&ics_res->erab_setup_resp_list.erab_su_res_item[i].transportLayerAddress;

            if (bearerCtxt->getBearerId() == ics_res->erab_setup_resp_list.erab_su_res_item[i].e_RAB_ID)
            {
                bearerCtxt->setS1uEnbUserFteid(Fteid(S1uEnbUserFteid));
            }
            i++;
        }
    }

    ProcedureStats::num_of_processed_init_ctxt_resp++;
    log_msg(LOG_DEBUG, "Leaving process_init_ctxt_resp_svc_req ");

    return ActStatus::PROCEED;
}


/***************************************
* Action handler : send_mb_req_to_sgw_svc_req
***************************************/
ActStatus ActionHandlers::send_mb_req_to_sgw_svc_req(ControlBlock& cb)
{
    log_msg(LOG_DEBUG, "Inside send_mb_req_to_sgw_svc_req ");

    UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
    VERIFY_UE(cb, ue_ctxt, "Invalid UE");
    ActStatus actStatus = ActStatus::PROCEED;

    MmeProcedureCtxt *procCtxt =
            dynamic_cast<MmeProcedureCtxt*>(cb.getTempDataBlock());
    VERIFY(procCtxt, return ActStatus::ABORT, "Procedure Context is NULL ");

    auto& sessionCtxtContainer = ue_ctxt->getSessionContextContainer();
    VERIFY(sessionCtxtContainer.size() > 0,
            procCtxt->setMmeErrorCause(SESSION_CONTAINER_EMPTY); return ActStatus::ABORT,
            "Sessions Container is empty");

    SessionContext* sessionCtxt = sessionCtxtContainer.front();
    VERIFY(sessionCtxt,
            procCtxt->setMmeErrorCause(SESSION_CONTEXT_NOT_FOUND); return ActStatus::ABORT,
            "Session Context is NULL ");

    //Support dedicated bearers
    auto& bearerCtxtCont = sessionCtxt->getBearerContextContainer();
    VERIFY(bearerCtxtCont.size() > 0,
            procCtxt->setMmeErrorCause(BEARER_CONTAINER_EMPTY); return ActStatus::ABORT,
            "Bearers Container is empty");

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
		    for (int j = 0; j < prcdCtxt->getErabToBeModifiedListLen(); j++)
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

				bearerCtxt->setS1uEnbUserFteid(Fteid(S1uEnbUserFteid));
			    }
		    }
		}
	    }

	    memcpy(&(mb_msg.bearer_ctx_list.bearer_ctxt[i].s1u_enb_fteid),
		&(bearerCtxt->getS1uEnbUserFteid().fteid_m), sizeof(struct fteid));
	    i++;
	}
    }
    cmn::ipc::IpcAddress destAddr;
    destAddr.u32 = TipcServiceInstance::s11AppInstanceNum_c;

    mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_TX_S11_MODIFY_BEARER_REQUEST);
    MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(
                                MmeIpcInterfaceCompId));
    mmeIpcIf.dispatchIpcMsg((char*) &mb_msg, sizeof(mb_msg), destAddr);

    ProcedureStats::num_of_mb_req_to_sgw_sent++;

    log_msg(LOG_DEBUG, "Leaving send_mb_req_to_sgw_svc_req ");

    return actStatus;
}

/***************************************
 * Action handler : process_mb_resp_svc_req
 ***************************************/
ActStatus ActionHandlers::process_mb_resp_svc_req(ControlBlock &cb)
{
    log_msg(LOG_DEBUG, "Inside process_mb_resp_svc_req ");

    ProcedureStats::num_of_processed_mb_resp++;

    UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
    VERIFY_UE(cb, ue_ctxt, "Invalid UE");

    MmeProcedureCtxt *procCtxt =
            dynamic_cast<MmeProcedureCtxt*>(cb.getTempDataBlock());
    VERIFY(procCtxt, return ActStatus::ABORT, "Procedure Context is NULL ");

    MsgBuffer *msgBuf = static_cast<MsgBuffer*>(cb.getMsgData());
    VERIFY(msgBuf,
            procCtxt->setMmeErrorCause(INVALID_MSG_BUFFER); return ActStatus::ABORT,
            "process_mb_resp_svc_req : Invalid message buffer ");
    VERIFY(msgBuf->getLength() >= sizeof(struct MB_resp_Q_msg),
            return ActStatus::ABORT, "Invalid MBResp message length ");

    const struct MB_resp_Q_msg *mbr_info =
            static_cast<const MB_resp_Q_msg*>(msgBuf->getDataPointer());

    ActStatus actStatus = ActStatus::PROCEED;

    if (mbr_info->cause != GTPV2C_CAUSE_REQUEST_ACCEPTED)
    {
        log_msg(LOG_INFO, "process_mb_resp_svc_req: MB Response Failure");

        procCtxt->setMmeErrorCause(S11_MODIFY_BEARER_RESP_FAILURE);
        actStatus = ActStatus::ABORT;
    }
    else
    {
        auto& sessionCtxtContainer = ue_ctxt->getSessionContextContainer();
        VERIFY(sessionCtxtContainer.size() > 0,
                procCtxt->setMmeErrorCause(SESSION_CONTAINER_EMPTY); return ActStatus::ABORT,
                "Sessions Container is empty");

        SessionContext* sessionCtxt = sessionCtxtContainer.front();
        VERIFY(sessionCtxt,
                procCtxt->setMmeErrorCause(SESSION_CONTEXT_NOT_FOUND); return ActStatus::ABORT,
                "Session Context is NULL ");

        //TODO: Support dedicated bearers
        BearerContext *bearerCtxt = sessionCtxt->findBearerContextByBearerId(
                sessionCtxt->getLinkedBearerId());
        VERIFY(bearerCtxt,
                procCtxt->setMmeErrorCause(BEARER_CONTEXT_NOT_FOUND); return ActStatus::ABORT,
                "Bearer Context is NULL ");

        if (procCtxt->getCtxtType() == erabModInd_c) {
            MmeErabModIndProcedureCtxt *procCtxt =
                    static_cast<MmeErabModIndProcedureCtxt*>(cb.getTempDataBlock());
            VERIFY(procCtxt, return ActStatus::ABORT,
                    "Procedure Context is NULL ");

            uint8_t eRabsModifiedList[1] = { bearerCtxt->getBearerId() };
            procCtxt->setErabModifiedList(eRabsModifiedList, 1);
        }
    }

    return actStatus;
}

/***************************************
* Action handler : send_service_reject
***************************************/
ActStatus ActionHandlers::send_service_reject(ControlBlock& cb)
{
    log_msg(LOG_DEBUG, "Inside send_service_reject ");

    UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
    VERIFY_UE(cb, ue_ctxt, "Invalid UE");

    struct commonRej_info service_rej;

    service_rej.msg_type = service_reject;
    service_rej.ue_idx = ue_ctxt->getContextID();
    service_rej.s1ap_enb_ue_id = ue_ctxt->getS1apEnbUeId();
    service_rej.enb_fd = ue_ctxt->getEnbFd();

    MmeProcedureCtxt* srPrcdCtxt_p =
            dynamic_cast<MmeProcedureCtxt*>(cb.getTempDataBlock());
    if(srPrcdCtxt_p != NULL)
    {
        service_rej.cause = MmeCauseUtils::convertToNasEmmCause(srPrcdCtxt_p->getMmeErrorCause());
    }
    else
    {
        service_rej.cause = emmCause_network_failure;
    }

    struct Buffer nasBuffer;
    struct nasPDU nas = { 0 };
    const uint8_t num_nas_elements = 1;
    nas.elements = (nas_pdu_elements *) calloc(num_nas_elements,
            sizeof(nas_pdu_elements)); // TODO : should i use new ?
    nas.elements_len = num_nas_elements;
    nas.header.security_header_type = Plain;
    nas.header.proto_discriminator = EPSMobilityManagementMessages;
    nas.header.message_type = ServiceReject;
    //Removed the hard-coded value "0x09", to set the appropriate EMM Cause
    nas.elements[0].pduElement.attach_res = service_rej.cause;
    MmeNasUtils::encode_nas_msg(&nasBuffer, &nas, ue_ctxt->getUeSecInfo());
    memcpy(&service_rej.nasMsgBuf[0], &nasBuffer.buf[0], nasBuffer.pos);
    service_rej.nasMsgSize = nasBuffer.pos;
    free(nas.elements);

    cmn::ipc::IpcAddress destAddr;
    destAddr.u32 = TipcServiceInstance::s1apAppInstanceNum_c;

    mmeStats::Instance()->increment(
            mmeStatsCounter::MME_MSG_TX_NAS_SERVICE_REJECT);
    MmeIpcInterface &mmeIpcIf =
            static_cast<MmeIpcInterface&>(compDb.getComponent(
                    MmeIpcInterfaceCompId));
    mmeIpcIf.dispatchIpcMsg((char *) &service_rej, sizeof(service_rej),
            destAddr);

    ProcedureStats::num_of_service_reject_sent++;

    return ActStatus::PROCEED;

}

/***************************************
* Action handler : svc_req_state_guard_timeout
***************************************/
ActStatus ActionHandlers::svc_req_state_guard_timeout(ControlBlock& cb)
{
    MmeSvcReqProcedureCtxt *procedure_p =
            dynamic_cast<MmeSvcReqProcedureCtxt*>(cb.getTempDataBlock());
    if (procedure_p != NULL)
    {
        procedure_p->setMmeErrorCause(NETWORK_TIMEOUT);
    }
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : send_ddn_failure_ind
***************************************/
ActStatus ActionHandlers::send_ddn_failure_ind(ControlBlock& cb)
{
    log_msg(LOG_DEBUG, "Inside send_ddn_failure_ind ");

    UEContext *ue_ctxt = static_cast<UEContext*>(cb.getPermDataBlock());
    VERIFY_UE(cb, ue_ctxt, "Invalid UE");

    MmeSvcReqProcedureCtxt* srPrcdCtxt_p =
            dynamic_cast<MmeSvcReqProcedureCtxt*>(cb.getTempDataBlock());
    VERIFY(srPrcdCtxt_p, return ActStatus::ABORT,
            "Procedure Context is NULL ");

    auto& sessionCtxtContainer = ue_ctxt->getSessionContextContainer();
    VERIFY(sessionCtxtContainer.size() > 0,
            srPrcdCtxt_p->setMmeErrorCause(SESSION_CONTAINER_EMPTY); return ActStatus::ABORT,
            "Sessions Container is empty");

    SessionContext* sess_p = sessionCtxtContainer.front();
    VERIFY(sess_p,
            srPrcdCtxt_p->setMmeErrorCause(SESSION_CONTEXT_NOT_FOUND); return ActStatus::ABORT,
            "Session Context is NULL ");

    DDN_FAIL_Q_msg ddn_fail;
    ddn_fail.msg_type = ddn_failure_indication;
    ddn_fail.seq_no = srPrcdCtxt_p->getDdnSeqNo();
    memcpy(&(ddn_fail.s11_sgw_c_fteid), &(sess_p->getS11SgwCtrlFteid().fteid_m),
            sizeof(struct Fteid));
    ddn_fail.cause = GTPV2C_CAUSE_UE_NOT_RESPONDING;

    cmn::ipc::IpcAddress destAddr;
    destAddr.u32 = TipcServiceInstance::s11AppInstanceNum_c;

    mmeStats::Instance()->increment(
            mmeStatsCounter::MME_MSG_TX_S11_DOWNLINK_DATA_NOTIFICATION_FAILURE_IND);
    MmeIpcInterface &mmeIpcIf =
            static_cast<MmeIpcInterface&>(compDb.getComponent(
                    MmeIpcInterfaceCompId));
    mmeIpcIf.dispatchIpcMsg((char *) &ddn_fail, sizeof(ddn_fail), destAddr);

    log_msg(LOG_DEBUG, "Leaving send_ddn_failure_ind ");

    return ActStatus::PROCEED;
}

/***************************************
* Action handler : abort_service_req_procedure
***************************************/
ActStatus ActionHandlers::abort_service_req_procedure(ControlBlock& cb)
{
    log_msg(LOG_DEBUG,"abort_service_req_procedure : Entry ");

    ERROR_CODES errorCause = SUCCESS;
    PagingTrigger pagingTrigger = 0;

    MmeSvcReqProcedureCtxt *procedure_p =
            dynamic_cast<MmeSvcReqProcedureCtxt*>(cb.getTempDataBlock());
    if (procedure_p != NULL)
    {
        errorCause = procedure_p->getMmeErrorCause();
        pagingTrigger = procedure_p->getPagingTrigger();

        MmeContextManagerUtils::deallocateProcedureCtxt(cb, procedure_p);
    }

    switch (errorCause)
    {
    case SESSION_CONTAINER_EMPTY:
    case SESSION_CONTEXT_NOT_FOUND:
    case BEARER_CONTAINER_EMPTY:
    case BEARER_CONTEXT_NOT_FOUND:
    case S11_MODIFY_BEARER_RESP_FAILURE:
    {
        MmeContextManagerUtils::deleteUEContext(cb.getCBIndex());
        break;
    }
    default:
    {
        if ((pagingTrigger & pgwInit_c) != 0)
        {
            /* Fire a page failure event. This is a CONSUME_AND_FORWARD event.
             * Other procedures waiting for paging complete (Create Bearer, Delete Bearer etc.)
             * should handle this event in the corresponding state and
             * take appropriate corrective action.
             */
            SM::Event evt(PAGING_FAILURE, NULL);
            cb.qInternalEvent(evt);
        }

        // Retain UE context in attached - idle state?

        break;
    }
    }

    return ActStatus::PROCEED;
}
/***************************************
* Action handler : service_request_complete
***************************************/
ActStatus ActionHandlers::service_request_complete(ControlBlock& cb)
{
    mmeStats::Instance()->increment(
            mmeStatsCounter::MME_PROCEDURES_SERVICE_REQUEST_PROC_SUCCESS);

    MmeSvcReqProcedureCtxt *procedure_p =
            static_cast<MmeSvcReqProcedureCtxt*>(cb.getTempDataBlock());
    VERIFY(procedure_p, return ActStatus::ABORT, "Procedure Context is NULL ");

    if (procedure_p->checkPagingTriggerBit(pgwInit_c))
    {
        SM::Event evt(PAGING_COMPLETE, NULL);
        cb.qInternalEvent(evt);
    }

    MmeContextManagerUtils::deallocateProcedureCtxt(cb, procedure_p);

    return ActStatus::PROCEED;
}

