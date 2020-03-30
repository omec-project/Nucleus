/*
 * Copyright 2020-present Infosys Limited  
 *   
 * SPDX-License-Identifier: Apache-2.0    
 */

#include "timerQueue.h"

TimerContext::TimerContext(const CTime &expiryTime):
    expiryTime_m(expiryTime)
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

TimerQueue::TimerQueue():mutex_m(), container_m()
{

}

TimerQueue::~TimerQueue()
{

}

void TimerQueue::add(TimerContext* item)
{
    std::unique_lock<std::mutex> lock(mutex_m);
    container_m.insert(item);
}

uint32_t TimerQueue::remove(TimerContext* item)
{
    std::unique_lock<std::mutex> lock(mutex_m);
    return container_m.erase(item);
}

void TimerQueue::onTimer(Callback appCb)
{
   CTime now;

   std::unique_lock<std::mutex> lock(mutex_m);

   std::set<TimerContext*>::iterator itr = container_m.begin();
   while (itr != container_m.end())
   {
       TimerContext* timerCtxt = *itr;
       if ((timerCtxt->getExpiryTime() < now) ||
               (timerCtxt->getExpiryTime() == now))
       {
           itr = container_m.erase(itr);
           appCb(timerCtxt);
       }
       else
       {
           break;
       }
   }
}
