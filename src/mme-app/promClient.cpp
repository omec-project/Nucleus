/*
 * Copyright 2020-present Open Networking Foundation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <iostream>
#include "mme_prometheus.h"
#include <chrono>
#include <map>
#include <memory>
#include <thread>
#include <promClient.h>

using namespace prometheus;

std::shared_ptr<Registry> registry;

void promThreadSetup(void)
{
    registry = std::make_shared<Registry>();
    statistics *instance = statistics::Instance();
    Exposer exposer{"0.0.0.0:3081", 1};
    std::string debugMetrics("/metrics");
    exposer.RegisterCollectable(registry, debugMetrics);
    while(1)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

statistics::statistics():
            ue_info(BuildGauge().Name("number_of_ue_attached").Help("Number of UE attached").Labels({{"current_imsis", "current_subscribers"}}).Register(*registry)),
            current_sub(ue_info.Add({{"imsi", "value"}}))
{
}

statistics* statistics::Instance()
{
    static statistics object;
    return &object;
}

void statistics::ue_detached(mme::UEContext *ue)
{
    current_sub.Decrement();
}

void statistics::ue_attached(mme::UEContext *ue)
{
   current_sub.Increment();
}
