/*
 * Copyright 2019-present Infosys Limited  
 *   
 * SPDX-License-Identifier: Apache-2.0    
 */

#ifndef INCLUDE_MME_APP_INTERFACES_MMEIPCINTERFACE_H_
#define INCLUDE_MME_APP_INTERFACES_MMEIPCINTERFACE_H_

#include <ipcChannel.h>
#include <componentDb.h>

namespace cmn{
    class IpcEventMessage;
namespace utils{
    class MsgBuffer;
}
}

class MmeIpcInterface:public cmn::ComponentIf {

public:
	MmeIpcInterface();
	virtual ~MmeIpcInterface();

	bool setup();
	void teardown();

	cmn::ipc::IpcChannel* sender();
	cmn::ipc::IpcChannel* reader();

	void handleIpcMsg(cmn::IpcEventMessage* buf);

	bool dispatchIpcMsg(char* buf, uint32_t len, cmn::ipc::IpcAddress& destAddr);

private:
	cmn::ipc::IpcChannel* sender_mp;
	cmn::ipc::IpcChannel* reader_mp;
};

#endif /* INCLUDE_MME_APP_INTERFACES_MMEIPCINTERFACE_H_ */
