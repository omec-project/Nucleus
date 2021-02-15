

/*
 * Copyright 2021-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */
 
/**************************************
 * serviceRequestStates.cpp
 * This is an auto generated file.
 * Please do not edit this file.
 * All edits to be made through template source file
 * <TOP-DIR/scripts/SMCodeGen/templates/stateMachineTmpls/state.cpp.tt>
 **************************************/


#include "actionTable.h"
#include "actionHandlers/actionHandlers.h"
#include <mmeSmDefs.h>
#include <utils/mmeStatesUtils.h>
#include <utils/mmeTimerTypes.h>

#include "mmeStates/serviceRequestStates.h"

using namespace mme;
using namespace SM;


/******************************************************************************
* Constructor
******************************************************************************/
PagingStart::PagingStart():State()
{
        stateEntryAction = &MmeStatesUtils::on_state_entry;
        stateExitAction = &MmeStatesUtils::on_state_exit;
        eventValidator = &MmeStatesUtils::validate_event;
		
}

/******************************************************************************
* Destructor
******************************************************************************/
PagingStart::~PagingStart()
{
}

/******************************************************************************
* creates and returns static instance
******************************************************************************/
PagingStart* PagingStart::Instance()
{
        static PagingStart state;
        return &state;
}

/******************************************************************************
* initializes eventToActionsMap
******************************************************************************/
void PagingStart::initialize()
{
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::send_paging_req_to_ue);
                actionTable.setNextState(PagingWfServiceReq::Instance());
                eventToActionsMap[GW_CP_REQ_INIT_PAGING] = actionTable;
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::send_paging_req_to_ue);
                actionTable.addAction(&ActionHandlers::send_ddn_ack_to_sgw);
                actionTable.setNextState(PagingWfServiceReq::Instance());
                eventToActionsMap[DDN_FROM_SGW] = actionTable;
        }
}

/******************************************************************************
* returns stateId
******************************************************************************/
uint16_t PagingStart::getStateId()const
{
	return paging_start;
}

/******************************************************************************
* returns stateName
******************************************************************************/
const char* PagingStart::getStateName()const
{
	return "paging_start";
}

/******************************************************************************
* Constructor
******************************************************************************/
PagingWfServiceReq::PagingWfServiceReq():State()
{
        stateGuardTimeoutDuration_m = defaultStateGuardTimerDuration_c;
        stateEntryAction = &MmeStatesUtils::on_state_entry;
        stateExitAction = &MmeStatesUtils::on_state_exit;
        eventValidator = &MmeStatesUtils::validate_event;
		
}

/******************************************************************************
* Destructor
******************************************************************************/
PagingWfServiceReq::~PagingWfServiceReq()
{
}

/******************************************************************************
* creates and returns static instance
******************************************************************************/
PagingWfServiceReq* PagingWfServiceReq::Instance()
{
        static PagingWfServiceReq state;
        return &state;
}

/******************************************************************************
* initializes eventToActionsMap
******************************************************************************/
void PagingWfServiceReq::initialize()
{
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::process_service_request);
                actionTable.addAction(&ActionHandlers::auth_req_to_ue);
                actionTable.setNextState(ServiceRequestWfAuthResponse::Instance());
                eventToActionsMap[SERVICE_REQUEST_FROM_UE] = actionTable;
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::abort_service_req_procedure);
                eventToActionsMap[STATE_GUARD_TIMEOUT] = actionTable;
        }
}

/******************************************************************************
* returns stateId
******************************************************************************/
uint16_t PagingWfServiceReq::getStateId()const
{
	return paging_wf_service_req;
}

/******************************************************************************
* returns stateName
******************************************************************************/
const char* PagingWfServiceReq::getStateName()const
{
	return "paging_wf_service_req";
}

/******************************************************************************
* Constructor
******************************************************************************/
ServiceRequestStart::ServiceRequestStart():State()
{
        stateEntryAction = &MmeStatesUtils::on_state_entry;
        stateExitAction = &MmeStatesUtils::on_state_exit;
        eventValidator = &MmeStatesUtils::validate_event;
		
}

/******************************************************************************
* Destructor
******************************************************************************/
ServiceRequestStart::~ServiceRequestStart()
{
}

/******************************************************************************
* creates and returns static instance
******************************************************************************/
ServiceRequestStart* ServiceRequestStart::Instance()
{
        static ServiceRequestStart state;
        return &state;
}

