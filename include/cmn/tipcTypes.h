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

#ifndef INCLUDE_COMMONFWK_TIPCTYPES_H_
#define INCLUDE_COMMONFWK_TIPCTYPES_H_

#include <stdint.h>

static const uint32_t tipcServiceAddressType_c = 18888;
static const uint32_t tipcDomain_c = 0x0;

typedef enum TipcServiceInstance
{
	mmeAppInstanceNum_c = 1,
	s1apAppInstanceNum_c,
	s6AppInstanceNum_c,
	s11AppInstanceNum_c,

	maxInstanceNum_c

} TipcInstanceTypes;

#endif /* INCLUDE_COMMONFWK_TIPCTYPES_H_ */
