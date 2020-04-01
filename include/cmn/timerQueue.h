/*
 * Copyright 2020-present Infosys Limited  
 *   
 * SPDX-License-Identifier: Apache-2.0    
 */

#ifndef TIMERS_TIMERQUEUE_H_
#define TIMERS_TIMERQUEUE_H_

#include <functional>
#include <memory>
#include <mutex>
#include <set>

#include <cTime.h>

class TimerContext;
typedef std::function<void(TimerContext*)> Callback;

class TimerContext
{
public:
    friend class TimerQueue;

    TimerContext(const CTime &expiryTime,
            uint16_t timerType, uint16_t timerId);
    virtual ~TimerContext();

    const CTime& getExpiryTime() const;
    void setExpiryTime(const CTime& expiryTime);
    uint16_t getTimerType() const;
    uint16_t getTimerId() const;

private:
    CTime expiryTime_m;
    uint16_t timerType_m;
    uint16_t timerId_m;
};

class TimerQueue
{
public:
    TimerQueue();
    ~TimerQueue();

    void addTimerInQueue(TimerContext* item);
    uint32_t removeTimerInQueue(TimerContext* item);

    void onTimer(Callback cb);

private:
    struct TimeCompare
    {
        bool operator() (const TimerContext* lhs, const TimerContext* rhs) const
        {
            return (lhs->expiryTime_m < rhs->expiryTime_m);
        }
    };

    std::mutex mutex_m;
    std::set<TimerContext*, TimeCompare> container_m;
};

#endif /* TIMERS_TIMERQUEUE_H_ */
