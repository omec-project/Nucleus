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

    static S1HandoverProcedureContext*
    allocateHoContext(SM::ControlBlock& cb_r);

    static MmeSmCreateBearerProcCtxt*
	allocateCreateBearerRequestProcedureCtxt(SM::ControlBlock& cb_r, uint8_t bearerId);

    static SmDedActProcCtxt*
	allocateDedBrActivationProcedureCtxt(SM::ControlBlock& cb_r, uint8_t bearerId);

    static MmeSmDeleteBearerProcCtxt*
        allocateDeleteBearerRequestProcedureCtxt(SM::ControlBlock& cb_r, uint8_t bearerId);

    static SmDedDeActProcCtxt*
        allocateDedBrDeActivationProcedureCtxt(SM::ControlBlock& cb_r, uint8_t bearerId);

    static MmeProcedureCtxt* findProcedureCtxt(SM::ControlBlock& cb_r, ProcedureType procType, uint8_t bearerId = 0);

	static bool deleteProcedureCtxt(MmeProcedureCtxt* procedure_p);
	static bool deallocateProcedureCtxt(SM::ControlBlock& cb_r, MmeProcedureCtxt* procedure_p);
	static bool deallocateAllProcedureCtxts(SM::ControlBlock& cb_r);

	static void deleteUEContext(uint32_t cbIndex, bool deleteControlBlockFlag=true);
	static void deleteAllSessionContext(SM::ControlBlock& cb_r);

    static SessionContext*
    allocateSessionContext (SM::ControlBlock &cb_r, UEContext &uectxt_r);
    static BearerContext*
    allocateBearerContext (SM::ControlBlock &cb_r, UEContext &ueCtxt_r,
            SessionContext &sessionCtxt_r);
    static void
    deallocateSessionContext(SM::ControlBlock& cb_r,
            SessionContext *sessionCtxt_p, UEContext *ueCtxt_p=NULL);
    static void
    deallocateBearerContext(SM::ControlBlock& cb_r, BearerContext *bearerCtxt_p,
            SessionContext *sessionCtxt_p=NULL, UEContext *ueCtxt_p=NULL);

    static BearerContext*
	findBearerContext(uint8_t bearerId, UEContext *ueCtxt_p, SessionContext *sessionCtxt_p=NULL);

    static SessionContext* findSessionCtxtForEpsBrId(uint8_t bearerId, UEContext *ueCtxt_p);

private:
	MmeContextManagerUtils();
	~MmeContextManagerUtils();

};
}



#endif /* INCLUDE_MME_APP_UTILS_MMECONTEXTMANAGERUTILS_H_ */
