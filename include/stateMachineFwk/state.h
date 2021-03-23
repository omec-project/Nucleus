/*
 * Copyright (c) 2019, Infosys Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
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
	        State();

      		virtual ~State();

	      	virtual void display();

	      	ActStatus executeActions(uint16_t evtId,ControlBlock& cb);

	      	virtual uint16_t getStateId() const;

	      	virtual const char* getStateName() const;

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

	      	bool isEventHandled(uint16_t eventId);

   	protected:
	      	EventToActionTableMap eventToActionsMap;

	      	ActionPointer stateEntryAction;
	      	ActionPointer stateExitAction;
	      	EventValidator eventValidator;

	      	uint32_t stateGuardTimeoutDuration_m;
	};
}
#endif /* STATE_H_ */
