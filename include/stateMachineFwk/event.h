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
      		Event(uint16_t evtID, cmn::EventMessage * eventData);
      		virtual ~Event();

      		inline uint16_t getEventId()const
      		{
      			return eventID;
      		}

      		cmn::EventMessage * getEventData() const;
      		virtual void display();
 
   	private:
	      	uint16_t eventID;
	      	cmn::EventMessage * eventData_p;
	};
}

#endif /* EVENT_H_ */
