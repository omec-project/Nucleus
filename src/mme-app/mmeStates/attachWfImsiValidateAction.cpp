
/*
 * Copyright 2020-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */
 
/**************************************
 * attachWfImsiValidateAction.cpp
 * This is an auto generated file.
 * Please do not edit this file.
 * All edits to be made through template source file
 * <TOP-DIR/scripts/SMCodeGen/templates/stateMachineTmpls/state.cpp.tt>
 **************************************/


#include "actionTable.h"
#include "actionHandlers/actionHandlers.h"
#include "mmeSmDefs.h"
#include "utils/mmeStatesUtils.h"
#include "utils/mmeTimerTypes.h"

#include "mmeStates/attachWfImsiValidateAction.h"	
#include "mmeStates/attachWfAia.h"	
#include "mmeStates/attachWfIdentityResponse.h"

using namespace mme;
using namespace SM;

/******************************************************************************
* Constructor
******************************************************************************/
AttachWfImsiValidateAction::AttachWfImsiValidateAction():State(attach_wf_imsi_validate_action, defaultStateGuardTimerDuration_c)
{
        stateEntryAction = &MmeStatesUtils::on_state_entry;
        stateExitAction = &MmeStatesUtils::on_state_exit;
        eventValidator = &MmeStatesUtils::validate_event;
}

/******************************************************************************
* Destructor
******************************************************************************/
AttachWfImsiValidateAction::~AttachWfImsiValidateAction()
{
}

/******************************************************************************
* creates and returns static instance
******************************************************************************/
AttachWfImsiValidateAction* AttachWfImsiValidateAction::Instance()
{
        static AttachWfImsiValidateAction state;
        return &state;
}

/******************************************************************************
* initializes eventToActionsMap
******************************************************************************/
void AttachWfImsiValidateAction::initialize()
{
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::send_air_to_hss);
                actionTable.setNextState(AttachWfAia::Instance());
                eventToActionsMap.insert(pair<uint16_t, ActionTable>(IMSI_VALIDATION_SUCCESS, actionTable));
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::send_identity_request_to_ue);
                actionTable.setNextState(AttachWfIdentityResponse::Instance());
                eventToActionsMap.insert(pair<uint16_t, ActionTable>(IMSI_VALIDATION_FAILURE, actionTable));
        }
}
