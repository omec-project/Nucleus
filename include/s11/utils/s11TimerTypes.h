/*
 * Copyright 2020-present Open Networking Foundation
 * Copyright 2020-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef INCLUDE_S11_APP_UTILS_S11TIMERTYPES_H_
#define INCLUDE_S11_APP_UTILS_S11TIMERTYPES_H_

#include <stdint.h>
#include <timerQueue.h>

namespace s11
{
/******************************************************
 * Timer ID range used by different modules in mme-app
 ******************************************************/
const uint16_t transTimer_c = 1;

/******************************************************
 * Timeout durations in milliseconds
 ******************************************************/
const uint32_t defaultStateGuardTimerDuration_c = 5000;


class gtpTransData;
class s11TimerContext : public TimerContext
{
public:
    s11TimerContext(gtpTransData *trans,
            uint16_t timerType,
            uint16_t timerId,
            CTime& expiryTime):
        TimerContext(expiryTime, timerType, timerId),
        trans_m(trans)
    {
    }

    ~s11TimerContext()
    {
    }
    gtpTransData *getTrans() { return trans_m; }

private:
    gtpTransData *trans_m;
};
}



#endif /* INCLUDE_S11_APP_UTILS_S11TIMERTYPES_H_ */
