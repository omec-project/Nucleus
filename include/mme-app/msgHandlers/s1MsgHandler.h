/*
 * Copyright 2019-present Infosys Limited  
 *   
 * SPDX-License-Identifier: Apache-2.0    
 */

#ifndef INCLUDE_MME_APP_MSGHANDLERS_S1MSGHANDLER_H_
#define INCLUDE_MME_APP_MSGHANDLERS_S1MSGHANDLER_H_

#include "msgType.h"
#include <msgBuffer.h>

class S1MsgHandler {
public:
	static S1MsgHandler* Instance();
	~S1MsgHandler();

	void handleS1Message_v(const cmn::utils::MsgBuffer* buffer);

private:
	S1MsgHandler();

	void handleInitUeAttachRequestMsg_v(const cmn::utils::MsgBuffer* msgData_p);
	void handleIdentityResponseMsg_v(const cmn::utils::MsgBuffer* msgData_p, uint32_t ueIdx);
	void handleAuthResponseMsg_v(const cmn::utils::MsgBuffer* msgData_p, uint32_t ueIdx);
	void handleSecurityModeResponse_v(const cmn::utils::MsgBuffer* msgData_p, uint32_t ueIdx);
	void handleEsmInfoResponse_v(const cmn::utils::MsgBuffer* msgData_p, uint32_t ueIdx);
	void handleInitCtxtResponse_v(const cmn::utils::MsgBuffer* msgData_p, uint32_t ueIdx);
	void handleAttachComplete_v(const cmn::utils::MsgBuffer* msgData_p, uint32_t ueIdx);
	void handleDetachRequest_v(const cmn::utils::MsgBuffer* msgData_p, uint32_t ueIdx);
	void handleS1ReleaseRequestMsg_v(const cmn::utils::MsgBuffer* msgData_p, uint32_t ueIdx);
	void handleS1ReleaseComplete_v(const cmn::utils::MsgBuffer* msgData_p, uint32_t ueIdx);
	void handleNIDetachRequest_v(const cmn::utils::MsgBuffer* msgData_p, uint32_t ueIdx);
	void handleDetachAcceptFromUE_v(const cmn::utils::MsgBuffer* msgData_p, uint32_t ueIdx);
	void handleServiceRequest_v(const cmn::utils::MsgBuffer* msgData_p, uint32_t ueIdx);
	void handleTauRequestMsg_v(const cmn::utils::MsgBuffer* msgData_p, uint32_t ueIdx);
};

#endif /* INCLUDE_MME_APP_MSGHANDLERS_S1MSGHANDLER_H_ */
