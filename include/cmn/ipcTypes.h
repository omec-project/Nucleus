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
