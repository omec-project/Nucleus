 /*
 * Copyright 2019-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef INCLUDE_MME_APP_UTILS_MMECAUSETYPES_H_
#define INCLUDE_MME_APP_UTILS_MMECAUSETYPES_H_

#include <stdint.h>

namespace mme
{

typedef uint32_t MmeErrorCause;

const MmeErrorCause noError_c = 0x00;
const MmeErrorCause ueContextNotFound_c = 0x01;
const MmeErrorCause networkTimeout_c = 0x02;
const MmeErrorCause secModeRespFailure_c = 0x03;
const MmeErrorCause s6AiaFailure_c = 0x04;
const MmeErrorCause hoRequestAckFailure_c = 0x05;
const MmeErrorCause s11MBRespFailure_c = 0x06;
const MmeErrorCause abortDueToAttachCollision_c = 0x07;
}

#endif /* INCLUDE_MME_APP_UTILS_MMECAUSETYPES_H_ */
