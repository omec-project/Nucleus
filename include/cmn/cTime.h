/*
 * Copyright 2020-present Infosys Limited  
 *   
 * SPDX-License-Identifier: Apache-2.0    
 */

#ifndef TIMERS_CTIME_H_
#define TIMERS_CTIME_H_

#include <sys/time.h>
#include <stdint.h>
#include <cstddef>

class CTime
{
public:
    CTime();
    CTime(const struct timeval &time);
    CTime(uint64_t ms);
    CTime(const CTime& cTime);

    ~CTime();

    void setTime(struct timeval& val);
    const struct timeval& getTime() const;

    void addDuration(const struct timeval& val);

    bool operator<(const CTime& rhs) const;
    bool operator>(const CTime& rhs) const;
    bool operator==(const CTime& rhs) const;
    bool operator<=(const CTime& rhs) const;
    CTime operator+(const CTime& rhs);
    CTime& operator=(const CTime& rhs);

private:
    struct timeval time_m;
};

#endif /* TIMERS_CTIME_H_ */
