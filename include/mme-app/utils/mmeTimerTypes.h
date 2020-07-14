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
const uint16_t mmeConfigDnsResolve_c = 2;

/******************************************************
 * Timeout durations in milliseconds
 ******************************************************/
const uint32_t defaultStateGuardTimerDuration_c = 5000;


class MmeUeTimerContext : public TimerContext
{
public:
    MmeUeTimerContext(uint32_t ueIdx,
            uint16_t timerType,
            uint16_t timerId,
            CTime& expiryTime):
        TimerContext(expiryTime, timerType, timerId),
        ueIndex_m(ueIdx)
    {
    }

    ~MmeUeTimerContext()
    {
    }

    uint32_t getUeIndex() const
    {
        return ueIndex_m;
    }

private:
    uint32_t ueIndex_m;
};
}



#endif /* INCLUDE_MME_APP_UTILS_MMETIMERTYPES_H_ */