/******************************************************************************
* initializes eventToActionsMap
******************************************************************************/
void ServiceRequestStart::initialize()
{
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::auth_req_to_ue);
                actionTable.setNextState(ServiceRequestWfAuthResponse::Instance());
                eventToActionsMap[SERVICE_REQUEST_FROM_UE] = actionTable;
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::send_service_reject);
                actionTable.addAction(&ActionHandlers::send_s1_rel_cmd_to_ue);
                actionTable.addAction(&ActionHandlers::abort_service_req_procedure);
                eventToActionsMap[STATE_GUARD_TIMEOUT] = actionTable;
        }
}

/******************************************************************************
* returns stateId
******************************************************************************/
uint16_t ServiceRequestStart::getStateId()const
{
	return service_request_start;
}

/******************************************************************************
* returns stateName
******************************************************************************/
const char* ServiceRequestStart::getStateName()const
{
	return "service_request_start";
}

/******************************************************************************
* Constructor
******************************************************************************/
ServiceRequestWfAuthResponse::ServiceRequestWfAuthResponse():State()
{
        stateGuardTimeoutDuration_m = defaultStateGuardTimerDuration_c;
        stateEntryAction = &MmeStatesUtils::on_state_entry;
        stateExitAction = &MmeStatesUtils::on_state_exit;
        eventValidator = &MmeStatesUtils::validate_event;
		
}

/******************************************************************************
* Destructor
******************************************************************************/
ServiceRequestWfAuthResponse::~ServiceRequestWfAuthResponse()
{
}

/******************************************************************************
* creates and returns static instance
******************************************************************************/
ServiceRequestWfAuthResponse* ServiceRequestWfAuthResponse::Instance()
{
        static ServiceRequestWfAuthResponse state;
        return &state;
}

/******************************************************************************
* initializes eventToActionsMap
******************************************************************************/
void ServiceRequestWfAuthResponse::initialize()
{
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::auth_response_validate);
                actionTable.setNextState(ServiceRequestWfAuthRespValidate::Instance());
                eventToActionsMap[AUTH_RESP_FROM_UE] = actionTable;
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::send_service_reject);
                actionTable.addAction(&ActionHandlers::send_s1_rel_cmd_to_ue);
                actionTable.addAction(&ActionHandlers::abort_service_req_procedure);
                eventToActionsMap[STATE_GUARD_TIMEOUT] = actionTable;
        }
}

/******************************************************************************
* returns stateId
******************************************************************************/
uint16_t ServiceRequestWfAuthResponse::getStateId()const
{
	return service_request_wf_auth_response;
}

/******************************************************************************
* returns stateName
******************************************************************************/
const char* ServiceRequestWfAuthResponse::getStateName()const
{
	return "service_request_wf_auth_response";
}

/******************************************************************************
* Constructor
******************************************************************************/
ServiceRequestWfAuthRespValidate::ServiceRequestWfAuthRespValidate():State()
{
        stateGuardTimeoutDuration_m = defaultStateGuardTimerDuration_c;
        stateEntryAction = &MmeStatesUtils::on_state_entry;
        stateExitAction = &MmeStatesUtils::on_state_exit;
        eventValidator = &MmeStatesUtils::validate_event;
		
}

/******************************************************************************
* Destructor
******************************************************************************/
ServiceRequestWfAuthRespValidate::~ServiceRequestWfAuthRespValidate()
{
}

/******************************************************************************
* creates and returns static instance
******************************************************************************/
ServiceRequestWfAuthRespValidate* ServiceRequestWfAuthRespValidate::Instance()
{
        static ServiceRequestWfAuthRespValidate state;
        return &state;
}

/******************************************************************************
* initializes eventToActionsMap
******************************************************************************/
void ServiceRequestWfAuthRespValidate::initialize()
{
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::sec_mode_cmd_to_ue);
                actionTable.setNextState(ServiceRequestWfSecCmp::Instance());
                eventToActionsMap[AUTH_RESP_SUCCESS] = actionTable;
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::send_air_to_hss);
                actionTable.setNextState(ServiceRequestWfAia::Instance());
                eventToActionsMap[AUTH_RESP_SYNC_FAILURE] = actionTable;
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::send_s1_rel_cmd_to_ue);
                actionTable.addAction(&ActionHandlers::abort_service_req_procedure);
                eventToActionsMap[AUTH_RESP_FAILURE] = actionTable;
        }
}

