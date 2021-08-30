

/*
 * Copyright 2021-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */
 
/**************************************
 * defaultMmeProcedureStates.cpp
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

#include "mmeStates/defaultMmeProcedureStates.h"

using namespace mme;
using namespace SM;


/******************************************************************************
* Constructor
******************************************************************************/
DefaultMmeState::DefaultMmeState():State()
{
        stateEntryAction = &MmeStatesUtils::on_state_entry;
        stateExitAction = &MmeStatesUtils::on_state_exit;
        eventValidator = &MmeStatesUtils::validate_event;
		
}

/******************************************************************************
* Destructor
******************************************************************************/
DefaultMmeState::~DefaultMmeState()
{
}

/******************************************************************************
* creates and returns static instance
******************************************************************************/
DefaultMmeState* DefaultMmeState::Instance()
{
        static DefaultMmeState state;
        return &state;
}

/******************************************************************************
* initializes eventToActionsMap
******************************************************************************/
void DefaultMmeState::initialize()
{
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::default_attach_req_handler);
                eventToActionsMap[ATTACH_REQ_FROM_UE] = actionTable;
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::default_detach_req_handler);
                eventToActionsMap[DETACH_REQ_FROM_UE] = actionTable;
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::default_s1_release_req_handler);
                eventToActionsMap[S1_REL_REQ_FROM_UE] = actionTable;
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::default_ddn_handler);
                eventToActionsMap[DDN_FROM_SGW] = actionTable;
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::default_service_req_handler);
                eventToActionsMap[SERVICE_REQUEST_FROM_UE] = actionTable;
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::default_cancel_loc_req_handler);
                eventToActionsMap[CLR_FROM_HSS] = actionTable;
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::default_tau_req_handler);
                eventToActionsMap[TAU_REQUEST_FROM_UE] = actionTable;
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::default_s1_ho_handler);
                eventToActionsMap[HO_REQUIRED_FROM_ENB] = actionTable;
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::default_erab_mod_indication_handler);
                eventToActionsMap[ERAB_MOD_INDICATION_FROM_ENB] = actionTable;
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::default_create_bearer_req_handler);
                eventToActionsMap[CREATE_BEARER_REQ_FROM_GW] = actionTable;
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::default_delete_bearer_req_handler);
                eventToActionsMap[DELETE_BEARER_REQ_FROM_GW] = actionTable;
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::handle_detach_failure);
                eventToActionsMap[DETACH_FAILURE] = actionTable;
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::handle_nas_pdu_parse_failure);
                eventToActionsMap[NAS_PDU_PARSE_FAILURE] = actionTable;
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::default_path_switch_req_handler);
                eventToActionsMap[PATH_SWITCH_REQ_FROM_ENB] = actionTable;
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::default_delete_subs_req_handler);
                eventToActionsMap[DSR_FROM_HSS] = actionTable;
        }
}

/******************************************************************************
* returns stateId
******************************************************************************/
uint16_t DefaultMmeState::getStateId()const
{
	return default_mme_state;
}

/******************************************************************************
* returns stateName
******************************************************************************/
const char* DefaultMmeState::getStateName()const
{
	return "default_mme_state";
}
