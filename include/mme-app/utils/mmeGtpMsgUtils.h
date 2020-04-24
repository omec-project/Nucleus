/*
* Copyright 2020-present, Infosys Ltd.
*
* SPDX-License-Identifier: Apache-2.0
*/
#ifndef INCLUDE_MME_APP_UTILS_MMEGTPMSGUTILS_H_
#define INCLUDE_MME_APP_UTILS_MMEGTPMSGUTILS_H_



#include <stdint.h>
#include <msgType.h>

namespace SM
{
	class ControlBlock;
}
namespace mme
{
	class S1HandoverProcedureContext;
	class UEContext;
	class SessionContext;
	class MmeGtpMsgUtils
	{
	public:
		static void populateModifyBearerRequestHo(SM::ControlBlock& cb,
		        UEContext& ueCtxt,
		        SessionContext& sessCtxt,
				S1HandoverProcedureContext& procCtxt,
				struct MB_Q_msg& mbMsg);
	private:
		MmeGtpMsgUtils();
		~MmeGtpMsgUtils();
	};
}

#endif /* INCLUDE_MME_APP_UTILS_MMEGTPMSGUTILS_H_ */
