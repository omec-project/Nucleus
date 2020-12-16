/*
 * Copyright (c) 2019, Infosys Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef EVENT_H_
#define EVENT_H_

#include <string>
#include <map>
#include <eventMessage.h>

using namespace std;

namespace SM
{
	class Event
	{
   	public:
	        Event();
      		Event(uint16_t evtID, cmn::EventMsgShPtr eventData);
      		virtual ~Event();

      		inline uint16_t getEventId()const
      		{
      			return eventID;
      		}

      		inline void setEventId(uint16_t evt)
      		{
      		    eventID = evt;
      		}

      		cmn::EventMsgShPtr getEventData() const;
      		virtual void display();
 
   	private:
	      	uint16_t eventID;
	      	cmn::EventMsgShPtr eventData;
	};
}

#endif /* EVENT_H_ */
