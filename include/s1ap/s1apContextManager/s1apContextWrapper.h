/*
 * Copyright 2019-present Open Networking Foundation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
 
#ifndef __S1apContextWrappers_H
#define __S1apContextWrappers_H


#include "stdint.h"

#include "s1_common_types.h"


	uint32_t createControlBlock_cpp();
    uint32_t findControlBlockWithEnbId_cpp(uint32_t enbId);
    uint32_t findControlBlockWithEnbFd_cpp(uint32_t enbFd);
    uint32_t getEnbFdWithCbIndex_cpp(uint32_t cbIndex);
    uint32_t setValuesForEnbCtx_cpp(uint32_t cbIndex, 
                                struct EnbStruct* enbCtx, bool update);
    uint32_t updateValuesForEnbCtx_cpp(uint32_t cbIndex, 
                                struct EnbStruct* enbCtx);

    bool clearControlBlockDetailsEnbFd_cpp(uint32_t enbFd, struct EnbStruct *);
#endif
