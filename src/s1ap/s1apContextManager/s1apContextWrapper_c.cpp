/*
 * Copyright 2019-present Open Networking Foundation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
 
#include "s1apContextManager/s1apContextWrapper_c.h"
#include "s1apContextManager/s1apContextWrapper.h"

extern "C" 
{
    uint32_t createControlBlock()
    {
        return createControlBlock_cpp();
    }

    uint32_t findControlBlockWithEnbId(
                                        uint32_t enbId)
    {
        return findControlBlockWithEnbId_cpp(enbId);
    }

    uint32_t findControlBlockWithEnbFd(
                                        uint32_t enbFd)
    {
        return findControlBlockWithEnbFd_cpp(enbFd);
    }

    uint32_t getEnbFdWithCbIndex(uint32_t cbIndex)
    {
        return getEnbFdWithCbIndex_cpp(cbIndex);
    }

    uint32_t setValuesForEnbCtx(uint32_t cbIndex, EnbStruct* enbCtx, bool update)
    {
        return setValuesForEnbCtx_cpp(cbIndex, enbCtx, update);
    }

    bool clearControlBlockDetailsEnbFd(uint32_t enbFd, EnbStruct *enb)
    {
        return clearControlBlockDetailsEnbFd_cpp(enbFd, enb);
    }

}
