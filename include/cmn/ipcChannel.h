/*
 * Copyright (c) 2019, Infosys Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
