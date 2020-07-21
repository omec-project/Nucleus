/*
* Copyright 2020-present, Infosys Ltd.
*
* SPDX-License-Identifier: Apache-2.0
*/
#ifndef INCLUDE_MME_APP_UTILS_MMES1MSGUTILS_H_
#define INCLUDE_MME_APP_UTILS_MMES1MSGUTILS_H_



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
	class MmeS1MsgUtils
	{
	public:

		static void populateHoRequest(SM::ControlBlock& cb, UEContext& ueCtxt,
				S1HandoverProcedureContext& procCtxt, struct handover_request_Q_msg& horeq);
		static void populateHoCommand(SM::ControlBlock& cb, UEContext& ueCtxt,
				S1HandoverProcedureContext& procCtxt, struct handover_command_Q_msg& hoCommand);

	private:
		MmeS1MsgUtils();
		~MmeS1MsgUtils();
	};
}

#endif /* INCLUDE_MME_APP_UTILS_MMES1MSGUTILS_H_ */
