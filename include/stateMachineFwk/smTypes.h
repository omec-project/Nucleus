/*
 * Copyright (c) 2019, Infosys Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef INCLUDE_STATEMACHFWK_SMTYPES_H_
#define INCLUDE_STATEMACHFWK_SMTYPES_H_

#include <map>
#include <event.h>

using namespace std;

namespace cmn {
class EventMessage;
}

namespace SM{

class ControlBlock;
class ActionTable;
class TempDataBlock;
class Event;

/******************************************************************
 * enum ActStatus
 * Indicates the status of execution of an Action.
 * PROCEED: Continues execution of next action
 * in the action table. Next State is set only if the ActStatus
 * is PROCEED
 * ABORT: Aborts execution of remaining actions. In addition,
 * SM frameworks fires a default 'ABORT_EVENT' on the current tempdata block.
 * Action handlers for ABORT_EVENT should not use 'ABORT' return codes.
 * BREAK: Stops execution of subsequent actions. Next State is
 * also not set.
 * HALT: Stops execution of subsequent actions. Any remaining events
 * queued in the event q will be ignored.
 */
enum ActStatus
{
	PROCEED,
	ABORT,
	BREAK,
    HALT
};

/******************************************************************
 * enum EventStatus
 * Used by the event validator to denote whether an event is
 * intended for a specific procedure block or not.
 * IGNORE : Event will be ignored by the procedure block.
 * CONSUME: Event will be processed by the procedure block.
 * FORWARD: Event is not meant for the current procedure block, SM forwards
 * the event to the next procedure block in the list for its validation.
 * CONSUME_AND_FORWARD: Event is to be consumed by the current procedure
 * as well as forwarded to other procedures for their validation.
 */
enum EventStatus
{
    IGNORE,
    CONSUME,
    FORWARD,
    CONSUME_AND_FORWARD,
    INVALID_STATUS
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

//SM specific Error Codes
const uint8_t CURRENT_STATE_NULL = 0;

using ActionPointer = ActStatus(*)(ControlBlock&);
using EventValidator = EventStatus(*)(ControlBlock&, TempDataBlock*, Event &event);
using EventToActionTableMap = std::map <uint16_t, ActionTable>;
using EventIdToStringMap = std::map <uint16_t, std::string>;
using StateMachineExceptionCallbk = void(*)(ControlBlock&, uint8_t);

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

    void addEventToStrEntry(uint16_t eventId, std::string eventName)
    {
        eventToStringMap.insert(
                std::pair<uint16_t, std::string>(eventId, eventName));
    }

private:
    SmUtility() :
            eventToStringMap()
    {

    }

    ~SmUtility()
    {

    }

    EventIdToStringMap eventToStringMap;
};

class StateMachineContext
{
public:
    Event evt;
    TempDataBlock* tempDataBlock_mp;

    StateMachineContext():evt(),
            tempDataBlock_mp(NULL)
    {
    }

    void clear()
    {
        tempDataBlock_mp = NULL;
        static Event dummyEvt;
        evt = dummyEvt;
    }
};

}

#endif /* INCLUDE_STATEMACHFWK_SMTYPES_H_ */
