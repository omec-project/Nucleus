/*
* Copyright 2020-present, Infosys Ltd.
*
* SPDX-License-Identifier: Apache-2.0
*/

#include <utils/mmeGtpMsgUtils.h>

#include <controlBlock.h>
#include <contextManager/dataBlocks.h>
#include <utils/mmeCauseUtils.h>
#include <utils/mmeContextManagerUtils.h>
#include "gtpCauseTypes.h"
#include <log.h>
#include <mme_app.h>

using namespace mme;

void MmeGtpMsgUtils::populateModifyBearerRequestHo(SM::ControlBlock& cb,
        UEContext& ueCtxt,
        SessionContext& sessionCtxt,
		S1HandoverProcedureContext& procCtxt,
		struct MB_Q_msg& mbMsg)
{
	mbMsg.msg_type = modify_bearer_request;
	mbMsg.ue_idx = ueCtxt.getContextID();
    memset(mbMsg.indication, 0, S11_MB_INDICATION_FLAG_SIZE);

    BearerContext *bearerCtxt = sessionCtxt.findBearerContextByBearerId(sessionCtxt.getLinkedBearerId());
    if (bearerCtxt == NULL)
    {
        log_msg(LOG_DEBUG, "send_mb_req_to_sgw_for_ho: bearer ctxt is NULL ");
        return;
    }

    mbMsg.bearer_ctx_list.bearers_count = 1;

    mbMsg.bearer_ctx_list.bearer_ctxt[0].eps_bearer_id = bearerCtxt->getBearerId();
    memcpy(&(mbMsg.s11_sgw_c_fteid),
            &(sessionCtxt.getS11SgwCtrlFteid().fteid_m), sizeof(struct fteid));

    mbMsg.bearer_ctx_list.bearer_ctxt[0].s1u_enb_fteid.header.iface_type = 0;
    mbMsg.bearer_ctx_list.bearer_ctxt[0].s1u_enb_fteid.header.v4 = 1;
    mbMsg.bearer_ctx_list.bearer_ctxt[0].s1u_enb_fteid.header.teid_gre = procCtxt.getErabAdmittedItem().gtp_teid;
    mbMsg.bearer_ctx_list.bearer_ctxt[0].s1u_enb_fteid.ip.ipv4.s_addr =
            procCtxt.getErabAdmittedItem().transportLayerAddress;

    bearerCtxt->setS1uEnbUserFteid(Fteid(mbMsg.bearer_ctx_list.bearer_ctxt[0].s1u_enb_fteid));

    mbMsg.userLocationInformationIePresent = true;
    memcpy(&(mbMsg.tai), &(procCtxt.getTargetTai().tai_m), sizeof(struct TAI));
    memcpy(&(mbMsg.utran_cgi), &(procCtxt.getTargetCgi().cgi_m), sizeof(struct CGI));

    mbMsg.servingNetworkIePresent = true;

}

bool MmeGtpMsgUtils::populateModifyBearerRequestForX2Ho(SM::ControlBlock& cb,
        UEContext& ueCtxt,
        SessionContext& sessionCtxt,
        X2HOSmProcedureContext& procCtxt,
        struct MB_Q_msg& mbMsg)
{
    bool rc = false;

    pathSwitchRequest_Msg_t *pathSwReq = NULL;

    cmn::IpcEventMessage *ipcMsg =
            const_cast<cmn::IpcEventMessage*>(
                    dynamic_cast<const cmn::IpcEventMessage*>(procCtxt.getPathSwitchReqMsgRaw()));
    if (ipcMsg)
    {
        MsgBuffer *msgBuf = static_cast<MsgBuffer*>(ipcMsg->getMsgBuffer());
        pathSwReq =
                static_cast<pathSwitchRequest_Msg_t*>(msgBuf->getDataPointer());
    }
    if (pathSwReq == NULL)
    {
        procCtxt.setMmeErrorCause(INVALID_MSG_BUFFER);
        return rc;
    }

    mbMsg.msg_type = modify_bearer_request;
    mbMsg.ue_idx = ueCtxt.getContextID();
    memset(mbMsg.indication, 0, S11_MB_INDICATION_FLAG_SIZE);

