/*
 * Copyright 2020-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef INCLUDE_CMN_EVENTMESSAGE_H_
#define INCLUDE_CMN_EVENTMESSAGE_H_

#include <stdint.h>
#include <msgBuffer.h>
#include <timerQueue.h>

namespace cmn
{
    // Add generic messages here.
    // Start app specfic message ids from 0x64
    const uint16_t IPC_EMSG = 0x01;
    const uint16_t TIMEOUT_EMSG = 0x02;

	class EventMessage
	{
		public:
			EventMessage(uint16_t msgId):msgId_m(msgId)
			{
			}

			virtual ~EventMessage ()
			{
			}

			uint16_t getMsgId()const
			{
				return msgId_m;
			}

		protected:
			uint16_t msgId_m;
	};

    class IpcEventMessage:public EventMessage
    {
        public:
	        IpcEventMessage(uint16_t msgSize):
	            EventMessage(IPC_EMSG), msgBuffer_p(NULL)
            {
	            msgBuffer_p = new MsgBuffer(msgSize);

            }

            IpcEventMessage (MsgBuffer* buf_p):
                EventMessage(IPC_EMSG), msgBuffer_p(buf_p)
            {
            }

            ~IpcEventMessage ()
            {
                if (msgBuffer_p != NULL)
                {
                    delete msgBuffer_p;
                }
            }

            MsgBuffer* getMsgBuffer()
            {
                return msgBuffer_p;
            }

            void setMsgBuffer(MsgBuffer* buf_p)
            {
            	msgBuffer_p = buf_p;
            }

        private:
            MsgBuffer* msgBuffer_p;
    };

    class TimeoutMessage :public EventMessage
    {
        public:
            TimeoutMessage (TimerContext* timerctxt):
                EventMessage(TIMEOUT_EMSG), timerctxt_p(timerctxt)
            {
            }

            ~TimeoutMessage ()
            {
                delete timerctxt_p;
            }

            TimerContext* getTimerContext()
            {
                return timerctxt_p;
            }

        private:
            TimerContext* timerctxt_p;
    };

    // Smart Pointer typedefs
    using EventMsgUnqPtr = std::unique_ptr<EventMessage>;
    using IpcEMsgUnqPtr = std::unique_ptr<IpcEventMessage>;
    using TimeoutEMsgUnqPtr = std::unique_ptr<TimeoutMessage>;

    using EventMsgShPtr = std::shared_ptr<EventMessage>;
    using IpcEMsgShPtr = std::shared_ptr<IpcEventMessage>;
    using TimeoutEMsgShPtr = std::shared_ptr<TimeoutMessage>;
}

#endif /* INCLUDE_CMN_EVENTMESSAGE_H_ */
