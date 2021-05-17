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

#include <iostream>
#include <string>
#include <time.h>
#include <msgBuffer.h>
#include "stateMachineEngine.h"
#include "controlBlock.h"
#include "event.h"
#include "state.h"
#include "smTypes.h"
extern "C"
{
#include "log.h"
}
using namespace std;

namespace SM
{

StateMachineEngine::StateMachineEngine() :
        procQ_m()
{
}

StateMachineEngine::~StateMachineEngine()
{
}

StateMachineEngine* StateMachineEngine::Instance()
{
    static StateMachineEngine SM;
    return &SM;
}

bool StateMachineEngine::addCBToProcQ(ControlBlock *cb)
{
    return procQ_m.push(cb);
}

ActStatus StateMachineEngine::handleProcedureEvent(ControlBlock &cb,
        StateMachineContext &smCtxt)
{
    State *currentState_p = smCtxt.tempDataBlock_mp->getCurrentState();

    SmUtility *util = SmUtility::Instance();
    log_msg(LOG_DEBUG,
            "### Executing actions for event: %s and State: %s ###",
            util->convertEventToString(smCtxt.evt.getEventId()).c_str(),
            currentState_p->getStateName());

    time_t mytime = time(NULL);
    debugEventInfo dEventInfo(smCtxt.evt.getEventId(),
            currentState_p->getStateName(), mytime);
    cb.addDebugInfo(dEventInfo);

    return currentState_p->executeActions(smCtxt.evt.getEventId(), cb);
}

void StateMachineEngine::registerSMExceptionCb(StateMachineExceptionCallbk smExceptionCb)
{
    smExceptionCb_m = smExceptionCb;
}

void StateMachineEngine::handleSMExceptionCb(ControlBlock& cb, uint8_t err_code)
{
    smExceptionCb_m(cb, err_code);
}

void StateMachineEngine::run()
{

    ControlBlock *cb = procQ_m.pop();

    if (cb == NULL)
        return;

    if (cb->getControlBlockState() == FREE)
    {
        log_msg(LOG_ERROR, "A freed Control Block found in Procedure Queue");
        return;
    }

    StateMachineContext &smCtxt = cb->getStateMachineContext();

    while (true) // Event loop
    {
        if (cb->getNextEvent(smCtxt.evt) == false)
        {
            break;
        }

        TempDataBlock *tempData_p = cb->getFirstTempDataBlock(); //head of the temp data block list
        TempDataBlock *nextTempData_p = NULL;

        while (tempData_p != NULL) // Temp Block loop

        {
            nextTempData_p = tempData_p->getNextTempDataBlock();

            State *currentState_p = tempData_p->getCurrentState();
            if (currentState_p == NULL)
            {
                handleSMExceptionCb(*cb, CURRENT_STATE_NULL);
                break;
            }

            ActStatus ret = PROCEED;

            EventStatus eStatus = currentState_p->validateEvent(*cb, tempData_p, smCtxt.evt);

            log_msg(LOG_INFO, "Event Status %d", eStatus);

            switch (eStatus)
            {
            case IGNORE:
            {
                tempData_p = NULL;

            }
                break;
            case CONSUME:
            case CONSUME_AND_FORWARD:
            {
                smCtxt.tempDataBlock_mp = tempData_p;

                ret = handleProcedureEvent(*cb, smCtxt);

                if (ret == ABORT)
                {
                    log_msg(LOG_INFO, "Abort Event Initiated ");

                    uint16_t currentEventId = smCtxt.evt.getEventId();

                    // Abort on same temp data block.
                    smCtxt.evt.setEventId((uint16_t) ABORT_EVENT);
                    ret = handleProcedureEvent(*cb, smCtxt);

                    // revert to the original event and let other procedure
                    // decide what to do with this event (in case this is
                    // a consume and forward event)
                    smCtxt.evt.setEventId(currentEventId);
                }

                if (eStatus == CONSUME || cb->getControlBlockState() == FREE)
                {
                    tempData_p = NULL;
                }
                else
                {
                    tempData_p = nextTempData_p;
                }

            }
                break;
            case FORWARD:
            {
                tempData_p = nextTempData_p;

            }
                break;
            default:
            {
                tempData_p = NULL;
            }
            }

            if (ret == HALT)
                break;

        }
        smCtxt.clear();
    }
    cb->resetProcQueueFlag();
}
}
