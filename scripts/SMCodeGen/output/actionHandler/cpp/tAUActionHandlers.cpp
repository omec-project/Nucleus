/*
 * Copyright 2019-present Infosys Limited  
 *   
 * SPDX-License-Identifier: Apache-2.0    
 */
 
/******************************************************************************
 *
 * This file has both generated and manual code.
 * 
 * File template used for code generation:
 * <TOP-DIR/scripts/SMCodeGen/templates/stateMachineTmpls/actionHandlers.cpp.tt>
 *
 ******************************************************************************/
#include "actionHandlers/actionHandlers.h"
#include "controlBlock.h" 

using namespace mme;
using namespace SM;

/***************************************
* Action handler : send_tau_response_to_ue
***************************************/
ActStatus ActionHandlers::send_tau_response_to_ue(ControlBlock& cb)
{
    return ActStatus::PROCEED;
}

