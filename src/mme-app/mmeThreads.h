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

#ifndef SRC_MME_APP_MMETHREADS_H_
#define SRC_MME_APP_MMETHREADS_H_

#include <blockingCircularFifo.h>
#include <ipcTypes.h>
#include <msgBuffer.h>
#include <interfaces/mmeIpcInterface.h>
#include <mme_app.h>

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
				cmn::IpcEventMessage *ipcMsg = new cmn::IpcEventMessage(bytesRead);
				MsgBuffer *msgBuf = ipcMsg->getMsgBuffer();
				msgBuf->writeBytes(buf, bytesRead);
				msgBuf->rewind();
				if (!mmeIpcIngressFifo_g.push(ipcMsg))
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
			while(mmeIpcIngressFifo_g.pop(eMsg) == true)
			{
				MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>(compDb.getComponent(MmeIpcInterfaceCompId));   
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
			while(mmeIpcEgressFifo_g.pop(eMsg) == true)
			{
				if (eMsg != NULL)
				{
					MsgBuffer * msgBuf = eMsg->getMsgBuffer();
					if (msgBuf != NULL)
					{
						cmn::ipc::IpcMsgHeader ipcHdr;
						MmeIpcInterface &mmeIpcIf = static_cast<MmeIpcInterface&>
										(compDb.getComponent(MmeIpcInterfaceCompId));
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