    uint8_t bearersToBeModifiedCount = 0;
    uint8_t bearersToBeRemovedCount = 0;

    auto &bearerCtxtCont = sessionCtxt.getBearerContextContainer();
    for (auto bearerCtxt : bearerCtxtCont)
    {
        if (bearerCtxt)
        {
            bool bearerFound = false;

            for (int i = 0;
                    i < pathSwReq->erab_to_be_switched_dl_list.count;
                    i++)
            {
                erab_switch_item *erabItem = &(pathSwReq->erab_to_be_switched_dl_list.erab_item[i]);
                bearer_ctxt_t *bearerToBeModified = &(mbMsg.bearer_ctx_list.bearer_ctxt[bearersToBeModifiedCount]);

                if (erabItem->e_RAB_ID == bearerCtxt->getBearerId())
                {
                    bearerFound = true;

                    bearerToBeModified->eps_bearer_id = erabItem->e_RAB_ID;

                    bearerToBeModified->s1u_enb_fteid.header.iface_type = 0;
                    bearerToBeModified->s1u_enb_fteid.header.v4 = 1;
                    bearerToBeModified->s1u_enb_fteid.header.teid_gre = erabItem->gtp_teid;
                    bearerToBeModified->s1u_enb_fteid.ip.ipv4.s_addr = erabItem->transportLayerAddress;

                    bearerCtxt->setS1uEnbUserFteid(Fteid(bearerToBeModified->s1u_enb_fteid));

                    bearersToBeModifiedCount++;
                }
            }

            if (!bearerFound)
            {
                if (bearerCtxt->getBearerId() == sessionCtxt.getLinkedBearerId())
                {
                    rc = false;
                    procCtxt.setMmeErrorCause(X2HO_PSREQ_MISSING_DEF_BEARER);
                    break;
                }
                else
                {
                    mbMsg.bearers_to_be_removed_list.bearer_id[bearersToBeRemovedCount] = bearerCtxt->getBearerId();
		    bearersToBeRemovedCount++;

                    log_msg(LOG_ERROR,
                            "Dedicated Bearer with erab_id %d isn't accepted "
                            "by the target eNB in X2 HO for the UE with IMSI %s ",
                            bearerCtxt->getBearerId(),
                            ueCtxt.getImsi().getDigitsArray());
                }
            }
        }
    }
    if (bearersToBeModifiedCount)
    {
        mbMsg.bearer_ctx_list.bearers_count = bearersToBeModifiedCount;
	mbMsg.bearers_to_be_removed_list.bearers_count = bearersToBeRemovedCount;
        memcpy(&(mbMsg.s11_sgw_c_fteid),
            &(sessionCtxt.getS11SgwCtrlFteid().fteid_m), sizeof(struct fteid));
        mbMsg.userLocationInformationIePresent = true;
        memcpy(&(mbMsg.tai), &(pathSwReq->tai), sizeof(struct TAI));
        memcpy(&(mbMsg.utran_cgi), &(pathSwReq->cgi), sizeof(struct CGI));
        mbMsg.servingNetworkIePresent = true;
	rc = true;
    }
    return rc;
}

