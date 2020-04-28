/*
 * Copyright 2020-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef INCLUDE_MME_APP_UTILS_MMETIMERUTILS_H_
#define INCLUDE_MME_APP_UTILS_MMETIMERUTILS_H_

#include <stdint.h>

class TimerContext;
namespace mme
{
class MmeUeTimerContext;

class MmeTimerUtils
{
public:
    static TimerContext* startTimer(uint32_t durationMs,
                                        uint32_t ueIdx,
                                        uint16_t timerType,
                                        uint16_t timerId);
    static uint32_t stopTimer(TimerContext* timerCtxt);
    static void onTimeout(TimerContext* timerCtxt);

private:
    MmeTimerUtils();
    ~MmeTimerUtils();
};
}

#endif /* INCLUDE_MME_APP_UTILS_MMETIMERUTILS_H_ */