/******************************************************************************
* returns stateId
******************************************************************************/
uint16_t ServiceRequestWfAuthRespValidate::getStateId()const
{
	return service_request_wf_auth_resp_validate;
}

/******************************************************************************
* returns stateName
******************************************************************************/
const char* ServiceRequestWfAuthRespValidate::getStateName()const
{
	return "service_request_wf_auth_resp_validate";
}

/******************************************************************************
* Constructor
******************************************************************************/
ServiceRequestWfSecCmp::ServiceRequestWfSecCmp():State()
{
        stateGuardTimeoutDuration_m = defaultStateGuardTimerDuration_c;
        stateEntryAction = &MmeStatesUtils::on_state_entry;
        stateExitAction = &MmeStatesUtils::on_state_exit;
        eventValidator = &MmeStatesUtils::validate_event;
		
}

/******************************************************************************
* Destructor
******************************************************************************/
ServiceRequestWfSecCmp::~ServiceRequestWfSecCmp()
{
}

/******************************************************************************
* creates and returns static instance
******************************************************************************/
ServiceRequestWfSecCmp* ServiceRequestWfSecCmp::Instance()
{
        static ServiceRequestWfSecCmp state;
        return &state;
}

/******************************************************************************
* initializes eventToActionsMap
******************************************************************************/
void ServiceRequestWfSecCmp::initialize()
{
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::process_sec_mode_resp);
                actionTable.addAction(&ActionHandlers::send_init_ctxt_req_to_ue_svc_req);
                actionTable.setNextState(ServiceRequestWfInitCtxtResp::Instance());
                eventToActionsMap[SEC_MODE_RESP_FROM_UE] = actionTable;
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::send_service_reject);
                actionTable.addAction(&ActionHandlers::send_s1_rel_cmd_to_ue);
                actionTable.addAction(&ActionHandlers::abort_service_req_procedure);
                eventToActionsMap[STATE_GUARD_TIMEOUT] = actionTable;
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::send_service_reject);
                actionTable.addAction(&ActionHandlers::send_s1_rel_cmd_to_ue);
                actionTable.addAction(&ActionHandlers::abort_service_req_procedure);
                eventToActionsMap[ABORT_EVENT] = actionTable;
        }
}

/******************************************************************************
* returns stateId
******************************************************************************/
uint16_t ServiceRequestWfSecCmp::getStateId()const
{
	return service_request_wf_sec_cmp;
}

/******************************************************************************
* returns stateName
******************************************************************************/
const char* ServiceRequestWfSecCmp::getStateName()const
{
	return "service_request_wf_sec_cmp";
}

/******************************************************************************
* Constructor
******************************************************************************/
ServiceRequestWfAia::ServiceRequestWfAia():State()
{
        stateGuardTimeoutDuration_m = defaultStateGuardTimerDuration_c;
        stateEntryAction = &MmeStatesUtils::on_state_entry;
        stateExitAction = &MmeStatesUtils::on_state_exit;
        eventValidator = &MmeStatesUtils::validate_event;
		
}

/******************************************************************************
* Destructor
******************************************************************************/
ServiceRequestWfAia::~ServiceRequestWfAia()
{
}

/******************************************************************************
* creates and returns static instance
******************************************************************************/
ServiceRequestWfAia* ServiceRequestWfAia::Instance()
{
        static ServiceRequestWfAia state;
        return &state;
}

/******************************************************************************
* initializes eventToActionsMap
******************************************************************************/
void ServiceRequestWfAia::initialize()
{
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::process_aia);
                actionTable.addAction(&ActionHandlers::auth_req_to_ue);
                actionTable.setNextState(ServiceRequestWfAuthResponse::Instance());
                eventToActionsMap[AIA_FROM_HSS] = actionTable;
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::send_service_reject);
                actionTable.addAction(&ActionHandlers::send_s1_rel_cmd_to_ue);
                actionTable.addAction(&ActionHandlers::abort_service_req_procedure);
                eventToActionsMap[STATE_GUARD_TIMEOUT] = actionTable;
        }
}

/******************************************************************************
* returns stateId
******************************************************************************/
uint16_t ServiceRequestWfAia::getStateId()const
{
	return service_request_wf_aia;
}

