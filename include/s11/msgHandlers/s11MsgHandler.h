/*
 * Copyright 2020-present Open Networking Foundation
 * Copyright 2019-present Infosys Limited  
 *   
 * SPDX-License-Identifier: Apache-2.0    
 */

#ifndef INCLUDE_S11_APP_MSGHANDLERS_S11MSGHANDLER_H_
#define INCLUDE_S11_APP_MSGHANDLERS_S11MSGHANDLER_H_

#include <stdint.h>

namespace cmn
{
	class IpcEventMessage;
}

class S11MsgHandler {
public:
	static S11MsgHandler* Instance();
	~S11MsgHandler();

    void handleMmeMessage_v(cmn::IpcEventMessage* eMsg);

private:
	S11MsgHandler();
	void handleCreateSessionRequestMsg_v(cmn::IpcEventMessage* eMsg);
    void handleModifyBearerRequestMsg_v(cmn::IpcEventMessage* eMsg);
    void handleDeleteSessionRequestMsg_v(cmn::IpcEventMessage *eMsg);
    void handleReleaseAccessBearerRequestMsg_v(cmn::IpcEventMessage *eMsg);
    void handleDownlinkDataNotificationAckMsg_v(cmn::IpcEventMessage *eMsg);
};

#endif /* INCLUDE_S11_APP_MSGHANDLERS_S11MSGHANDLER_H_ */
