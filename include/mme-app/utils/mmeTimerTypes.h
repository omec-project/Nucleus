/*
 * Copyright 2020-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef INCLUDE_MME_APP_UTILS_MMETIMERTYPES_H_
#define INCLUDE_MME_APP_UTILS_MMETIMERTYPES_H_

#include <stdint.h>
#include <timerQueue.h>

namespace mme
{
/******************************************************
 * Timer ID range used by different modules in mme-app
 ******************************************************/
const uint16_t stateGuardTimer_c = 1;

/******************************************************
 * Timeout durations in milliseconds
 ******************************************************/
const uint32_t defaultStateGuardTimerDuration_c = 5000;


class MmeTimerContext : public TimerContext
{
public:
    MmeTimerContext(uint32_t cbIdx,
            uint16_t timerType,
            uint16_t timerId,
            CTime& expiryTime):
        TimerContext(expiryTime),
        controlBlockIdx_m(cbIdx),
        timerType_m(timerType),
        timerId_m(timerId)
    {
    }

    ~MmeTimerContext()
    {
    }

    uint16_t getTimerType() const
    {
        return timerType_m;
    }

    uint16_t getTimerId() const
    {
        return timerId_m;
    }

    uint32_t getCbIndex() const
    {
        return controlBlockIdx_m;
    }

private:
    uint32_t controlBlockIdx_m;
    uint16_t timerType_m;
    uint16_t timerId_m;
};
}



#endif /* INCLUDE_MME_APP_UTILS_MMETIMERTYPES_H_ */
