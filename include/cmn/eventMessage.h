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
    const uint16_t IPC_EMSG = 0x01;
    const uint16_t TIMEOUT_EMSG = 0x02;
    const uint16_t DNS_RESP_EMSG = 0x04;

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
    
    class DnsEventMessage :public EventMessage
    {
        public:
            DnsEventMessage (uint64_t ip ):
                EventMessage(DNS_RESP_EMSG), ipAddress(ip)
            {
            }
	    
           virtual ~DnsEventMessage ()
            {

            }
	    uint64_t getIPAddress() 
            {
		return ipAddress;
            }

        private:
            uint64_t ipAddress;
    };

}

#endif /* INCLUDE_CMN_EVENTMESSAGE_H_ */
