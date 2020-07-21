/*
 * Copyright 2020-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <controlBlock.h>
#include <contextManager/dataBlocks.h>
#include <state.h>
#include <timeoutManager.h>
#include <timerQueue.h>
#include <utils/mmeContextManagerUtils.h>
#include <utils/mmeStatesUtils.h>
#include <utils/mmeTimerTypes.h>
#include <utils/mmeTimerUtils.h>

using namespace mme;
using namespace SM;

/***************************************
* MmeStatesUtils : on_state_entry
***************************************/
ActStatus MmeStatesUtils::on_state_entry(ControlBlock& cb)
{
    // Check and start state guard timers
    MmeProcedureCtxt *procedure_p =
            dynamic_cast<MmeProcedureCtxt*>(cb.getTempDataBlock());
    if (procedure_p == NULL)
    {
        return ActStatus::HALT;
    }

    if (procedure_p->getCtxtType() != defaultMmeProcedure_c)
    {
        SM::State *currentState_p = procedure_p->getCurrentState();

        if (currentState_p != NULL)
        {
            uint32_t timeoutVal =
                    currentState_p->getStateGuardTimeoutDuration();
            if (timeoutVal > 0)
            {
                MmeUeTimerContext *timerCtxt =
                        static_cast<MmeUeTimerContext*>(MmeTimerUtils::startTimer(
                                timeoutVal,
                                cb.getCBIndex(),
                                stateGuardTimer_c,  // timer type
                                currentState_p->getStateId())); // treat state id as timerId

                if (timerCtxt != NULL)
                    procedure_p->setStateGuardTimerCtxt(timerCtxt);
            }
        }
    }
    return ActStatus::PROCEED;
}

/***************************************
* MmeStatesUtils : on_state_exit
***************************************/
ActStatus MmeStatesUtils::on_state_exit(ControlBlock &cb)
{
    // Check and stop state guard timers
    MmeProcedureCtxt *procedure_p =
            static_cast<MmeProcedureCtxt*>(cb.getTempDataBlock());
    if (procedure_p == NULL)
    {
        return ActStatus::HALT;
    }

    if (procedure_p->getCtxtType() != defaultMmeProcedure_c)
    {
        MmeTimerUtils::stopTimer(procedure_p->getStateGuardTimerCtxt());
        procedure_p->setStateGuardTimerCtxt(NULL);
    }
    return ActStatus::PROCEED;
}

/***************************************
 * MmeStatesUtils : validate_event
 ***************************************/
EventStatus MmeStatesUtils::validate_event(ControlBlock &cb,
        TempDataBlock *tempDataBlock, SM::Event &event)
{
    EventStatus rc = EventStatus::IGNORE;

    switch (event.getEventId())
    {
    case STATE_GUARD_TIMEOUT:
    {
        cmn::TimeoutMessage *eMsg =
                static_cast<cmn::TimeoutMessage*>(event.getEventData());

        if (eMsg != NULL)
        {
            MmeUeTimerContext *timerCtxt =
                    static_cast<MmeUeTimerContext*>(eMsg->getTimerContext());

            if (timerCtxt != NULL)
            {
                uint16_t timerId =
                        tempDataBlock->getCurrentState()->getStateId();

                // Check if we are in same state or state has already changed.
                // This happens when an event for the state was received and before
                // it got processed, the timeout event was fired by the timer thread.
                // If so, we need to ignore the guard timer expiry event.
                if (timerCtxt->getTimerId() == timerId)
                {
                    rc = EventStatus::CONSUME;
                }
            }
        }
    }
        break;
    default:
    {
        rc = EventStatus::CONSUME;
    }
    }
    return rc;
}

