/*
 * Copyright (c) 2019, Infosys Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
}

#endif /* INCLUDE_MME_APP_UTILS_MMECAUSETYPES_H_ */
