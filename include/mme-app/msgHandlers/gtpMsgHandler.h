/*
 * Copyright 2019-present Infosys Limited  
 *   
 * SPDX-License-Identifier: Apache-2.0    
 */

#ifndef INCLUDE_MME_APP_MSGHANDLERS_GTPMSGHANDLER_H_
#define INCLUDE_MME_APP_MSGHANDLERS_GTPMSGHANDLER_H_

#include <stdint.h>
#include <eventMessage.h>

class GtpMsgHandler {
public:
	static GtpMsgHandler* Instance();
	~GtpMsgHandler();

	void handleGtpMessage_v(cmn::IpcEMsgUnqPtr eMsg);

private:
	GtpMsgHandler();

	void handleCreateSessionResponseMsg_v(cmn::IpcEMsgUnqPtr eMsg, uint32_t ueIdx);
	void handleModifyBearerResponseMsg_v(cmn::IpcEMsgUnqPtr eMsg, uint32_t ueIdx);
	void handleDeleteSessionResponseMsg_v(cmn::IpcEMsgUnqPtr eMsg, uint32_t ueIdx);
	void handleReleaseBearerResponseMsg_v(cmn::IpcEMsgUnqPtr eMsg, uint32_t ueIdx);
	void handleDdnMsg_v(cmn::IpcEMsgUnqPtr eMsg, uint32_t ueIdx);
	void handleCreateBearerRequestMsg_v(cmn::IpcEMsgUnqPtr eMsg, uint32_t ueIdx);
	void handleDeleteBearerRequestMsg_v(cmn::IpcEMsgUnqPtr eMsg, uint32_t ueIdx);
};

#endif /* INCLUDE_MME_APP_MSGHANDLERS_GTPMSGHANDLER_H_ */
