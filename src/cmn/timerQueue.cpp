/*
 * Copyright 2020-present Infosys Limited  
 *   
 * SPDX-License-Identifier: Apache-2.0    
 */

#include "timerQueue.h"
#include <iostream>

TimerContext::TimerContext(const CTime &expiryTime,
        uint16_t timerType, uint16_t timerId):
    expiryTime_m(expiryTime),
    timerType_m(timerType),
    timerId_m(timerId)
{

}

TimerContext::~TimerContext()
{

}

void TimerContext::setExpiryTime(const CTime& expiryTime)
{
    expiryTime_m = expiryTime;
}

const CTime& TimerContext::getExpiryTime() const
{
    return expiryTime_m;
}

uint16_t TimerContext::getTimerType() const
{
    return timerType_m;
}

uint16_t TimerContext::getTimerId() const
{
    return timerId_m;
}

TimerQueue::TimerQueue():mutex_m(), container_m()
{

}

TimerQueue::~TimerQueue()
{

}

void TimerQueue::addTimerInQueue(TimerContext* item)
{
    std::unique_lock<std::mutex> lock(mutex_m);
    container_m.insert(item);
}

uint32_t TimerQueue::removeTimerInQueue(TimerContext* item)
{
    std::unique_lock<std::mutex> lock(mutex_m);
    return container_m.erase(item);
}

void TimerQueue::onTimer(Callback appCb)
{
    CTime now;

    while(true)
    {
        std::unique_lock<std::mutex> lock(mutex_m);
        std::set<TimerContext*>::iterator itr = container_m.begin();
        if (itr != container_m.end())
        {
            TimerContext* timerCtxt = *itr;
            if (timerCtxt->getExpiryTime() <= now)
            {
                itr = container_m.erase(itr);
                mutex_m.unlock();
                appCb(timerCtxt);
                continue; 
            }
        }
        break; // no more timer or no more timers need expiry handling 
    }
}
