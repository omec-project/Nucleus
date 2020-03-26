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

#ifndef EVENT_H_
#define EVENT_H_

#include <string>
#include <map>

using namespace std;

namespace SM
{
	class Event
	{
   	public:
		Event();
      		Event(uint16_t evtID, void* eventData);
      		virtual ~Event();

      		inline uint16_t getEventId()const
      		{
      			return eventID;
      		}

      		void* getEventData() const;

      		virtual void display();
 
   	private:
      		uint16_t eventID;
	      	void * eventData_p;
	};
}

#endif /* EVENT_H_ */
