/*
 * Copyright 2020-present Open Networking Foundation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef WRAPPERSTRUCTURES_H
#define WRAPPERSTRUCTURES_H
/**************************************
 *
 ***************************************/
#include "s1ap_structs.h"

#define INVALID_ENTRY    0xFFFFFFFF
#define INVALID_CB_INDEX 0xFFFFFFFF
struct EnbStruct
{
    int enbFd_m;
    int enbId_m;
    int s1apEnbUeId_m;
    struct TAI tai_m;
    char eNbName[128];
};

#endif
