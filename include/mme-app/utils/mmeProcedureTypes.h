 /*
 * Copyright 2019-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef INCLUDE_MME_APP_UTILS_MMEPROCEDURETYPES_H_
#define INCLUDE_MME_APP_UTILS_MMEPROCEDURETYPES_H_

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

    	maxProcedureType_c
 };

 enum DetachType
 {
    	invalidDetachType_c,

    	mmeInitDetach_c,
    	hssInitDetach_c,
    	ueInitDetach_c,

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

 enum PagingTrigger
 {
        none_c,
        hssInit_c,
        ddnInit_c,
        maxPagingTrigger_c
 };

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


}

#endif /* INCLUDE_MME_APP_UTILS_MMEPROCEDURETYPES_H_ */
