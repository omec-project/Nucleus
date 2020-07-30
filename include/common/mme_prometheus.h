#ifndef __INCLUDE_COMMON_MME_PROMETHEUS_H_
#define __INCLUDE_COMMON_MME_PROMETHEUS_H_
#include <prometheus/gauge.h>
#include <prometheus/counter.h>
#include <prometheus/exposer.h>
#include <prometheus/registry.h>
using namespace prometheus;
extern std::shared_ptr<Registry> registry;
#endif


