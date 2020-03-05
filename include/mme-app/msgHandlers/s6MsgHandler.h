/*
 * Copyright 2019-present Infosys Limited  
 *   
 * SPDX-License-Identifier: Apache-2.0    
 */

#ifndef INCLUDE_MME_APP_MSGHANDLERS_S6MSGHANDLER_H_
#define INCLUDE_MME_APP_MSGHANDLERS_S6MSGHANDLER_H_

#include "msgType.h"
#include <msgBuffer.h>

class S6MsgHandler {
public:
	static S6MsgHandler* Instance();
	virtual ~S6MsgHandler();

	void handleS6Message_v(cmn::utils::MsgBuffer* msgBuf);

private:
	S6MsgHandler();

	void handleAuthInfoAnswer_v(cmn::utils::MsgBuffer* msgData_p, uint32_t ueIdx);
	void handleUpdateLocationAnswer_v(cmn::utils::MsgBuffer* msgData_p, uint32_t ueIdx);
	void handlePurgeAnswer_v(cmn::utils::MsgBuffer* msgData_p, uint32_t ueIdx);
	void handleCancelLocationRequest_v(cmn::utils::MsgBuffer* msgData_p);

};

#endif /* INCLUDE_MME_APP_MSGHANDLERS_S6MSGHANDLER_H_ */
