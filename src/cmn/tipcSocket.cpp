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

#include "../../include/cmn/tipcSocket.h"
#include "../../include/cmn/tipcTypes.h"
#include "../../include/common/log.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <linux/tipc.h>

#define TIPC_SERVICE_ADDR       2

namespace cmn {
namespace ipc {

TipcSocket::TipcSocket():IpcChannel(IpcChannelType::tipc_c)
{
	initialize();
}

TipcSocket::~TipcSocket()
{
	if (ipcHdl_m.u32 > 0)
		close(ipcHdl_m.u32);
}

void TipcSocket::initialize()
{
	ipcHdl_m.u32 = socket(AF_TIPC, SOCK_RDM, 0);
    if (ipcHdl_m.u32 <= 0)
        log_msg(LOG_ERROR, "Failed to create TIPC socket. error: %s", strerror(errno));

    return;
}

bool TipcSocket::bindTipcSocket(IpcAddress myAddress)
{
	if (ipcHdl_m.u32 <= 0)
	{
		log_msg(LOG_ERROR, "Invalid socket fd");
		return false;
	}

    struct sockaddr_tipc server;

    server.family = AF_TIPC;
    server.addrtype = TIPC_SERVICE_ADDR;
    server.scope = TIPC_CLUSTER_SCOPE;
    server.addr.name.name.type = tipcServiceAddressType_c;
    server.addr.name.name.instance = myAddress.u32;

    bool rc = true;

    if (0 != bind(ipcHdl_m.u32, (sockaddr*)&server, sizeof(server)))
    {
    	log_msg(LOG_ERROR, "Failed to bind TIPC socket. error: %s\n", strerror(errno));
        rc = false;
    }
    return rc;
}

void TipcSocket::sendMsgTo(void * buffer, uint32_t len, IpcAddress destAddress)
{
    struct sockaddr_tipc dest;
    dest.family = AF_TIPC;
    dest.addrtype = TIPC_SERVICE_ADDR;
    dest.scope = TIPC_CLUSTER_SCOPE;
    dest.addr.name.domain = 0;
    dest.addr.name.name.type = tipcServiceAddressType_c;
    dest.addr.name.name.instance = destAddress.u32;


   if (0 > sendto(ipcHdl_m.u32, buffer, len, 0, (sockaddr*)&dest, sizeof(dest)))
   {
	   log_msg(LOG_ERROR, "Failed to send message via TIPC socket. error: %s\n", strerror(errno));
   }
   else
   {
	   log_msg(LOG_INFO, "Message sent successfully\n");
   }
}


uint32_t TipcSocket::recvMsgFrom(void * buffer, uint32_t len, IpcAddress& srcAddr)
{
    uint32_t bytesRead = 0;

    struct sockaddr_tipc client;
    socklen_t alen = sizeof(client);

    if ((bytesRead = recvfrom(ipcHdl_m.u32, buffer, len, 0,
			    (struct sockaddr *)&client, &alen)) > 0) 
    {
    	srcAddr.u32 = client.addr.name.name.instance;
    }

    return bytesRead;
}

} /* namespace ipc */
} /* namespace cmn */
