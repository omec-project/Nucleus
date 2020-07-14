/*
 * Copyright (c) 2019, Infosys Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef INCLUDE_STATEMACHFWK_SMTYPES_H_
#define INCLUDE_STATEMACHFWK_SMTYPES_H_

#include <map>

using namespace std;

namespace SM{

class ControlBlock;
class ActionTable;
class TempDataBlock;
class Event;

enum ActStatus
{
	PROCEED,
	HALT,
	ABORT
};

/* IGNORE : Event will be ignored by the current procedure
 * CONSUME: Event will be processed by the current procedure */
enum EventStatus
{
    IGNORE,
    CONSUME
    // TODO: Concurrent procedure support
    // FORWARD,
    // CONSUME_AND_FORWARD,
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
using EventValidator = EventStatus(*)(ControlBlock&, TempDataBlock*, Event &event);
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