bool MmeGtpMsgUtils::populateCreateBearerResponse(SM::ControlBlock &cb,
        MmeSmCreateBearerProcCtxt &createBearerProc, struct CB_RESP_Q_msg &cb_resp)
{
    bool status = false;

    cb_resp.msg_type = create_bearer_response;

    UEContext *ueCtxt_p = static_cast<UEContext*>(cb.getPermDataBlock());
    if (ueCtxt_p != NULL)
    {
        SessionContext *sessionCtxt_p =
                ueCtxt_p->findSessionContextByLinkedBearerId(
                        createBearerProc.getBearerId());

        if (sessionCtxt_p != NULL)
        {
            uint8_t index = 0;
            uint8_t successCount = 0;

            memcpy(&(cb_resp.s11_sgw_c_fteid),
                    &(sessionCtxt_p->getS11SgwCtrlFteid().fteid_m), sizeof(struct fteid));

            auto &bearerStatusContainer = createBearerProc.getBearerStatusContainer();
            cb_resp.bearer_ctxt_cb_resp_list.bearers_count = bearerStatusContainer.size();

            for (auto &it : bearerStatusContainer)
            {
                BearerContext *bearerCtxt_p =
                        MmeContextManagerUtils::findBearerContext(
                                it.bearer_ctxt_cb_resp_m.eps_bearer_id,
                                ueCtxt_p);

                memcpy(&cb_resp.bearer_ctxt_cb_resp_list.bearer_ctxt[index],
                        &it.bearer_ctxt_cb_resp_m,
                        sizeof(bearer_ctxt_cb_resp_t));
                if (bearerCtxt_p != NULL)
                {
                    memcpy(
                            &(cb_resp.bearer_ctxt_cb_resp_list.bearer_ctxt[index].s1u_enb_fteid),
                            &(bearerCtxt_p->getS1uEnbUserFteid().fteid_m),
                            sizeof(struct fteid));
                }

                index++;

                if (it.bearer_ctxt_cb_resp_m.cause.cause
                        == GTPV2C_CAUSE_REQUEST_ACCEPTED
                        || it.bearer_ctxt_cb_resp_m.cause.cause
                                == GTPV2C_CAUSE_REQUEST_ACCEPTED_PARTIALLY)
                {
                    successCount++;
                }
            }

            if (successCount == 0)
                cb_resp.cause = GTPV2C_CAUSE_REQUEST_REJECTED;
            else
                cb_resp.cause = GTPV2C_CAUSE_REQUEST_ACCEPTED;
        }
        else
        {
            log_msg(LOG_INFO,
                    "populateCreateBearerResponse : SessionContext is NULL ");
            cb_resp.cause = GTPV2C_CAUSE_CONTEXT_NOT_FOUND;
        }
    }
    else
    {
        log_msg(LOG_INFO,
                "populateCreateBearerResponse : UEContext is NULL ");
        cb_resp.cause = GTPV2C_CAUSE_CONTEXT_NOT_FOUND;
    }

    cmn::IpcEventMessage *ipcMsg =
            dynamic_cast<cmn::IpcEventMessage*>(createBearerProc.getCreateBearerReqEMsgRaw());

    if (ipcMsg != NULL)
    {
        cmn::utils::MsgBuffer *msgBuf =
                static_cast<cmn::utils::MsgBuffer*>(ipcMsg->getMsgBuffer());
        if (msgBuf != NULL)
        {
            cb_req_Q_msg *cb_req =
                    static_cast<cb_req_Q_msg*>(msgBuf->getDataPointer());

            if (cb_req != NULL)
            {
                // In case of unavailability of session/UE Contexts,
                // s11_sgw_cp_teid will be set as 0
                if (cb_resp.cause == GTPV2C_CAUSE_CONTEXT_NOT_FOUND)
                {
                    cb_resp.s11_sgw_c_fteid.header.teid_gre = 0;
                }
                cb_resp.s11_sgw_c_fteid.ip.ipv4.s_addr = cb_req->sgw_ip;
                cb_resp.destination_port = cb_req->source_port;
		cb_resp.seq_no = cb_req->seq_no;
		if(createBearerProc.getMmeErrorCause())
		{
                    cb_resp.cause = MmeCauseUtils::convertToGtpCause(
                        createBearerProc.getMmeErrorCause());
		}

                status = true;

                log_msg(LOG_INFO, "populateCreateBearerResponse : CB Response Cause: %d ", cb_resp.cause);
            }
        }
    }
    return status;
}

bool MmeGtpMsgUtils::populateDeleteBearerResponse(SM::ControlBlock &cb,
        MmeSmDeleteBearerProcCtxt &deleteBearerProc,
        struct DB_RESP_Q_msg &db_resp)
{
    bool status = false;
    cmn::utils::MsgBuffer *msgBuf = NULL;
    db_req_Q_msg *db_req = NULL;
    /*
     * Even in cases of abort, we are deleting the bearers implicitly.
     * Hence, initializing the cause value as REQUEST_ACCEPTED
     */
    uint8_t gtpCause = GTPV2C_CAUSE_REQUEST_ACCEPTED;

