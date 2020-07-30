/*
 * Copyright 2019-present Infosys Limited  
 *   
 * SPDX-License-Identifier: Apache-2.0    
 */

#ifndef INCLUDE_MME_APP_MSGHANDLERS_S1MSGHANDLER_H_
#define INCLUDE_MME_APP_MSGHANDLERS_S1MSGHANDLER_H_

#include <stdint.h>
#include <mme_prometheus.h>

namespace cmn
{
	class IpcEventMessage;
}

class S1MsgHandler {
public:
    Counter &attach_req_counter;
    Counter &id_rsp_counter;
    Counter &auth_rsp_counter;
    Counter &sec_mod_complete_counter;
    Counter &esm_rsp_counter;
    Counter &init_ctxt_rsp_counter;
    Counter &attach_complete_counter;
    Counter &detach_req_counter;
    Counter &s1_release_req_counter;
    Counter &s1_release_comp_counter;
    Counter &detach_accept_counter;
    Counter &service_req_counter;
    Counter &tau_req_counter;
    Counter &handover_req_ack_counter;
    Counter &handover_notify_counter;
    Counter &handover_required_counter;
    Counter &enb_status_transfer_counter;
    Counter &handover_cancel_counter;
    Counter &handover_failure_counter;
    Counter &erab_mod_ind_counter;
    Counter &unknown_msg_counter;

	static S1MsgHandler* Instance(Family<Counter> &msg_info);
	~S1MsgHandler();

	void handleS1Message_v(cmn::IpcEventMessage* eMsg);

private:
	S1MsgHandler(Family<Counter> &msg_info);

	void handleInitUeAttachRequestMsg_v(cmn::IpcEventMessage* eMsg);
	void handleIdentityResponseMsg_v(cmn::IpcEventMessage* eMsg, uint32_t ueIdx);
	void handleAuthResponseMsg_v(cmn::IpcEventMessage* eMsg, uint32_t ueIdx);
	void handleSecurityModeResponse_v(cmn::IpcEventMessage* eMsg, uint32_t ueIdx);
	void handleEsmInfoResponse_v(cmn::IpcEventMessage* eMsg, uint32_t ueIdx);
	void handleInitCtxtResponse_v(cmn::IpcEventMessage* eMsg, uint32_t ueIdx);
	void handleAttachComplete_v(cmn::IpcEventMessage* eMsg, uint32_t ueIdx);
	void handleDetachRequest_v(cmn::IpcEventMessage* eMsg, uint32_t ueIdx);
	void handleS1ReleaseRequestMsg_v(cmn::IpcEventMessage* eMsg, uint32_t ueIdx);
	void handleS1ReleaseComplete_v(cmn::IpcEventMessage* eMsg, uint32_t ueIdx);
	void handleNIDetachRequest_v(cmn::IpcEventMessage* eMsg, uint32_t ueIdx);
	void handleDetachAcceptFromUE_v(cmn::IpcEventMessage* eMsg, uint32_t ueIdx);
	void handleServiceRequest_v(cmn::IpcEventMessage* eMsg, uint32_t ueIdx);
	void handleTauRequestMsg_v(cmn::IpcEventMessage* eMsg, uint32_t ueIdx);
    void handleHandoverRequiredMsg_v(cmn::IpcEventMessage* eMsg, uint32_t ueIdx);
    void handleHandoverRequestAckMsg_v(cmn::IpcEventMessage* eMsg, uint32_t ueIdx);
    void handleHandoverNotifyMsg_v(cmn::IpcEventMessage* eMsg, uint32_t ueIdx);
    void handleEnbStatusTransferMsg_v(cmn::IpcEventMessage* eMsg, uint32_t ueIdx);
    void handleHandoverCancelMsg_v(cmn::IpcEventMessage* eMsg, uint32_t ueIdx);
    void handleHandoverFailureMsg_v(cmn::IpcEventMessage* eMsg, uint32_t ueIdx);
    void handleErabModificationIndicationMsg_v(cmn::IpcEventMessage* eMsg, uint32_t ueIdx);
};

#endif /* INCLUDE_MME_APP_MSGHANDLERS_S1MSGHANDLER_H_ */
