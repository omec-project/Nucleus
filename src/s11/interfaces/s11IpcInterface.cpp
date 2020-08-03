/*
 * Copyright (c) 2019, Infosys Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <interfaces/s11IpcInterface.h>
#include <blockingCircularFifo.h>
#include <eventMessage.h>
#include <ipcTypes.h>
#include <tipcSocket.h>
#include <tipcTypes.h>
#include <msgBuffer.h>
#include <s11.h>
#include <msgHandlers/s11MsgHandler.h>

extern "C" {
	#include "log.h"
}

using namespace cmn;
using namespace cmn::ipc;
using namespace cmn::utils;

extern BlockingCircularFifo<cmn::IpcEventMessage, fifoQSize_c> toMmeIpcIngressFifo_g;

s11IpcInterface::s11IpcInterface():sender_mp(), reader_mp()
{
     compDb.registerComponent(S11IpcInterfaceCompId, this);
}

s11IpcInterface::~s11IpcInterface()
{
	teardown();
}

bool s11IpcInterface::setup()
{

	// INIT socket for receiving ipc message
	TipcSocket* receiver_sock = new TipcSocket();

	IpcAddress myAddress;
	myAddress.u32 = s11AppInstanceNum_c;

	if (receiver_sock->bindTipcSocket(myAddress) == false)
	{
		log_msg(LOG_ERROR, "s11AppInstanceNum_c Setup Failed!!!\n");

		delete receiver_sock;

		return false;
	}

	// INIT socket for sending ipc message
	TipcSocket* sender_sock = new TipcSocket();

	sender_mp = sender_sock;
	reader_mp = receiver_sock;

	return true;
}

void s11IpcInterface::teardown()
{
	if (sender_mp != NULL)
		delete sender_mp;

	if (reader_mp != NULL)
		delete reader_mp;
}

cmn::ipc::IpcChannel* s11IpcInterface::sender()
{
	if (sender_mp != NULL)
		return sender_mp;
	else
		return NULL;
}

cmn::ipc::IpcChannel* s11IpcInterface::reader()
{
	if (reader_mp != NULL)
		return reader_mp;
	else
		return NULL;
}

void s11IpcInterface::handleIpcMsg(cmn::IpcEventMessage* eMsg)
{
	uint32_t srcAddr, destAddr;

	MsgBuffer *msgBuf = eMsg->getMsgBuffer();

	msgBuf->readUint32(destAddr);
	msgBuf->readUint32(srcAddr);

	log_msg(LOG_INFO, "IPC Message of size %d received from src %u to dest %u\n", msgBuf->getLength(), srcAddr, destAddr);

	switch (srcAddr)

	{
	case TipcInstanceTypes::mmeAppInstanceNum_c:
		S11MsgHandler::Instance()->handleMmeMessage_v(eMsg);
		break;
	default:
		log_msg(LOG_INFO, "IPC Message from unsupported instance\n");
	}
}

bool s11IpcInterface::dispatchIpcMsg(gtp_incoming_msg_data_t *buf, uint32_t len, cmn::ipc::IpcAddress& destAddr)
{
	cmn::ipc::IpcMsgHeader msgHeader;
	msgHeader.srcAddr.u32 = TipcInstanceTypes::s11AppInstanceNum_c;
	msgHeader.destAddr.u32 = destAddr.u32;

	cmn::IpcEventMessage* eMsg = new cmn::IpcEventMessage(
	        len + sizeof(cmn::ipc::IpcMsgHeader));
	MsgBuffer *msgBuf = eMsg->getMsgBuffer();
	msgBuf->writeUint32(msgHeader.destAddr.u32);
    msgBuf->writeUint32(msgHeader.srcAddr.u32);
	msgBuf->writeBytes((uint8_t*)(&(buf->msg_type)), len-8);

	log_msg(LOG_INFO, "Dispatch IPC msg. Len %d\n", msgBuf->getLength());

	return toMmeIpcIngressFifo_g.push(eMsg);
}

