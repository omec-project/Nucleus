/*
 * Copyright 2020-present, Infosys Limited.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef INCLUDE_MME_APP_UTILS_MMECAUSEUTILS_H_
#define INCLUDE_MME_APP_UTILS_MMECAUSEUTILS_H_

#include <stdint.h>
#include <structs.h>
#include <utils/mmeCauseTypes.h>

namespace mme
{
	class MmeCauseUtils
	{
		public:
			static uint32_t convertToNasEmmCause(MmeErrorCause mmeErrorCause);
			static uint32_t convertToGtpCause(MmeErrorCause mmeErrorCause);
			static S1apCause convertToS1apCause(MmeErrorCause mmeErrorCause);
	
		private:
			MmeCauseUtils();
			~MmeCauseUtils();
	};
}

#endif /* INCLUDE_MME_APP_UTILS_MMECAUSETYPES_H_ */


