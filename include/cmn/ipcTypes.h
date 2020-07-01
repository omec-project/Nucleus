 /*
 * Copyright 2019-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef INCLUDE_CMN_IPCTYPES_H_
#define INCLUDE_CMN_IPCTYPES_H_

#include <stdint.h>

namespace cmn {
namespace ipc {

enum class IpcChannelType
{
	tipc_c,
	maxIpcType_c
};

union IpcChannelHdl
{
	uint32_t u32;
};

union IpcAddress
{
	uint32_t u32;
};

typedef struct IpcMsgHeader
{
	IpcAddress destAddr;
	IpcAddress srcAddr;

} IpcMsgHeader;

} /* namespace ipc */
} /* namespace cmn */




#endif /* INCLUDE_CMN_IPCTYPES_H_ */
