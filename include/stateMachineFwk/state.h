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
#include "event.h"

using namespace std;

namespace SM
{
	class State
	{
  	public:
	        State(uint16_t sID);

	        State(uint16_t sID, uint32_t duration);

      		virtual ~State();

	      	virtual void display();

	      	ActStatus executeActions(uint16_t evtId,ControlBlock& cb);

	      	uint16_t getStateId() const;

	      	// Sets the action to be executed when a state is set
	      	void setEntryAction(ActionPointer entryAction);

	      	// Sets the action to be executed when the temp data
	      	// block leaves a state
	      	void setExitAction(ActionPointer exitAction);

	      	// Invokes the state entry action
	      	void OnEntry(ControlBlock& cb);

	      	// Invokes the state exit action
	      	void OnExit(ControlBlock& cb);

	      	// Check if an event is valid for a state and
	      	// temp block
	      	EventStatus validateEvent(ControlBlock& cb,
	      	        TempDataBlock* tempData, Event& event);

	      	void setStateGuardTimeoutDuration(uint32_t timeoutDuration);

	      	uint32_t getStateGuardTimeoutDuration() const;


   	protected:
      		uint16_t stateID;
	      	EventToActionTableMap eventToActionsMap;

	      	ActionPointer stateEntryAction;
	      	ActionPointer stateExitAction;
	      	EventValidator eventValidator;

	      	uint32_t stateGuardTimeoutDuration_m;
	};
}
#endif /* STATE_H_ */
