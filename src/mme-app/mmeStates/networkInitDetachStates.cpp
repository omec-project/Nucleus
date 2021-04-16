

/*
 * Copyright 2021-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */
 
/**************************************
 * networkInitDetachStates.cpp
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

#include "mmeStates/networkInitDetachStates.h"

using namespace mme;
using namespace SM;


/******************************************************************************
* Constructor
******************************************************************************/
NetworkInitDetachState::NetworkInitDetachState():State()
{
        stateEntryAction = &MmeStatesUtils::on_state_entry;
        stateExitAction = &MmeStatesUtils::on_state_exit;
        eventValidator = &MmeStatesUtils::validate_event;
		
}

/******************************************************************************
* Destructor
******************************************************************************/
NetworkInitDetachState::~NetworkInitDetachState()
{
}

/******************************************************************************
* creates and returns static instance
******************************************************************************/
NetworkInitDetachState* NetworkInitDetachState::Instance()
{
        static NetworkInitDetachState state;
        return &state;
}

/******************************************************************************
* initializes eventToActionsMap
******************************************************************************/
void NetworkInitDetachState::initialize()
{
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::handle_dsr_during_detach);
                eventToActionsMap[DSR_FROM_HSS] = actionTable;
        }
}

/******************************************************************************
* returns stateId
******************************************************************************/
uint16_t NetworkInitDetachState::getStateId()const
{
	return network_init_detach_state;
}

/******************************************************************************
* returns stateName
******************************************************************************/
const char* NetworkInitDetachState::getStateName()const
{
	return "network_init_detach_state";
}

/******************************************************************************
* Constructor
******************************************************************************/
NiDetachStart::NiDetachStart(): NetworkInitDetachState()
{
        stateEntryAction = &MmeStatesUtils::on_state_entry;
        stateExitAction = &MmeStatesUtils::on_state_exit;
        eventValidator = &MmeStatesUtils::validate_event;
		
}

/******************************************************************************
* Destructor
******************************************************************************/
NiDetachStart::~NiDetachStart()
{
}

/******************************************************************************
* creates and returns static instance
******************************************************************************/
NiDetachStart* NiDetachStart::Instance()
{
        static NiDetachStart state;
        return &state;
}

/******************************************************************************
* initializes eventToActionsMap
******************************************************************************/
void NiDetachStart::initialize()
{
        NetworkInitDetachState::initialize();
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::ni_detach_req_to_ue);
                actionTable.addAction(&ActionHandlers::del_session_req);
                actionTable.setNextState(NiDetachWfDetAccptDelSessResp::Instance());
                eventToActionsMap[HSS_INIT_DETACH] = actionTable;
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::ni_detach_req_to_ue);
                actionTable.addAction(&ActionHandlers::del_session_req);
                actionTable.setNextState(NiDetachWfDetAccptDelSessResp::Instance());
                eventToActionsMap[MME_INIT_DETACH] = actionTable;
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::ni_detach_req_to_ue);
                actionTable.setNextState(NiDetachWfDetachAccept::Instance());
                eventToActionsMap[PGW_INIT_DETACH] = actionTable;
        }
        {
                ActionTable actionTable;
                actionTable.setNextState(NiDetachWfPagingComplete::Instance());
                eventToActionsMap[HSS_INIT_PAGING] = actionTable;
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::send_s1_rel_cmd_to_ue);
                actionTable.addAction(&ActionHandlers::abort_detach);
                eventToActionsMap[ABORT_EVENT] = actionTable;
        }
}

/******************************************************************************
* returns stateId
******************************************************************************/
uint16_t NiDetachStart::getStateId()const
{
	return ni_detach_start;
}

/******************************************************************************
* returns stateName
******************************************************************************/
const char* NiDetachStart::getStateName()const
{
	return "ni_detach_start";
}

/******************************************************************************
* Constructor
******************************************************************************/
NiDetachWfPagingComplete::NiDetachWfPagingComplete(): NetworkInitDetachState()
{
        stateGuardTimeoutDuration_m = 12000;
        stateEntryAction = &MmeStatesUtils::on_state_entry;
        stateExitAction = &MmeStatesUtils::on_state_exit;
        eventValidator = &MmeStatesUtils::validate_event;
		
}

/******************************************************************************
* Destructor
******************************************************************************/
NiDetachWfPagingComplete::~NiDetachWfPagingComplete()
{
}

/******************************************************************************
* creates and returns static instance
******************************************************************************/
NiDetachWfPagingComplete* NiDetachWfPagingComplete::Instance()
{
        static NiDetachWfPagingComplete state;
        return &state;
}

