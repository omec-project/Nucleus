 /*
 * Copyright 2019-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef INCLUDE_CMN_IPCCHANNEL_H_
#define INCLUDE_CMN_IPCCHANNEL_H_

#include "ipcTypes.h"

namespace cmn {
namespace ipc {

class IpcChannel {
public:
	IpcChannel(IpcChannelType ipcType);
	virtual ~IpcChannel();

	IpcChannelType getIpcChannelType() const;
	void setIpcChannelType(const IpcChannelType ipcType);

	IpcChannelHdl getIpcChannelHdl() const;
	void setIpcChannelHdl(const IpcChannelHdl ipcHdl, const IpcChannelType ipcType);

	virtual void sendMsgTo(void * buffer, uint32_t len, IpcAddress destAddress) = 0;
	virtual uint32_t recvMsgFrom(void * buffer, uint32_t bufLen, IpcAddress& srcAddress) = 0;

protected:
	IpcChannelType ipcType_m;
	IpcChannelHdl ipcHdl_m;
};

} /* namespace ipc */
} /* namespace cmn */

#endif /* INCLUDE_CMN_IPCCHANNEL_H_ */
