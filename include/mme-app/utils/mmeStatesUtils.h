/*
 * Copyright 2020-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef INCLUDE_MME_APP_UTILS_MMESTATESUTILS_H_
#define INCLUDE_MME_APP_UTILS_MMESTATESUTILS_H_

#include <stdint.h>

#include "smTypes.h"

namespace mme
{
    class MmeStatesUtils
    {

        /***************************************
        * Constructor
        ****************************************/
        MmeStatesUtils()
        {
        }

        /***************************************
        * Destructor
        ****************************************/
        ~MmeStatesUtils()
        {
        }

        public:
        /**********************************************
        * MmeStatesUtils : on_state_entry
        ***********************************************/
        static SM::ActStatus on_state_entry(SM::ControlBlock& cb);

        /**********************************************
     	* MmeStatesUtils : on_state_exit
        ***********************************************/
        static SM::ActStatus on_state_exit(SM::ControlBlock& cb);

        /************************************************
        * MmeStatesUtils : validate_event
        ************************************************/
        static SM::EventStatus validate_event(SM::ControlBlock& cb,
                SM::TempDataBlock* tempDataBlock,
                SM::Event& evt);

        /**********************************************
        * MmeStatesUtils : handle_sm_exception
        ***********************************************/
        static void handle_sm_exception(SM::ControlBlock& cb, uint8_t err_code);
    };
};
#endif /* INCLUDE_MME_APP_UTILS_MMESTATESUTILS_H_ */
