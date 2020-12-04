
/*
 * Copyright 2020-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */
 
/**************************************
 * dedActWfBearerSetup.cpp
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

#include "mmeStates/dedActWfBearerSetup.h"

using namespace mme;
using namespace SM;

/******************************************************************************
* Constructor
******************************************************************************/
DedActWfBearerSetup::DedActWfBearerSetup():State(ded_act_wf_bearer_setup, defaultStateGuardTimerDuration_c)
{
        stateEntryAction = &MmeStatesUtils::on_state_entry;
        stateExitAction = &MmeStatesUtils::on_state_exit;
        eventValidator = &MmeStatesUtils::validate_event;
}

/******************************************************************************
* Destructor
******************************************************************************/
DedActWfBearerSetup::~DedActWfBearerSetup()
{
}

/******************************************************************************
* creates and returns static instance
******************************************************************************/
DedActWfBearerSetup* DedActWfBearerSetup::Instance()
{
        static DedActWfBearerSetup state;
        return &state;
}

/******************************************************************************
* initializes eventToActionsMap
******************************************************************************/
void DedActWfBearerSetup::initialize()
{
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::process_erab_setup_response);
                actionTable.addAction(&ActionHandlers::ded_act_complete);
                eventToActionsMap.insert(pair<uint16_t, ActionTable>(ERAB_SETUP_RESP_FROM_ENB, actionTable));
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::handle_state_guard_timeouts);
                eventToActionsMap.insert(pair<uint16_t, ActionTable>(STATE_GUARD_TIMEOUT, actionTable));
        }
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::abort_ded_activation);
                eventToActionsMap.insert(pair<uint16_t, ActionTable>(ABORT_EVENT, actionTable));
        }
}