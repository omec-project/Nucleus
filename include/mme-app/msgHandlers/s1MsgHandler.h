/*
 * Copyright 2019-present Infosys Limited  
 *   
 * SPDX-License-Identifier: Apache-2.0    
 */

#ifndef INCLUDE_MME_APP_MSGHANDLERS_S1MSGHANDLER_H_
#define INCLUDE_MME_APP_MSGHANDLERS_S1MSGHANDLER_H_

#include <stdint.h>
#include <eventMessage.h>
#include <structs.h>

class S1MsgHandler {
public:
	static S1MsgHandler* Instance();
	~S1MsgHandler();

	void handleS1Message_v(cmn::IpcEMsgUnqPtr eMsg);

private:
	S1MsgHandler();

	void handleInitUeAttachRequestMsg_v(cmn::IpcEMsgUnqPtr eMsg);
	void handleIdentityResponseMsg_v(cmn::IpcEMsgUnqPtr eMsg, uint32_t ueIdx);
	void handleAuthResponseMsg_v(cmn::IpcEMsgUnqPtr eMsg, uint32_t ueIdx);
	void handleSecurityModeResponse_v(cmn::IpcEMsgUnqPtr eMsg, uint32_t ueIdx);
	void handleEsmInfoResponse_v(cmn::IpcEMsgUnqPtr eMsg, uint32_t ueIdx);
	void handleInitCtxtResponse_v(cmn::IpcEMsgUnqPtr eMsg, uint32_t ueIdx);
	void handleAttachComplete_v(cmn::IpcEMsgUnqPtr eMsg, uint32_t ueIdx);
	void handleDetachRequest_v(cmn::IpcEMsgUnqPtr eMsg, uint32_t ueIdx);
	void handleS1ReleaseRequestMsg_v(cmn::IpcEMsgUnqPtr eMsg, uint32_t ueIdx);
	void handleS1ReleaseComplete_v(cmn::IpcEMsgUnqPtr eMsg, uint32_t ueIdx);
	void handleNIDetachRequest_v(cmn::IpcEMsgUnqPtr eMsg, uint32_t ueIdx);
	void handleDetachAcceptFromUE_v(cmn::IpcEMsgUnqPtr eMsg, uint32_t ueIdx);
	void handleServiceRequest_v(cmn::IpcEMsgUnqPtr eMsg, uint32_t ueIdx);
	void handleTauRequestMsg_v(cmn::IpcEMsgUnqPtr eMsg, uint32_t ueIdx);
    void handleHandoverRequiredMsg_v(cmn::IpcEMsgUnqPtr eMsg, uint32_t ueIdx);
    void handleHandoverRequestAckMsg_v(cmn::IpcEMsgUnqPtr eMsg, uint32_t ueIdx);
    void handleHandoverNotifyMsg_v(cmn::IpcEMsgUnqPtr eMsg, uint32_t ueIdx);
    void handleEnbStatusTransferMsg_v(cmn::IpcEMsgUnqPtr eMsg, uint32_t ueIdx);
    void handleHandoverCancelMsg_v(cmn::IpcEMsgUnqPtr eMsg, uint32_t ueIdx);
    void handleHandoverFailureMsg_v(cmn::IpcEMsgUnqPtr eMsg, uint32_t ueIdx);
    void handleErabModificationIndicationMsg_v(cmn::IpcEMsgUnqPtr eMsg, uint32_t ueIdx);
	void handleErabSetupResponseMsg_v(cmn::IpcEMsgUnqPtr eMsg, uint32_t ueIdx);
    void handleActDedBearerCtxtAcceptMsg_v(cmn::IpcEMsgUnqPtr eMsg, uint32_t ueIdx);
    void handleActDedBearerCtxtRejectMsg_v(cmn::IpcEMsgUnqPtr eMsg, uint32_t ueIdx);
    void handleErabRelResponseMsg_v(cmn::IpcEMsgUnqPtr eMsg, uint32_t ueIdx);
    void handleDeActBearerCtxtAcceptMsg_v(cmn::IpcEMsgUnqPtr eMsg, uint32_t ueIdx);
    void handleS1apEnbStatusMsg_v(cmn::IpcEMsgUnqPtr eMsg);
    void handleNasPduParseFailureInd_v(NasPduParseFailureIndEMsgShPtr eMsg, uint32_t ueIdx);
};

#endif /* INCLUDE_MME_APP_MSGHANDLERS_S1MSGHANDLER_H_ */
