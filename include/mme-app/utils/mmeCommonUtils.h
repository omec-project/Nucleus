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

#ifndef INCLUDE_MME_APP_UTILS_MMECOMMONUTILS_H_
#define INCLUDE_MME_APP_UTILS_MMECOMMONUTILS_H_

#include <stdint.h>
#include <utils/mmeProcedureTypes.h>
#include <msgType.h>
#include <s1ap_structs.h>

struct guti;

namespace SM
{
	class ControlBlock;
}

namespace cmn
{
	namespace utils
	{
		class MsgBuffer;
	}
}

namespace mme
{
	class MmeProcedureCtxt;
	class UEContext;
	class MmeCommonUtils
	{
	public:
		static bool isLocalGuti(const guti& guti_r);
		static uint32_t allocateMtmsi();

		static SM::ControlBlock* findControlBlock(cmn::utils::MsgBuffer* msgData_p);

		static AttachType getAttachType(UEContext* ueCtxt_p, const struct ue_attach_info& attachReqMsg_r);

		static void formatS1apPlmnId(struct PLMN* plmn_p);

		static bool isEmmInfoRequired(SM::ControlBlock& cb, UEContext& ueCtxt, MmeProcedureCtxt& procCtxt);

	private:
		MmeCommonUtils();
		~MmeCommonUtils();
	};
}

#endif /* INCLUDE_MME_APP_UTILS_MMECOMMONUTILS_H_ */
