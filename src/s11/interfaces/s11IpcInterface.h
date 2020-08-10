/*
 * Copyright 2020-present Open Networking Foundation
 *   
 * SPDX-License-Identifier: Apache-2.0    
 */

#ifndef INCLUDE_S11_APP_INTERFACES_S11IPCINTERFACE_H_
#define INCLUDE_S11_APP_INTERFACES_S11IPCINTERFACE_H_

#include <ipcChannel.h>
#include <componentDb.h>
#include "msgType.h"

namespace cmn{
    class IpcEventMessage;
namespace utils{
    class MsgBuffer;
}
}

class s11IpcInterface:public cmn::ComponentIf {

public:
	s11IpcInterface();
	virtual ~s11IpcInterface();

	bool setup();
	void teardown();

	cmn::ipc::IpcChannel* sender();
	cmn::ipc::IpcChannel* reader();

	void handleIpcMsg(cmn::IpcEventMessage* buf);

	bool dispatchIpcMsg(gtp_incoming_msg_data_t* buf, uint32_t len, cmn::ipc::IpcAddress& destAddr);

private:
	cmn::ipc::IpcChannel* sender_mp;
	cmn::ipc::IpcChannel* reader_mp;
};

#endif /* INCLUDE_MME_APP_INTERFACES_MMEIPCINTERFACE_H_ */
