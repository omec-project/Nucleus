/*
 * Copyright (c) 2020, Infosys Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "err_codes.h"
#include "gtpv2c.h"
#include "s11_options.h"
#include "ipc_api.h"
#include "s11.h"
#include "s11_config.h"
#include "msgType.h"

#include <gtpV2StackWrappers.h>
/*Globals and externs*/
extern int g_resp_fd;
extern struct GtpV2Stack* gtpStack_gp;
/*End : globals and externs*/

/*S11 CP communication parameters*/
extern int g_s11_fd;
extern struct sockaddr_in g_s11_cp_addr;
extern socklen_t g_s11_serv_size;

int
s11_ECHO_req_resp_handler(MsgBuffer* message, GtpV2MessageHeader* hdr, uint32_t sgw_ip_val, uint16_t src_port)
{

	EchoRequestMsgData msgData;
	memset(&msgData, 0, sizeof(EchoRequestMsgData));

	bool rc = GtpV2Stack_decodeMessage(gtpStack_gp, hdr, message, &msgData);
	if(rc == false)
	{
			
			log_msg(LOG_ERROR, "s11_ECHO_req_resp_handler: "
					   "Failed to decode echo req Msg");
			return E_PARSING_FAILED;
	}

	struct MsgBuffer* echoRespMsgBuf_p = createMsgBuffer(S11_MSGBUF_SIZE);
	if(echoRespMsgBuf_p == NULL)
	{
	    log_msg(LOG_ERROR, "Error in initializing msg buffers required by gtp codec.");
            return -1;
	}
	
	GtpV2MessageHeader gtpHeader;
	gtpHeader.msgType =  GTP_ECHO_RSP;
	gtpHeader.teidPresent = false;
	gtpHeader.sequenceNumber = hdr->sequenceNumber;
        struct sockaddr_in sgw_ip = {0};
        create_sock_addr(&sgw_ip, src_port, sgw_ip_val);

	EchoResponseMsgData msgData_echo_resp;
	memset(&msgData_echo_resp, 0, sizeof(EchoResponseMsgData));
	
	msgData_echo_resp.recovery.restartCounter.count = 1;
	msgData_echo_resp.recovery.restartCounter.values[0] = 1;

	GtpV2Stack_buildGtpV2Message(gtpStack_gp, echoRespMsgBuf_p, &gtpHeader, &msgData_echo_resp);

	sendto(g_s11_fd,
			MsgBuffer_getDataPointer(echoRespMsgBuf_p),
			MsgBuffer_getBufLen(echoRespMsgBuf_p), 0,
			(struct sockaddr*)&sgw_ip, g_s11_serv_size);
	
	log_msg(LOG_INFO, "ECHO Resp Sent, len - %d bytes.", MsgBuffer_getBufLen(echoRespMsgBuf_p));
	MsgBuffer_free(echoRespMsgBuf_p);
	return SUCCESS;

}
