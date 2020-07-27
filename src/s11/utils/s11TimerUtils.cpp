/*
 * Copyright 2020-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <timeoutManager.h>
#include <timerQueue.h>
#include <utils/s11TimerUtils.h>
#include <utils/s11TimerTypes.h>
#include "s11.h"

using namespace s11;
using namespace cmn;

extern TimeoutManager* timeoutMgr_g;

TimerContext* s11TimerUtils::startTimer( uint32_t durationMs,
        uint32_t ueIdx,
        uint16_t timerType,
        uint16_t timerId)
{
    s11TimerContext* timerCtxt = NULL;
    
    TimeoutManager &timeoutMgr =
    		static_cast<TimeoutManager&>(
    	    			compDb.getComponent(TimeoutManagerCompId));

    CTime duration(durationMs);
    CTime expiryTime;
    expiryTime = expiryTime + duration;
    
    timerCtxt = new s11TimerContext(
                ueIdx, timerType, timerId, expiryTime);
    
    timeoutMgr.startTimer(timerCtxt);
    
    log_msg(LOG_DEBUG,
                "Timer started. duration %d", durationMs);
    return timerCtxt;
}

uint32_t s11TimerUtils::stopTimer(TimerContext* timerCtxt)
{
    uint32_t rc = 0;

    TimeoutManager &timeoutMgr =
    		static_cast<TimeoutManager&>(
    				compDb.getComponent(TimeoutManagerCompId));

    if (timerCtxt != NULL)
    {
        rc = timeoutMgr.cancelTimer(timerCtxt);
        if (rc > 0)
        {
            log_msg(LOG_DEBUG, "Timer deleted\n");
            delete timerCtxt;
        }
    }
    return rc;
}

void s11TimerUtils::onTimeout(TimerContext* timerCtxt)
{
    s11TimerContext* mmeTimerCtxt = static_cast<s11TimerContext *>(timerCtxt);
    if (mmeTimerCtxt == NULL)
    {
        return;
    }
}
