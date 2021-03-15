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
#include <interfaces/mmeIpcInterface.h>
#include <mme_app.h>
#include "log.h"

#define DATA_BUF_SIZE 4096

using namespace cmn;
using namespace cmn::ipc;
using namespace cmn::utils;

extern cmn::utils::BlockingCircularFifo<cmn::IpcEventMessage, fifoQSize_c> mmeIpcIngressFifo_g;
extern cmn::utils::BlockingCircularFifo<cmn::IpcEventMessage, fifoQSize_c> mmeIpcEgressFifo_g;

class MmeIngressIpcProducerThread
{
public:
	void operator()()
	{
		uint16_t bytesRead = 0;
		cmn::ipc::IpcAddress srcAddr;
		unsigned char buf[DATA_BUF_SIZE] = {0};
		MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));

		while(1)
		{
			if ((bytesRead = mmeIpcIf.reader()->recvMsgFrom(buf, DATA_BUF_SIZE, srcAddr)) > 0 )
			{
                // allocating memory max size so that nas message can be expanded on same meory footprint 
				cmn::IpcEventMessage *ipcMsg = new cmn::IpcEventMessage(DATA_BUF_SIZE); 
				MsgBuffer *msgBuf = ipcMsg->getMsgBuffer();
				msgBuf->writeBytes(buf, bytesRead);
				msgBuf->rewind();
				if (!mmeIpcIngressFifo_g.push(ipcMsg))
				{
					log_msg(LOG_ERROR,"Failed to enqueue tipc message. Drop message");
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
        MmeIpcInterface &mmeIpcIf =
                static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));

		while(1)
		{
			cmn::IpcEventMessage* eMsg = NULL;
			while(mmeIpcIngressFifo_g.pop(eMsg) == true)
			{
			    IpcEMsgUnqPtr eMsgPtr(eMsg);
				mmeIpcIf.handleIpcMsg(std::move(eMsgPtr));
			}
		}
	}
};

class MmeEgressIpcConsumerThread
{
public:
	void operator()()
	{
        MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>
                        (compDb.getComponent(MmeIpcInterfaceCompId));

		while(1)
		{
			cmn::IpcEventMessage* eMsg = NULL;
			while(mmeIpcEgressFifo_g.pop(eMsg) == true)
			{
				if (eMsg != NULL)
				{
					MsgBuffer * msgBuf = eMsg->getMsgBuffer();
					if (msgBuf != NULL)
					{
						cmn::ipc::IpcMsgHeader ipcHdr;
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
