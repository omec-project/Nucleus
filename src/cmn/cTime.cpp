/*
 * Copyright 2020-present Infosys Limited  
 *   
 * SPDX-License-Identifier: Apache-2.0    
 */

#include "cTime.h"

CTime::CTime()
{
    gettimeofday(&time_m, NULL);
}

CTime::CTime(const struct timeval &timeVal)
{
    time_m.tv_sec = timeVal.tv_sec;
    time_m.tv_usec = timeVal.tv_usec;
}

CTime::CTime(uint64_t ms)
{
    time_m.tv_sec = ms/1000;
    time_m.tv_usec = (ms % 1000) * 1000;
}

CTime::CTime(const CTime& in)
{
    time_m.tv_sec = in.time_m.tv_sec;
    time_m.tv_usec = in.time_m.tv_usec;
}

CTime::~CTime()
{

}

void CTime::setTime(struct timeval& val)
{
    time_m.tv_sec = val.tv_sec;
    time_m.tv_usec = val.tv_usec;
}

const struct timeval& CTime::getTime()const
{
	return time_m;
}

void CTime::addDuration(const struct timeval& duration)
{
    time_m.tv_sec += duration.tv_sec;
    time_m.tv_usec += duration.tv_usec;

    if (time_m.tv_usec > 1000000)
    {
        time_m.tv_sec += (time_m.tv_usec/1000000);
        time_m.tv_usec -= 1000000;
    }
}

bool CTime::operator <(const CTime& rhs) const
{
    bool rc = false;

    if (time_m.tv_sec < rhs.time_m.tv_sec)
    {
        rc = true;
    }
    else if (time_m.tv_sec == rhs.time_m.tv_sec)
    {
        if (time_m.tv_usec < rhs.time_m.tv_usec)
        {
            rc = true;
        }
    }
    return rc;
}

bool CTime::operator >(const CTime& rhs) const
{
    return (rhs < *this);
}

bool CTime::operator==(const CTime& rhs) const
{
    return (time_m.tv_sec == rhs.time_m.tv_sec &&
            time_m.tv_usec == rhs.time_m.tv_usec);
}

bool CTime::operator<=(const CTime& rhs) const
{
    return (!(*this > rhs));
}

CTime CTime::operator+(const CTime& rhs)
{
    CTime res(*this);
    res.addDuration(rhs.getTime());

    return res;
}

CTime& CTime::operator=(const CTime& rhs)
{
    if (this != &rhs)
    {
        time_m.tv_sec = rhs.time_m.tv_sec;
        time_m.tv_usec = rhs.time_m.tv_usec;
    }
    return *this;
}
