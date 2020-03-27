  
/*
 * Copyright 2019-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */
 
/**************************************
 * tauWfAuthResponse.cpp
 * This is an auto generated file.
 * Please do not edit this file.
 * All edits to be made through template source file
 * <TOP-DIR/scripts/SMCodeGen/templates/stateMachineTmpls/state.cpp.tt>
 **************************************/

#include "mmeSmDefs.h"
#include "actionTable.h"
#include "actionHandlers/actionHandlers.h"

#include "mmeStates/tauWfAuthResponse.h"	
#include "mmeStates/tauWfAuthRespValidate.h"

using namespace mme;
using namespace SM;

/******************************************************************************
* Constructor
******************************************************************************/
TauWfAuthResponse::TauWfAuthResponse():State(tau_wf_auth_response)
{
}

/******************************************************************************
* Destructor
******************************************************************************/
TauWfAuthResponse::~TauWfAuthResponse()
{
}

/******************************************************************************
* creates and returns static instance
******************************************************************************/
TauWfAuthResponse* TauWfAuthResponse::Instance()
{
        static TauWfAuthResponse state;
        return &state;
}

/******************************************************************************
* initializes eventToActionsMap
******************************************************************************/
void TauWfAuthResponse::initialize()
{
        {
                ActionTable actionTable;
                actionTable.addAction(&ActionHandlers::auth_response_validate);
                actionTable.setNextState(TauWfAuthRespValidate::Instance());
                eventToActionsMap.insert(pair<uint16_t, ActionTable>(AUTH_RESP_FROM_UE, actionTable));
        }
}
