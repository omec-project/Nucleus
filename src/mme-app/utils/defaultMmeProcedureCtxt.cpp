/*
 * Copyright (c) 2019, Infosys Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <utils/defaultMmeProcedureCtxt.h>
#include <mmeStates/defaultMmeState.h>

using namespace mme;

DefaultMmeProcedureCtxt::DefaultMmeProcedureCtxt()
{
    setNextState(DefaultMmeState::Instance());
    setCtxtType(defaultMmeProcedure_c);
}

DefaultMmeProcedureCtxt::~DefaultMmeProcedureCtxt()
{

}

DefaultMmeProcedureCtxt* DefaultMmeProcedureCtxt::Instance()
{
    static DefaultMmeProcedureCtxt defaultMmeProcedureCtxt;
    return &defaultMmeProcedureCtxt;
}




