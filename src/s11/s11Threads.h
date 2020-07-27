/*
 * Copyright 2019-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SRC_MME_APP_MMETHREADS_H_
#define SRC_MME_APP_MMETHREADS_H_

#include <blockingCircularFifo.h>
#include <ipcTypes.h>
#include <msgBuffer.h>
#include <interfaces/s11IpcInterface.h>
#include <s11.h>
#include "eventMessage.h"

#define DATA_BUF_SIZE 4096

using namespace cmn;
using namespace cmn::ipc;
using namespace cmn::utils;

extern cmn::utils::BlockingCircularFifo<cmn::IpcEventMessage, fifoQSize_c> s11IpcIngressFifo_g;
extern cmn::utils::BlockingCircularFifo<cmn::IpcEventMessage, fifoQSize_c> s11IpcEgressFifo_g;

class MmeIngressIpcProducerThread
{
public:
	void operator()()
	{
		uint16_t bytesRead = 0;
		cmn::ipc::IpcAddress srcAddr;
		unsigned char buf[DATA_BUF_SIZE] = {0};
		s11IpcInterface &mmeIpcIf = static_cast<s11IpcInterface&>(compDb.getComponent(S11IpcInterfaceCompId));

		while(1)
		{
			if ((bytesRead = mmeIpcIf.reader()->recvMsgFrom(buf, DATA_BUF_SIZE, srcAddr)) > 0 )
			{
				cmn::IpcEventMessage *ipcMsg = new cmn::IpcEventMessage(bytesRead);
				MsgBuffer *msgBuf = ipcMsg->getMsgBuffer();
				msgBuf->writeBytes(buf, bytesRead);
				msgBuf->rewind();
				if (!s11IpcIngressFifo_g.push(ipcMsg))
				{
					delete ipcMsg;
				}
			}
		}
	}
};

class MmeIngressIpcConsumerThread
{
public:
	void operator()()
	{
		while(1)
		{
			cmn::IpcEventMessage* eMsg = NULL;
			while(s11IpcIngressFifo_g.pop(eMsg) == true)
			{
				s11IpcInterface &mmeIpcIf = static_cast<s11IpcInterface&>(compDb.getComponent(S11IpcInterfaceCompId));   
				mmeIpcIf.handleIpcMsg(eMsg);
			}
		}
	}
};

class MmeEgressIpcConsumerThread
{
public:
	void operator()()
	{
		while(1)
		{
			cmn::IpcEventMessage* eMsg = NULL;
			while(s11IpcEgressFifo_g.pop(eMsg) == true)
			{
				if (eMsg != NULL)
				{
					MsgBuffer * msgBuf = eMsg->getMsgBuffer();
					if (msgBuf != NULL)
					{
						cmn::ipc::IpcMsgHeader ipcHdr;
						s11IpcInterface &mmeIpcIf = static_cast<s11IpcInterface&>
										(compDb.getComponent(S11IpcInterfaceCompId));
						msgBuf->rewind();
						msgBuf->readUint32(ipcHdr.destAddr.u32);
						msgBuf->readUint32(ipcHdr.srcAddr.u32);
						mmeIpcIf.sender()->sendMsgTo(msgBuf->getDataPointer(), 
								msgBuf->getLength(), ipcHdr.destAddr);
					}
					delete eMsg;
				}
			}
		}
	}
};

#endif /* SRC_MME_APP_MMETHREADS_H_ */
