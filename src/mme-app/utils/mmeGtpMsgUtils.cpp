/*
* Copyright 2020-present, Infosys Ltd.
*
* SPDX-License-Identifier: Apache-2.0
*/

#include <utils/mmeGtpMsgUtils.h>

#include <controlBlock.h>
#include <contextManager/dataBlocks.h>
#include <contextManager/subsDataGroupManager.h>
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

    BearerContext *bearerCtxt = sessionCtxt.getBearerContext();
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