/******************************************************************************
* returns stateName
******************************************************************************/
const char* ServiceRequestWfAia::getStateName()const
{
	return "service_request_wf_aia";
}

/******************************************************************************
* Constructor
******************************************************************************/
ServiceRequestWfInitCtxtResp::ServiceRequestWfInitCtxtResp():State()
{
        stateGuardTimeoutDuration_m = defaultStateGuardTimerDuration_c;
        stateEntryAction = &MmeStatesUtils::on_state_entry;
        stateExitAction = &MmeStatesUtils::on_state_exit;
        eventValidator = &MmeStatesUtils::validate_event;
		
}

/******************************************************************************
* Destructor
******************************************************************************/
ServiceRequestWfInitCtxtResp::~ServiceRequestWfInitCtxtResp()
{
}

/******************************************************************************
* creates and returns static instance
******************************************************************************/
ServiceRequestWfInitCtxtResp* ServiceRequestWfInitCtxtResp::Instance()
{
        static ServiceRequestWfInitCtxtResp state;
        return &state;
}

/******************************************************************************
* initializes eventToActionsMap
******************************************************************************/
void ServiceRequestWfInitCtxtResp::initialize()
{
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::process_init_ctxt_resp_svc_req);
                actionTable.addAction(&ActionHandlers::send_mb_req_to_sgw_svc_req);
                actionTable.setNextState(ServiceRequestWfMbResp::Instance());
                eventToActionsMap[INIT_CTXT_RESP_FROM_UE] = actionTable;
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::send_service_reject);
                actionTable.addAction(&ActionHandlers::send_s1_rel_cmd_to_ue);
                actionTable.addAction(&ActionHandlers::abort_service_req_procedure);
                eventToActionsMap[STATE_GUARD_TIMEOUT] = actionTable;
        }
}

/******************************************************************************
* returns stateId
******************************************************************************/
uint16_t ServiceRequestWfInitCtxtResp::getStateId()const
{
	return service_request_wf_init_ctxt_resp;
}

/******************************************************************************
* returns stateName
******************************************************************************/
const char* ServiceRequestWfInitCtxtResp::getStateName()const
{
	return "service_request_wf_init_ctxt_resp";
}

/******************************************************************************
* Constructor
******************************************************************************/
ServiceRequestWfMbResp::ServiceRequestWfMbResp():State()
{
        stateGuardTimeoutDuration_m = defaultStateGuardTimerDuration_c;
        stateEntryAction = &MmeStatesUtils::on_state_entry;
        stateExitAction = &MmeStatesUtils::on_state_exit;
        eventValidator = &MmeStatesUtils::validate_event;
		
}

/******************************************************************************
* Destructor
******************************************************************************/
ServiceRequestWfMbResp::~ServiceRequestWfMbResp()
{
}

/******************************************************************************
* creates and returns static instance
******************************************************************************/
ServiceRequestWfMbResp* ServiceRequestWfMbResp::Instance()
{
        static ServiceRequestWfMbResp state;
        return &state;
}

/******************************************************************************
* initializes eventToActionsMap
******************************************************************************/
void ServiceRequestWfMbResp::initialize()
{
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::process_mb_resp_svc_req);
                actionTable.addAction(&ActionHandlers::service_request_complete);
                eventToActionsMap[MB_RESP_FROM_SGW] = actionTable;
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::send_service_reject);
                actionTable.addAction(&ActionHandlers::send_s1_rel_cmd_to_ue);
                actionTable.addAction(&ActionHandlers::abort_service_req_procedure);
                eventToActionsMap[STATE_GUARD_TIMEOUT] = actionTable;
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::send_service_reject);
                actionTable.addAction(&ActionHandlers::send_s1_rel_cmd_to_ue);
                actionTable.addAction(&ActionHandlers::abort_service_req_procedure);
                eventToActionsMap[ABORT_EVENT] = actionTable;
        }
}

/******************************************************************************
* returns stateId
******************************************************************************/
uint16_t ServiceRequestWfMbResp::getStateId()const
{
	return service_request_wf_mb_resp;
}

/******************************************************************************
* returns stateName
******************************************************************************/
const char* ServiceRequestWfMbResp::getStateName()const
{
	return "service_request_wf_mb_resp";
}
