 /*
 * Copyright 2019-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
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
