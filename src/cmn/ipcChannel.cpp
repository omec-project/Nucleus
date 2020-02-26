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

#include "../../include/cmn/ipcChannel.h"

namespace cmn {
namespace ipc {

IpcChannel::IpcChannel(IpcChannelType ipcType):
		ipcType_m(ipcType),
		ipcHdl_m()
{

}

IpcChannel::~IpcChannel()
{

}

IpcChannelType IpcChannel::getIpcChannelType() const
{
	return ipcType_m;
}

void IpcChannel::setIpcChannelType(const IpcChannelType ipcType)
{
	ipcType_m = ipcType;
}

IpcChannelHdl IpcChannel::getIpcChannelHdl() const
{
	return ipcHdl_m;
}

void IpcChannel::setIpcChannelHdl(const IpcChannelHdl ipcHdl, const IpcChannelType ipcType)
{
	if (ipcType == IpcChannelType::tipc_c)
		ipcHdl_m.u32 = ipcHdl.u32;
}

} /* namespace ipc */
} /* namespace cmn */
