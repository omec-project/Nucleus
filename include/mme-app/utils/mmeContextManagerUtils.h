 /*
 * Copyright 2019-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef INCLUDE_MME_APP_UTILS_MMECONTEXTMANAGERUTILS_H_
#define INCLUDE_MME_APP_UTILS_MMECONTEXTMANAGERUTILS_H_

#include <utils/mmeProcedureTypes.h>
#include <stdint.h>

namespace SM
{
class ControlBlock;
}

namespace mme
{

class MmeProcedureCtxt;
class S1HandoverProcedureContext;
class MmeContextManagerUtils
{
public:

    static MmeDetachProcedureCtxt*
    allocateDetachProcedureCtxt(SM::ControlBlock& cb_r, DetachType detachType);

    static MmeSvcReqProcedureCtxt*
    allocateServiceRequestProcedureCtxt( SM::ControlBlock& cb_r, PagingTrigger pagingTrigger);

    static MmeTauProcedureCtxt*
    allocateTauProcedureCtxt(SM::ControlBlock& cb_r);

    static MmeErabModIndProcedureCtxt*
    allocateErabModIndProcedureCtxt(SM::ControlBlock& cb_r);

    static MmeProcedureCtxt* findProcedureCtxt(SM::ControlBlock& cb_r, ProcedureType procType);

	static bool deleteProcedureCtxt(MmeProcedureCtxt* procedure_p);
	static bool deallocateProcedureCtxt(SM::ControlBlock& cb_r, ProcedureType procType);
	static bool deallocateAllProcedureCtxts(SM::ControlBlock& cb_r);

	static void deleteUEContext(uint32_t cbIndex, bool deleteControlBlockFlag=true);
	static void deleteSessionContext(SM::ControlBlock& cb_r);

	static S1HandoverProcedureContext* allocateHoContext(SM::ControlBlock& cb_r);

private:
	MmeContextManagerUtils();
	~MmeContextManagerUtils();
};
}



#endif /* INCLUDE_MME_APP_UTILS_MMECONTEXTMANAGERUTILS_H_ */
