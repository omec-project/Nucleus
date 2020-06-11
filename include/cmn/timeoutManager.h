/*
 * Copyright 2020-present Infosys Limited  
 *   
 * SPDX-License-Identifier: Apache-2.0    
 */

#ifndef TIMERS_TIMEOUTMANAGER_H_
#define TIMERS_TIMEOUTMANAGER_H_

#include <componentDb.h>
#include "timerQueue.h"

#include <thread>

class TimeoutManager:public cmn::ComponentIf
{
public:
    TimeoutManager(Callback cb);
    ~TimeoutManager();

    void startTimer(TimerContext* ctxt);
    uint32_t cancelTimer(TimerContext* ctxt);

    void run();

protected:
    class Timer
    {
    public:
        const uint32_t defaultTimeoutDurationNanoSeconds_c = 1000000;
        const uint32_t defaultTimeoutDurationSeconds_c = 0;

        Timer();
        Timer(const struct itimerspec &tspec);
        ~Timer();

        void initTimer();

        bool startTimer();
        bool stopTimer();

        int getTimerFd() const;

    private:
        struct itimerspec tspec_m;
        int fd_m;
    };

private:
    TimerQueue timerQueue_m;
    Timer timer_m;
    std::thread thread_m;
    Callback appCb_m;
};


#endif /* TIMERS_TIMEOUTMANAGER_H_ */
