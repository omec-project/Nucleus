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
* Action handler : default_attach_req_handler
***************************************/
ActStatus ActionHandlers::default_attach_req_handler(ControlBlock& cb)
{
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : default_detach_req_handler
***************************************/
ActStatus ActionHandlers::default_detach_req_handler(ControlBlock& cb)
{
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : default_s1_release_req_handler
***************************************/
ActStatus ActionHandlers::default_s1_release_req_handler(ControlBlock& cb)
{
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : default_ddn_handler
***************************************/
ActStatus ActionHandlers::default_ddn_handler(ControlBlock& cb)
{
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : default_service_req_handler
***************************************/
ActStatus ActionHandlers::default_service_req_handler(ControlBlock& cb)
{
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : default_cancel_loc_req_handler
***************************************/
ActStatus ActionHandlers::default_cancel_loc_req_handler(ControlBlock& cb)
{
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : default_tau_req_handler
***************************************/
ActStatus ActionHandlers::default_tau_req_handler(ControlBlock& cb)
{
    return ActStatus::PROCEED;
}

