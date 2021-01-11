 /*
 * Copyright 2019-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef INCLUDE_MME_APP_UTILS_MMEPROCEDURETYPES_H_
#define INCLUDE_MME_APP_UTILS_MMEPROCEDURETYPES_H_
#include <list>
#include <structs.h>
namespace mme
{

 enum AttachType
 {
	 invalidAttachType_c,

	 imsiAttach_c,
	 knownGutiAttach_c,
	 unknownGutiAttach_c,

	 maxAttachType_c
 };

 enum ProcedureType
 {
    	invalidProcedureType_c,

    	defaultMmeProcedure_c,
    	attach_c,
    	detach_c,
    	s1Release_c,
    	serviceRequest_c,
    	tau_c,
    	s1Handover_c,
        erabModInd_c,
	cbReq_c,
	dedBrActivation_c,
	dbReq_c,
	dedBrDeActivation_c,

    	maxProcedureType_c
 };

 enum DetachType
 {
    	invalidDetachType_c,

    	mmeInitDetach_c,
    	hssInitDetach_c,
    	ueInitDetach_c,
	pgwInitDetach_c,

    	maxDetachtype_c
 };

 enum UE_State_e
 {
        InvalidState,

        NoState,
        EpsAttached,
        EpsDetached,

        maxUeState
 };
 using EmmState = UE_State_e;

 typedef uint32_t PagingTrigger;
 const PagingTrigger none_c = 0x00;
 const PagingTrigger ddnInit_c = 0x01;
 const PagingTrigger pgwInit_c = 0x02;
 const PagingTrigger hssInit_c = 0x04;

 enum EcmState{
	ecmUnknown_c,
	ecmIdle_c,
	ecmConnected_c,
	maxEcmState_c
 };

 enum S1ReleaseTrigger
 {
       noTrigger_c,
       enbInit_c,
       mmeInit_c,
       maxS1ReleaseTrigger_c
 };

 enum HoType
 {
 	invalid_c,
 	intraMmeS1Ho_c
 };

 struct BearerStatusCompareById
 {
     uint8_t bearerId;
     BearerStatusCompareById(uint8_t id):bearerId(id)
     { }

     bool operator()(const BearerCtxtCBResp& lhs)
     {
         if (lhs.bearer_ctxt_cb_resp_m.eps_bearer_id == bearerId)
             return true;
         else
             return false;
     }

     bool operator()(const BearerCtxtDBResp& lhs)
     {
         if (lhs.eps_bearer_id == bearerId)
             return true;
         else
             return false;
     }
 };

}

#endif /* INCLUDE_MME_APP_UTILS_MMEPROCEDURETYPES_H_ */
