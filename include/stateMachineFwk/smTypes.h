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

#ifndef INCLUDE_STATEMACHFWK_SMTYPES_H_
#define INCLUDE_STATEMACHFWK_SMTYPES_H_

#include <map>

using namespace std;

namespace SM{

class ControlBlock;
class State;
class ActionTable;

enum ActStatus
{
	PROCEED,
	HALT,
	ABORT
};

enum ControlBlockState
{
    FREE,
    ALLOCATED,
    MAX_STATE
};

// Generic Events ranging from 0 to 100
const uint16_t DEFAULT_EVENT = 0;
const uint16_t ABORT_EVENT = 1;
const uint16_t STATE_GUARD_TIMEOUT = 2;

//Generic States 
const uint16_t default_state = 0;

using ActionPointer = ActStatus(*)(ControlBlock&);
using EventToActionTableMap = std::map <uint16_t, ActionTable>;
using EventIdToStringMap = std::map <uint16_t, std::string>;
using StateIdToStringMap = std::map <uint16_t, std::string>;

class SmUtility
{
public:
    static SmUtility* Instance()
    {
        static SmUtility smUtility_p;
        return &smUtility_p;
    }

    std::string convertEventToString(uint16_t eventId)
    {
        std::string eventName = " ";
        auto itr = eventToStringMap.find(eventId);
        if (itr != eventToStringMap.end())
        {
            eventName = itr->second;
        }
        return eventName;
    }

    std::string convertStateToString(uint16_t stateId)
    {
        std::string stateName = " ";
        auto itr = stateToStringMap.find(stateId);
        if (itr != stateToStringMap.end())
        {
            stateName = itr->second;
        }
        return stateName;
    }

    void addEventToStrEntry(uint16_t eventId, std::string eventName)
    {
        eventToStringMap.insert(
                std::pair<uint16_t, std::string>(eventId, eventName));
    }

    void addStateToStrEntry(uint16_t stateId, std::string stateName)
    {
        stateToStringMap.insert(
                std::pair<uint16_t, std::string>(stateId, stateName));
    }

private:
    SmUtility() :
            eventToStringMap(), stateToStringMap()
    {

    }

    ~SmUtility()
    {

    }

    EventIdToStringMap eventToStringMap;
    StateIdToStringMap stateToStringMap;

};
        

}

#endif /* INCLUDE_STATEMACHFWK_SMTYPES_H_ */
