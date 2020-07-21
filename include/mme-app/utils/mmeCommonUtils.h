 /*
 * Copyright 2019-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef INCLUDE_MME_APP_UTILS_MMECOMMONUTILS_H_
#define INCLUDE_MME_APP_UTILS_MMECOMMONUTILS_H_

#include <stdint.h>
#include <utils/mmeProcedureTypes.h>
#include <msgType.h>
#include <s1ap_structs.h>

#define MAX_ALGO_COUNT 8
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

		static SM::ControlBlock* findControlBlockForS11Msg(cmn::utils::MsgBuffer* msg_p);

		static AttachType getAttachType(UEContext* ueCtxt_p, const struct ue_attach_info& attachReqMsg_r);

		static void formatS1apPlmnId(struct PLMN* plmn_p);
        static uint8_t select_preferred_int_algo(uint8_t &val);
        static uint8_t select_preferred_sec_algo(uint8_t &val);

		static bool isEmmInfoRequired(SM::ControlBlock& cb, UEContext& ueCtxt, MmeProcedureCtxt& procCtxt);


	private:
		MmeCommonUtils();
		~MmeCommonUtils();
	};
}

#endif /* INCLUDE_MME_APP_UTILS_MMECOMMONUTILS_H_ */
