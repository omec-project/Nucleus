 /*
 * Copyright 2019-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
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
