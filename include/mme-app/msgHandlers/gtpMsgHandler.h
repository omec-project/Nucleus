/*
 * Copyright 2019-present Infosys Limited  
 *   
 * SPDX-License-Identifier: Apache-2.0    
 */

#ifndef INCLUDE_MME_APP_MSGHANDLERS_GTPMSGHANDLER_H_
#define INCLUDE_MME_APP_MSGHANDLERS_GTPMSGHANDLER_H_

#include "msgType.h"
#include <msgBuffer.h>

class GtpMsgHandler {
public:
	static GtpMsgHandler* Instance();
	~GtpMsgHandler();

	void handleGtpMessage_v(cmn::utils::MsgBuffer* buffer);

private:
	GtpMsgHandler();

	void handleCreateSessionResponseMsg_v(const cmn::utils::MsgBuffer* msgData_p, uint32_t ueIdx);
	void handleModifyBearerResponseMsg_v(const cmn::utils::MsgBuffer* msgData_p, uint32_t ueIdx);
	void handleDeleteSessionResponseMsg_v(const cmn::utils::MsgBuffer* msgData_p, uint32_t ueIdx);
	void handleReleaseBearerResponseMsg_v(const cmn::utils::MsgBuffer* msgData_p, uint32_t ueIdx);
	void handleDdnMsg_v(const cmn::utils::MsgBuffer* msgData_p, uint32_t ueIdx);
};

#endif /* INCLUDE_MME_APP_MSGHANDLERS_GTPMSGHANDLER_H_ */
