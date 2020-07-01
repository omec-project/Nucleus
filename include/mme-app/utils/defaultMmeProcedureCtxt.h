 /*
 * Copyright 2019-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef INCLUDE_MME_APP_UTILS_DEFAULTMMEPROCEDURECTXT_H_
#define INCLUDE_MME_APP_UTILS_DEFAULTMMEPROCEDURECTXT_H_

#include <contextManager/dataBlocks.h>

namespace mme
{
class DefaultMmeProcedureCtxt : public MmeProcedureCtxt
{
public:
    static DefaultMmeProcedureCtxt* Instance();

private:
    DefaultMmeProcedureCtxt();
    ~DefaultMmeProcedureCtxt();
};
}



#endif /* INCLUDE_MME_APP_UTILS_DEFAULTMMEPROCEDURECTXT_H_ */
