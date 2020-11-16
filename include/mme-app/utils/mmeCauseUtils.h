/*
 * Copyright 2020-present, Infosys Limited.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef INCLUDE_MME_APP_UTILS_MMECAUSEUTILS_H_
#define INCLUDE_MME_APP_UTILS_MMECAUSEUTILS_H_

#include <stdint.h>
#include <structs.h>

namespace mme
{
	class MmeCauseUtils
	{
		public:
			static uint32_t convertToNasEmmCause(ERROR_CODES mmeErrorCause);
			static uint32_t convertToGtpCause(ERROR_CODES mmeErrorCause);
			static S1apCause convertToS1apCause(ERROR_CODES mmeErrorCause);
			static uint32_t convertToGtpCause(s1apCause_t s1apCause);
			static uint32_t convertToGtpCause(esm_cause_t esmCause);
	
		private:
			MmeCauseUtils();
			~MmeCauseUtils();
	};
}

#endif /* INCLUDE_MME_APP_UTILS_MMECAUSETYPES_H_ */


