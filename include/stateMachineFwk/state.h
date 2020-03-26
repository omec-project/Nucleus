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

#ifndef STATE_H_
#define STATE_H_

#include <string>
#include <map>
#include "smTypes.h"

using namespace std;

namespace SM
{
	class State
	{
  	public:
		State(uint16_t sID);
      		virtual ~State();

	      	virtual void display();

      		ActStatus executeActions(uint16_t evtId,ControlBlock& cb);

	      	uint16_t getStateId() const;


   	protected:
      		uint16_t stateID;
	      	EventToActionTableMap eventToActionsMap;
	};
}
#endif /* STATE_H_ */
