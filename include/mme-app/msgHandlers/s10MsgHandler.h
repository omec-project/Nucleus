/*
 * Copyright 2019-present Infosys Limited  
 *   
 * SPDX-License-Identifier: Apache-2.0    
 */

#ifndef INCLUDE_MME_APP_MSGHANDLERS_S10MSGHANDLER_H_
#define INCLUDE_MME_APP_MSGHANDLERS_S10MSGHANDLER_H_

#include <stdint.h>
#include <eventMessage.h>

class S10MsgHandler {
public:
	static S10MsgHandler* Instance();
	~GtpMsgHandler();

	void handleS10Message_v(cmn::IpcEMsgUnqPtr eMsg);

private:
	S10MsgHandler();

	void handleIdentificationRequestMsg_v(cmn::IpcEMsgUnqPtr eMsg, uint32_t ueIdx);
	void handleIdentificationResponseMsg_v(cmn::IpcEMsgUnqPtr eMsg, uint32_t ueIdx);
};

#endif /* INCLUDE_MME_APP_MSGHANDLERS_GTPMSGHANDLER_H_ */
