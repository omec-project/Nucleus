/*
 * Copyright 2019-present Infosys Limited  
 *   
 * SPDX-License-Identifier: Apache-2.0    
 */

#ifndef INCLUDE_MME_APP_MSGHANDLERS_GTPMSGHANDLER_H_
#define INCLUDE_MME_APP_MSGHANDLERS_GTPMSGHANDLER_H_

#include <stdint.h>

namespace cmn
{
        class IpcEventMessage;
}

class GtpMsgHandler {
public:
	static GtpMsgHandler* Instance();
	~GtpMsgHandler();

	void handleGtpMessage_v(cmn::IpcEventMessage *eMsg);

private:
	GtpMsgHandler();

	void handleCreateSessionResponseMsg_v(cmn::IpcEventMessage* eMsg, uint32_t ueIdx);
	void handleModifyBearerResponseMsg_v(cmn::IpcEventMessage* eMsg, uint32_t ueIdx);
	void handleDeleteSessionResponseMsg_v(cmn::IpcEventMessage* eMsg, uint32_t ueIdx);
	void handleReleaseBearerResponseMsg_v(cmn::IpcEventMessage* eMsg, uint32_t ueIdx);
	void handleDdnMsg_v(cmn::IpcEventMessage* eMsg, uint32_t ueIdx);
};

#endif /* INCLUDE_MME_APP_MSGHANDLERS_GTPMSGHANDLER_H_ */