/******************************************************************************
* initializes eventToActionsMap
******************************************************************************/
void NiDetachWfPagingComplete::initialize()
{
        NetworkInitDetachState::initialize();
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::trigger_nwk_init_detach);
                actionTable.setNextState(NiDetachStart::Instance());
                eventToActionsMap[PAGING_COMPLETE] = actionTable;
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::handle_paging_failure);
                eventToActionsMap[PAGING_FAILURE] = actionTable;
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::handle_state_guard_timeouts);
                eventToActionsMap[STATE_GUARD_TIMEOUT] = actionTable;
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::abort_detach);
                eventToActionsMap[ABORT_EVENT] = actionTable;
        }
}

/******************************************************************************
* returns stateId
******************************************************************************/
uint16_t NiDetachWfPagingComplete::getStateId()const
{
	return ni_detach_wf_paging_complete;
}

/******************************************************************************
* returns stateName
******************************************************************************/
const char* NiDetachWfPagingComplete::getStateName()const
{
	return "ni_detach_wf_paging_complete";
}

/******************************************************************************
* Constructor
******************************************************************************/
NiDetachWfDetAccptDelSessResp::NiDetachWfDetAccptDelSessResp(): NetworkInitDetachState()
{
        stateGuardTimeoutDuration_m = defaultStateGuardTimerDuration_c;
        stateEntryAction = &MmeStatesUtils::on_state_entry;
        stateExitAction = &MmeStatesUtils::on_state_exit;
        eventValidator = &MmeStatesUtils::validate_event;
		
}

/******************************************************************************
* Destructor
******************************************************************************/
NiDetachWfDetAccptDelSessResp::~NiDetachWfDetAccptDelSessResp()
{
}

/******************************************************************************
* creates and returns static instance
******************************************************************************/
NiDetachWfDetAccptDelSessResp* NiDetachWfDetAccptDelSessResp::Instance()
{
        static NiDetachWfDetAccptDelSessResp state;
        return &state;
}

/******************************************************************************
* initializes eventToActionsMap
******************************************************************************/
void NiDetachWfDetAccptDelSessResp::initialize()
{
        NetworkInitDetachState::initialize();
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::process_detach_accept_from_ue);
                actionTable.setNextState(NiDetachWfDelSessResp::Instance());
                eventToActionsMap[DETACH_ACCEPT_FROM_UE] = actionTable;
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::process_del_session_resp);
                actionTable.setNextState(NiDetachWfDetachAccept::Instance());
                eventToActionsMap[DEL_SESSION_RESP_FROM_SGW] = actionTable;
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::handle_state_guard_timeouts);
                eventToActionsMap[STATE_GUARD_TIMEOUT] = actionTable;
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::send_s1_rel_cmd_to_ue);
                actionTable.addAction(&ActionHandlers::abort_detach);
                eventToActionsMap[ABORT_EVENT] = actionTable;
        }
}

/******************************************************************************
* returns stateId
******************************************************************************/
uint16_t NiDetachWfDetAccptDelSessResp::getStateId()const
{
	return ni_detach_wf_det_accpt_del_sess_resp;
}

/******************************************************************************
* returns stateName
******************************************************************************/
const char* NiDetachWfDetAccptDelSessResp::getStateName()const
{
	return "ni_detach_wf_det_accpt_del_sess_resp";
}

/******************************************************************************
* Constructor
******************************************************************************/
NiDetachWfDelSessResp::NiDetachWfDelSessResp(): NetworkInitDetachState()
{
        stateGuardTimeoutDuration_m = defaultStateGuardTimerDuration_c;
        stateEntryAction = &MmeStatesUtils::on_state_entry;
        stateExitAction = &MmeStatesUtils::on_state_exit;
        eventValidator = &MmeStatesUtils::validate_event;
		
}

/******************************************************************************
* Destructor
******************************************************************************/
NiDetachWfDelSessResp::~NiDetachWfDelSessResp()
{
}

/******************************************************************************
* creates and returns static instance
******************************************************************************/
NiDetachWfDelSessResp* NiDetachWfDelSessResp::Instance()
{
        static NiDetachWfDelSessResp state;
        return &state;
}

