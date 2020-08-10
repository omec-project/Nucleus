/*
 * Copyright 2020-present Open Networking Foundation
 * Copyright 2020-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef INCLUDE_S11_APP_UTILS_S11TIMERUTILS_H_
#define INCLUDE_S11_APP_UTILS_S11TIMERUTILS_H_

#include <stdint.h>

class s11TimerContext;
class MmeUeTimerContext;
class gtpTransData;

class s11TimerUtils
{
public:
    static s11TimerContext* startTimer(uint32_t durationMs,
                                    uint16_t timerType,
                                    uint16_t timerId, 
                                    gtpTransData *trans);

    static uint32_t stopTimer(TimerContext* timerCtxt);

    static void onTimeout(TimerContext* timerCtxt);

private:
    s11TimerUtils();
    ~s11TimerUtils();
};

#endif 
