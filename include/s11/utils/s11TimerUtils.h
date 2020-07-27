/*
 * Copyright 2020-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef INCLUDE_S11_APP_UTILS_S11TIMERUTILS_H_
#define INCLUDE_S11_APP_UTILS_S11TIMERUTILS_H_

#include <stdint.h>

class TimerContext;
namespace s11
{
class MmeUeTimerContext;

class s11TimerUtils
{
public:
    static TimerContext* startTimer(uint32_t durationMs,
                                        uint32_t ueIdx,
                                        uint16_t timerType,
                                        uint16_t timerId);
    static uint32_t stopTimer(TimerContext* timerCtxt);
    static void onTimeout(TimerContext* timerCtxt);

private:
    s11TimerUtils();
    ~s11TimerUtils();
};
}

#endif 