    cmn::IpcEventMessage *ipcMsg =
            dynamic_cast<cmn::IpcEventMessage*>(deleteBearerProc.getDeleteBearerReqEMsgRaw());

    if (ipcMsg != NULL)
    {
        msgBuf = static_cast<cmn::utils::MsgBuffer*>(ipcMsg->getMsgBuffer());
        if (msgBuf != NULL)
        {
            db_req = static_cast<db_req_Q_msg*>(msgBuf->getDataPointer());
        }
    }

    if (db_req == NULL)
    {
        log_msg(LOG_INFO,
                "populateDeleteBearerResponse : db_Req is NULL ");
        return status;
    }

    db_resp.msg_type = delete_bearer_response;

    UEContext *ueCtxt_p = static_cast<UEContext*>(cb.getPermDataBlock());
    if (ueCtxt_p != NULL)
    {
        SessionContext *sessionCtxt_p = ueCtxt_p->findSessionContextByLinkedBearerId(
                deleteBearerProc.getBearerId());

        if (sessionCtxt_p != NULL)
        {
            uint8_t index = 0;

            memcpy(&(db_resp.s11_sgw_c_fteid),
                    &(sessionCtxt_p->getS11SgwCtrlFteid().fteid_m),
                    sizeof(struct fteid));

            if (db_req->linked_bearer_id == 0)
            {
                auto &bearerStatusContainer =
                        deleteBearerProc.getBearerStatusContainer();
                db_resp.bearer_ctxt_db_resp_list.bearers_count =
                        bearerStatusContainer.size();

                for (auto &it : bearerStatusContainer)
                {
                    db_resp.bearer_ctxt_db_resp_list.bearer_ctxt[index].eps_bearer_id =
                           		it.eps_bearer_id;

                    db_resp.bearer_ctxt_db_resp_list.bearer_ctxt[index].cause.cause =
                            it.cause.cause;

                    if (it.pco_from_ue_opt.pco_length)
                    {
                        memcpy(
                                &db_resp.bearer_ctxt_db_resp_list.bearer_ctxt[index].pco_from_ue_opt.pco_options,
                                &it.pco_from_ue_opt.pco_options,
                                it.pco_from_ue_opt.pco_length);
                    }

                    index++;
                }
            }
            else
            {
                db_resp.linked_bearer_id = deleteBearerProc.getBearerId();
            }
        }
        else
        {
            log_msg(LOG_INFO,
                    "populateDeleteBearerResponse : SessionContext is NULL ");
            gtpCause = GTPV2C_CAUSE_CONTEXT_NOT_FOUND;
        }
    }
    else
    {
        log_msg(LOG_INFO,
                "populateDeleteBearerResponse : UEContext is NULL ");
        gtpCause = GTPV2C_CAUSE_CONTEXT_NOT_FOUND;
    }

    db_resp.s11_sgw_c_fteid.ip.ipv4.s_addr = db_req->sgw_ip;
    db_resp.destination_port = db_req->source_port;
    db_resp.seq_no = db_req->seq_no;

    if (deleteBearerProc.getMmeErrorCause())
    {
        gtpCause = MmeCauseUtils::convertToGtpCause(
                deleteBearerProc.getMmeErrorCause());
        /*
         * Rewriting the cause as GTPV2C_CAUSE_REQUEST_ACCEPTED,
         * since bearers are deleted even in cases of timeout &
         * abort except few cases such as CONTEXT_NOT_FOUND
         */
        if(gtpCause == GTPV2C_CAUSE_NETWORK_FAILURE)
            gtpCause = GTPV2C_CAUSE_REQUEST_ACCEPTED;
    }

    db_resp.cause = gtpCause;

    status = true;

    log_msg(LOG_INFO, "populateDeleteBearerResponse : DB Response Cause: %d ",
            db_resp.cause);

    return status;
}
