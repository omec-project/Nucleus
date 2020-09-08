#ifndef __S1apContextWrappers_C_H
#define __S1apContextWrappers_C_H


#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include "s1_common_types.h"

	uint32_t createControlBlock();
    uint32_t findControlBlockWithEnbId(uint32_t enbId);
    uint32_t findControlBlockWithEnbFd(uint32_t enbFd);
    uint32_t getEnbFdWithCbIndex(uint32_t cbIndex);
    uint32_t setValuesForEnbCtx(uint32_t cbIndex, 
                                struct EnbStruct* enbCtx,
                                bool update);

    bool getControlBlockDetailsEnbFd(uint32_t sd, struct EnbStruct *temp);

#ifdef __cplusplus
}
#endif
#endif
