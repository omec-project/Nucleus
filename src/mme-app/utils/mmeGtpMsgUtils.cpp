/*
* Copyright 2020-present, Infosys Ltd.
*
* SPDX-License-Identifier: Apache-2.0
*/

#include <utils/mmeGtpMsgUtils.h>

#include <controlBlock.h>
#include <contextManager/dataBlocks.h>
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
        log_msg(LOG_DEBUG, "send_mb_req_to_sgw_for_ho: bearer ctxt is NULL \n");
        return;
    }

    mbMsg.bearer_id = bearerCtxt->getBearerId();
    memcpy(&(mbMsg.s11_sgw_c_fteid),
            &(sessionCtxt.getS11SgwCtrlFteid().fteid_m), sizeof(struct fteid));

    mbMsg.s1u_enb_fteid.header.iface_type = 0;
    mbMsg.s1u_enb_fteid.header.v4 = 1;
    mbMsg.s1u_enb_fteid.header.teid_gre = procCtxt.getErabAdmittedItem().gtp_teid;
    mbMsg.s1u_enb_fteid.ip.ipv4.s_addr =
            procCtxt.getErabAdmittedItem().transportLayerAddress;

    bearerCtxt->setS1uEnbUserFteid(Fteid(mbMsg.s1u_enb_fteid));

    mbMsg.userLocationInformationIePresent = true;
    memcpy(&(mbMsg.tai), &(procCtxt.getTargetTai().tai_m), sizeof(struct TAI));
    memcpy(&(mbMsg.utran_cgi), &(procCtxt.getTargetCgi().cgi_m), sizeof(struct CGI));

    mbMsg.servingNetworkIePresent = true;

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
                    "populateCreateBearerResponse : SessionContext is NULL \n");
            cb_resp.cause = GTPV2C_CAUSE_CONTEXT_NOT_FOUND;
        }
    }
    else
    {
        log_msg(LOG_INFO,
                "populateCreateBearerResponse : UEContext is NULL \n");
        cb_resp.cause = GTPV2C_CAUSE_CONTEXT_NOT_FOUND;
    }

    const cmn::IpcEventMessage *eMsg =
            dynamic_cast<const cmn::IpcEventMessage*>(createBearerProc.getCreateBearerReqEMsgRaw());

    cmn::IpcEventMessage *ipcMsg = const_cast<cmn::IpcEventMessage *>(eMsg);

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

                //TODO : SET SEQ NUMBER

                status = true;

                log_msg(LOG_INFO, "populateCreateBearerResponse : CB Response Cause: %d \n", cb_resp.cause);
            }
        }
    }
    return status;
}
