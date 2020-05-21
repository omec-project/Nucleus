/*
 * Copyright 2020-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */
 
#include "componentDb.h"
#include <assert.h>

namespace cmn
{
    void ComponentDb::registerComponent(uint16_t componentId, ComponentIf* comp)
    {
	auto ret = componentContainer.insert(std::pair < uint16_t,
                             ComponentIf * >(componentId, comp));
	assert(ret.second == true);
    }

    ComponentIf& ComponentDb::getComponent(uint16_t componentId)
    {
        std::map < uint16_t, ComponentIf * >::iterator it;
        it = componentContainer.find (componentId);

	assert( it != componentContainer.end());
	return *(it->second);
    }

    ComponentDb compDb;
}
