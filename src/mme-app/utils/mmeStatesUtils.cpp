/*
 * Copyright 2020-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <utils/mmeStatesUtils.h>

using namespace mme;
using namespace SM;

/***************************************
* MmeStatesUtils : on_state_entry
***************************************/
ActStatus MmeStatesUtils::on_state_entry(ControlBlock& cb)
{
    // TODO: Start state guard timer if configured
    return ActStatus::PROCEED;
}

/***************************************
* MmeStatesUtils : on_state_exit
***************************************/
ActStatus MmeStatesUtils::on_state_exit(ControlBlock& cb)
{
    // TODO: Stop state guard timer
    return ActStatus::PROCEED;
}

/***************************************
* MmeStatesUtils : validate_event
***************************************/
EventStatus MmeStatesUtils::validate_event(ControlBlock& cb,
        TempDataBlock* tempDataBlock,
        SM::Event& event)
{
    // TODO: Validate the event
    return EventStatus::CONSUME;
}