/******************************************************************************
* initializes eventToActionsMap
******************************************************************************/
void NiDetachWfDelSessResp::initialize()
{
        NetworkInitDetachState::initialize();
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::process_del_session_resp);
                actionTable.addAction(&ActionHandlers::send_s1_rel_cmd_to_ue);
                actionTable.setNextState(NiDetachWfS1RelComp::Instance());
                eventToActionsMap[DEL_SESSION_RESP_FROM_SGW] = actionTable;
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::handle_state_guard_timeouts);
                eventToActionsMap[STATE_GUARD_TIMEOUT] = actionTable;
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::send_s1_rel_cmd_to_ue);
                actionTable.addAction(&ActionHandlers::abort_detach);
                eventToActionsMap[ABORT_EVENT] = actionTable;
        }
}

/******************************************************************************
* returns stateId
******************************************************************************/
uint16_t NiDetachWfDelSessResp::getStateId()const
{
	return ni_detach_wf_del_sess_resp;
}

/******************************************************************************
* returns stateName
******************************************************************************/
const char* NiDetachWfDelSessResp::getStateName()const
{
	return "ni_detach_wf_del_sess_resp";
}

/******************************************************************************
* Constructor
******************************************************************************/
NiDetachWfDetachAccept::NiDetachWfDetachAccept(): NetworkInitDetachState()
{
        stateGuardTimeoutDuration_m = defaultStateGuardTimerDuration_c;
        stateEntryAction = &MmeStatesUtils::on_state_entry;
        stateExitAction = &MmeStatesUtils::on_state_exit;
        eventValidator = &MmeStatesUtils::validate_event;
		
}

/******************************************************************************
* Destructor
******************************************************************************/
NiDetachWfDetachAccept::~NiDetachWfDetachAccept()
{
}

/******************************************************************************
* creates and returns static instance
******************************************************************************/
NiDetachWfDetachAccept* NiDetachWfDetachAccept::Instance()
{
        static NiDetachWfDetachAccept state;
        return &state;
}

/******************************************************************************
* initializes eventToActionsMap
******************************************************************************/
void NiDetachWfDetachAccept::initialize()
{
        NetworkInitDetachState::initialize();
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::process_detach_accept_from_ue);
                actionTable.addAction(&ActionHandlers::send_s1_rel_cmd_to_ue_for_detach);
                actionTable.setNextState(NiDetachWfS1RelComp::Instance());
                eventToActionsMap[DETACH_ACCEPT_FROM_UE] = actionTable;
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::handle_state_guard_timeouts);
                eventToActionsMap[STATE_GUARD_TIMEOUT] = actionTable;
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::send_s1_rel_cmd_to_ue);
                actionTable.addAction(&ActionHandlers::abort_detach);
                eventToActionsMap[ABORT_EVENT] = actionTable;
        }
}

/******************************************************************************
* returns stateId
******************************************************************************/
uint16_t NiDetachWfDetachAccept::getStateId()const
{
	return ni_detach_wf_detach_accept;
}

/******************************************************************************
* returns stateName
******************************************************************************/
const char* NiDetachWfDetachAccept::getStateName()const
{
	return "ni_detach_wf_detach_accept";
}

/******************************************************************************
* Constructor
******************************************************************************/
NiDetachWfS1RelComp::NiDetachWfS1RelComp(): NetworkInitDetachState()
{
        stateGuardTimeoutDuration_m = defaultStateGuardTimerDuration_c;
        stateEntryAction = &MmeStatesUtils::on_state_entry;
        stateExitAction = &MmeStatesUtils::on_state_exit;
        eventValidator = &MmeStatesUtils::validate_event;
		
}

/******************************************************************************
* Destructor
******************************************************************************/
NiDetachWfS1RelComp::~NiDetachWfS1RelComp()
{
}

/******************************************************************************
* creates and returns static instance
******************************************************************************/
NiDetachWfS1RelComp* NiDetachWfS1RelComp::Instance()
{
        static NiDetachWfS1RelComp state;
        return &state;
}

/******************************************************************************
* initializes eventToActionsMap
******************************************************************************/
void NiDetachWfS1RelComp::initialize()
{
        NetworkInitDetachState::initialize();
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::process_ue_ctxt_rel_comp_for_detach);
                eventToActionsMap[UE_CTXT_REL_COMP_FROM_ENB] = actionTable;
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::handle_state_guard_timeouts);
                eventToActionsMap[STATE_GUARD_TIMEOUT] = actionTable;
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::abort_detach);
                eventToActionsMap[ABORT_EVENT] = actionTable;
        }
}

/******************************************************************************
* returns stateId
******************************************************************************/
uint16_t NiDetachWfS1RelComp::getStateId()const
{
	return ni_detach_wf_s1_rel_comp;
}

/******************************************************************************
* returns stateName
******************************************************************************/
const char* NiDetachWfS1RelComp::getStateName()const
{
	return "ni_detach_wf_s1_rel_comp";
}
