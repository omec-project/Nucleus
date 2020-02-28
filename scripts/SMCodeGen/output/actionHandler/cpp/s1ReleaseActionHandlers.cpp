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
* Action handler : send_rel_ab_req_to_sgw
***************************************/
ActStatus ActionHandlers::send_rel_ab_req_to_sgw(ControlBlock& cb)
{
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : process_rel_ab_resp_from_sgw
***************************************/
ActStatus ActionHandlers::process_rel_ab_resp_from_sgw(ControlBlock& cb)
{
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : send_s1_rel_cmd_to_ue
***************************************/
ActStatus ActionHandlers::send_s1_rel_cmd_to_ue(ControlBlock& cb)
{
    return ActStatus::PROCEED;
}

/***************************************
* Action handler : process_ue_ctxt_rel_comp
***************************************/
ActStatus ActionHandlers::process_ue_ctxt_rel_comp(ControlBlock& cb)
{
    return ActStatus::PROCEED;
}

