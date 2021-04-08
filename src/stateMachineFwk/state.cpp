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

#include <string>
#include <iostream>
#include "state.h"
#include "actionTable.h"
#include "smTypes.h"
#include "log.h"

using namespace std;


namespace SM
{
    State::State()
          :eventToActionsMap(),
           stateEntryAction(NULL),
           stateExitAction(NULL),
           eventValidator(NULL),
           stateGuardTimeoutDuration_m(0)
    {
    }

	State::~State()
	{
	}

	void State::display()	
	{
		for(auto& eventToActionsMapEntry : eventToActionsMap)
		{
			log_msg(LOG_DEBUG, "Event Id = %d ", eventToActionsMapEntry.first);
			ActionTable& act = eventToActionsMapEntry.second;
			act.display();
		}
	}

    void State::setEntryAction(ActionPointer entryAction)
    {
        stateEntryAction = entryAction;
    }
    void State::setExitAction(ActionPointer exitAction)
    {
        stateEntryAction = exitAction;
    }
    void State::OnEntry(ControlBlock& cb)
    {
        stateEntryAction(cb);
    }
    void State::OnExit(ControlBlock& cb)
    {
        stateExitAction(cb);
    }
    EventStatus State::validateEvent(ControlBlock& cb, TempDataBlock* tempData, Event& event)
    {
        return eventValidator(cb, tempData, event);
    }

    void State::setStateGuardTimeoutDuration(uint32_t duration)
    {
        stateGuardTimeoutDuration_m = duration;
    }

    uint32_t State::getStateGuardTimeoutDuration() const
    {
        return stateGuardTimeoutDuration_m;
    }

    bool State::isEventHandled(uint16_t eventId)
    {
        bool handleEvent = false;

        auto ret = eventToActionsMap.find(eventId);
        if (ret != eventToActionsMap.end())
            handleEvent = true;

        return handleEvent;
    }

	uint16_t State::getStateId()const
	{
	    return 0;
	}

    const char* State::getStateName()const
    {
        return "";
    }

	ActStatus State::executeActions(uint16_t evt,ControlBlock& cb)
	{
		EventToActionTableMap::iterator itr = eventToActionsMap.find(evt);

		if (itr != eventToActionsMap.end())
		{
			ActionTable& actions_r = itr->second;
			return actions_r.executeActions(cb, this);
		}
		else
			return ActStatus::HALT;
	}
}
