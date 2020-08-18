/*
 * Copyright 2020-present Open Networking Foundation
 * Copyright 2020-present Infosys Limited  
 *   
 * SPDX-License-Identifier: Apache-2.0    
 */

#ifndef INCLUDE_S11_APP_MSGHANDLERS_GTPINCOMINGMSGHANDLER_H_
#define INCLUDE_S11_APP_MSGHANDLERS_GTPINCOMINGMSGHANDLER_H_

#include <stdint.h>
#include "msgBuffer.h"
#include "gtpV2Stack.h"

namespace cmn
{
	class IpcEventMessage;
}

class gtpIncomingMsgHandler {
public:
	~gtpIncomingMsgHandler(void) {}
	gtpIncomingMsgHandler(void) {}
    static void handle_s11_message(MsgBuffer *msgBuf_p); 

private:
    static int  s11_CS_resp_handler(MsgBuffer* message, GtpV2MessageHeader* hdr, uint32_t sgw_ip);
    static int  s11_MB_resp_handler(MsgBuffer* message, GtpV2MessageHeader* hdr, uint32_t sgw_ip);
    static int  s11_DS_resp_handler(MsgBuffer* message, GtpV2MessageHeader* hdr, uint32_t sgw_ip);
    static int s11_RB_resp_handler(MsgBuffer* message, GtpV2MessageHeader* hdr, uint32_t sgw_ip);
    static int s11_DDN_handler(MsgBuffer* message, GtpV2MessageHeader* hdr, uint32_t sgw_ip);
};

#endif /* INCLUDE_S11_APP_MSGHANDLERS_S11MSGHANDLER_H_ */
