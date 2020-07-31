/*
 * Copyright 2019-present Infosys Limited  
 *   
 * SPDX-License-Identifier: Apache-2.0    
 */

#ifndef INCLUDE_MME_APP_MSGHANDLERS_S6MSGHANDLER_H_
#define INCLUDE_MME_APP_MSGHANDLERS_S6MSGHANDLER_H_

#include <stdint.h>

namespace cmn
{
        class IpcEventMessage;
}

class S6MsgHandler {
public:
	static S6MsgHandler* Instance();
	virtual ~S6MsgHandler();
	void handleS6Message_v(cmn::IpcEventMessage* eMsg);

private:
	S6MsgHandler();
	void handleAuthInfoAnswer_v(cmn::IpcEventMessage* eMsg, uint32_t ueIdx);
	void handleUpdateLocationAnswer_v(cmn::IpcEventMessage* eMsg, uint32_t ueIdx);
	void handlePurgeAnswer_v(cmn::IpcEventMessage* eMsg, uint32_t ueIdx);
	void handleCancelLocationRequest_v(cmn::IpcEventMessage* eMsg);

};

#endif /* INCLUDE_MME_APP_MSGHANDLERS_S6MSGHANDLER_H_ */
